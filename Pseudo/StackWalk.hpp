// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#include <Pseudo\Compiler.hpp>

// Windows
//#pragma pack(push, 8) // We set the packing to 64 bits to avoid problems with DbgHelp.dll
//#define _IMAGEHLP64
#include <dbghelp.h>
#include <tlhelp32.h>
//#pragma pack(pop)

// Pseudo
#include <Pseudo\String.hpp>
#include <Pseudo\ArrayList.hpp>
#include <Pseudo\Macros.hpp>
#include <Pseudo\Trace.hpp>
#include <Pseudo\Exception.hpp>
#include <Pseudo\CountedPtr.hpp>
#include <Pseudo\Encoding.hpp>
#include <Pseudo\File.hpp>
#include <Pseudo\Path.hpp>

namespace Pseudo
{
	const int MAX_SYMBOL_NAME_LEN = 1024;

	/// <summary>
	/// This structure contains information about one function 'frame' on the stack.  This quality of this
	/// information is largely dependent on whether full and up-to-date debug (.pdb) symbols are available for
	/// the module containing the function.
	/// </summary>
	struct StackFrame
	{
	public:
		// Default constructor
		StackFrame()
		{
			line = 0;
			address = 0;
			offset = 0;
		}

		// Copy constructor
		StackFrame(StackFrame const& rhs)
		{
			moduleName = rhs.moduleName;
			fileName = rhs.fileName;
			methodName = rhs.methodName;
			line = rhs.line;
			address = rhs.address;
			offset = rhs.offset;
		}

		String moduleName;	// Module name
		String fileName;	// Source file
		String methodName;	// Method name
		Int line;			// Line #
		void* address;		// Address of first instruction in the method
		IntPtr offset;		// Offset of the first instruction in the line from the method base
							// or the offset of the faulting instruction from the method base if no line information
							// or the offset of the faulting instruction from the base of the module if no module
							// information
	};

	typedef struct
	{
		DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64_V2)
		DWORD64  BaseOfImage;            // base load address of module
		DWORD    ImageSize;              // virtual size of the loaded module
		DWORD    TimeDateStamp;          // date/time stamp from pe header
		DWORD    CheckSum;               // checksum from the pe header
		DWORD    NumSyms;                // number of symbols in the symbol table
		SYM_TYPE SymType;                // type of symbols loaded
		CHAR     ModuleName[32];         // module name
		CHAR     ImageName[256];         // image name
		CHAR     LoadedImageName[256];   // symbol file name
	} IMAGEHLP_MODULE64_V2;

	class StackWalker
	{
		// DbgHelp API's
		private: typedef BOOL (__stdcall *SymCleanupProc)(IN HANDLE hProcess);
		private: typedef PVOID (__stdcall *SymFunctionTableAccess64Proc)(HANDLE hProcess, DWORD64 AddrBase);
		private: typedef BOOL (__stdcall *SymGetLineFromAddr64Proc)( IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE64 Line);
		private: typedef DWORD64 (__stdcall *SymGetModuleBase64Proc)(IN HANDLE hProcess, IN DWORD64 dwAddr);
		private: typedef BOOL (__stdcall *SymGetModuleInfo64Proc)(IN HANDLE hProcess, IN DWORD64 dwAddr, OUT IMAGEHLP_MODULE64_V2 *ModuleInfo);
		private: typedef DWORD (__stdcall *SymGetOptionsProc)(VOID);
		private: typedef BOOL (__stdcall *SymFromAddrProc)(
			IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PDWORD64 pdwDisplacement, OUT PSYMBOL_INFO Symbol);
		private: typedef BOOL (__stdcall *SymInitializeProc)(IN HANDLE hProcess, IN const Char* UserSearchPath, IN BOOL fInvadeProcess);
		private: typedef DWORD64 (__stdcall *SymLoadModule64Proc)(
			IN HANDLE hProcess, IN HANDLE hFile, IN PSTR ImageName, IN PSTR ModuleName, IN DWORD64 BaseOfDll, IN DWORD SizeOfDll);
		private: typedef DWORD (__stdcall *SymSetOptionsProc)(IN DWORD SymOptions);
		private: typedef BOOL (__stdcall *StackWalk64Proc)(
			DWORD MachineType,
			HANDLE hProcess,
			HANDLE hThread,
			LPSTACKFRAME64 StackFrame,
			PVOID ContextRecord,
			PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
			PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
			PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
			PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress);
		private: typedef DWORD (__stdcall WINAPI *UnDecorateSymbolNameProc)(
			PCSTR DecoratedName, PSTR UnDecoratedName, DWORD UndecoratedLength, DWORD Flags);

		// ToolHelp API's
		private: typedef HANDLE (__stdcall *CreateToolhelp32SnapshotProc)(DWORD dwFlags, DWORD th32ProcessID);
		private: typedef BOOL (__stdcall *Module32FirstWProc)(HANDLE hSnapshot, LPMODULEENTRY32W pme);
		private: typedef BOOL (__stdcall *Module32NextWProc)(HANDLE hSnapshot, LPMODULEENTRY32W pme);

		private: SymCleanupProc _SymCleanup;
		private: SymFunctionTableAccess64Proc _SymFunctionTableAccess64;
		private: SymGetLineFromAddr64Proc _SymGetLineFromAddr64;
		private: SymGetModuleBase64Proc _SymGetModuleBase64;
		private: SymGetModuleInfo64Proc _SymGetModuleInfo64;
		private: SymGetOptionsProc _SymGetOptions;
		private: SymFromAddrProc _SymFromAddr;
		private: SymInitializeProc _SymInitialize;
		private: SymLoadModule64Proc _SymLoadModule64;
		private: SymSetOptionsProc _SymSetOptions;
		private: StackWalk64Proc _StackWalk64;
		private: UnDecorateSymbolNameProc _UnDecorateSymbolName;

		private: CreateToolhelp32SnapshotProc _CreateToolhelp32Snapshot;
		private: Module32FirstWProc _Module32FirstW;
		private: Module32NextWProc _Module32NextW;

		private: HANDLE hProcess;
		private: HMODULE hDbgHelp;
		private: HMODULE hKernel32;
		private: bool initialized;

		public: StackWalker()
		{
		}

		private: void Reset()
		{
			hDbgHelp = NULL;
			hKernel32 = NULL;

			_SymCleanup = NULL;
			_SymFunctionTableAccess64 = NULL;
			_SymGetLineFromAddr64 = NULL;
			_SymGetModuleBase64 = NULL;
			_SymGetModuleInfo64 = NULL;
			_SymGetOptions = NULL;
			_SymFromAddr = NULL;
			_SymInitialize = NULL;
			_SymLoadModule64 = NULL;
			_SymSetOptions = NULL;
			_StackWalk64 = NULL;
			_UnDecorateSymbolName = NULL;

			_CreateToolhelp32Snapshot = NULL;
			_Module32FirstW = NULL;
			_Module32NextW = NULL;

			initialized = false;
		}

		public: bool Initialize()
		{
			String fileName;
			String programFiles;
			String systemRoot;

			if (initialized)
				return true;

			// This had better not fail!
			hKernel32 = LoadLibrary(L"kernel32.dll");

			ArrayList<String> path;

			if (Environment::GetVariable(L"ProgramFiles", programFiles))
			{
			    // TODO-johnls-4/15/2008: Is there a better way to get this path?
			    path.Add(programFiles + L"\\Debugger");
			    path.Add(programFiles + L"\\Debugging Tools for Windows");
				path.Add(programFiles + L"\\Debugging Tools for Windows 64-Bit");
			    path.Add(programFiles + L"\\Microsoft Visual Studio 9.0\\Common7\\IDE");
			    path.Add(programFiles + L"\\Microsoft Visual Studio 9.0\\Team Tools\\Performance Tools\\");
			    path.Add(programFiles + L"\\Microsoft Visual Studio 8\\Common7\\IDE");
			    path.Add(programFiles + L"\\Microsoft Visual Studio 8\\Team Tools\\Performance Tools\\");
			}

			if (Environment::GetVariable(L"SystemRoot", systemRoot))
			{
                // TODO: What happens on x64?  Eeeek!
			    path.Add(programFiles + L"\\System32");
			    path.Add(programFiles + L"\\SysWow64");
            }

            for (Int i = 0; i < path.get_Count(); i++)
            {
                fileName = path[i] + L"\\dbghelp.dll";

			    if (File::Exists(fileName))
			    {
				    this->hDbgHelp = LoadLibrary(fileName);

        			if (this->hDbgHelp != NULL)
        			{
        			    break;
        			}
			    }
			}

            // We didn't find one - hell must have frozen over
			if (this->hDbgHelp == NULL)
				return false;

			// Get DbgHelp API's
			_SymInitialize = (SymInitializeProc)GetProcAddress(this->hDbgHelp, "SymInitialize");
			_SymCleanup = (SymCleanupProc)GetProcAddress(this->hDbgHelp, "SymCleanup");
			_StackWalk64 = (StackWalk64Proc)GetProcAddress(this->hDbgHelp, "StackWalk64");
			_SymGetOptions = (SymGetOptionsProc)GetProcAddress(this->hDbgHelp, "SymGetOptions");
			_SymSetOptions = (SymSetOptionsProc)GetProcAddress(this->hDbgHelp, "SymSetOptions");
			_SymFunctionTableAccess64 = (SymFunctionTableAccess64Proc)GetProcAddress(this->hDbgHelp, "SymFunctionTableAccess64");
			_SymGetLineFromAddr64 = (SymGetLineFromAddr64Proc)GetProcAddress(this->hDbgHelp, "SymGetLineFromAddr64");
			_SymGetModuleBase64 = (SymGetModuleBase64Proc)GetProcAddress(this->hDbgHelp, "SymGetModuleBase64");
			_SymGetModuleInfo64 = (SymGetModuleInfo64Proc)GetProcAddress(this->hDbgHelp, "SymGetModuleInfo64");
			_SymFromAddr = (SymFromAddrProc)GetProcAddress(this->hDbgHelp, "SymFromAddr");
			_UnDecorateSymbolName = (UnDecorateSymbolNameProc)GetProcAddress(this->hDbgHelp, "UnDecorateSymbolName");
			_SymLoadModule64 = (SymLoadModule64Proc)GetProcAddress(this->hDbgHelp, "SymLoadModule64");

			if (
				_SymInitialize == NULL ||
				_SymCleanup == NULL ||
				_StackWalk64 == NULL ||
				_SymGetOptions == NULL ||
				_SymSetOptions == NULL ||
				_SymFunctionTableAccess64 == NULL ||
				_SymGetLineFromAddr64 == NULL ||
				_SymGetModuleBase64 == NULL ||
				_SymGetModuleInfo64 == NULL ||
				_SymFromAddr == NULL ||
				_UnDecorateSymbolName == NULL ||
				_SymLoadModule64 == NULL)
			{
				FreeLibrary(hDbgHelp);
				hDbgHelp = NULL;
				_SymCleanup = NULL;
				return false;
			}

			// Get Toolhelp API's
			_CreateToolhelp32Snapshot = (CreateToolhelp32SnapshotProc)GetProcAddress(this->hKernel32, "CreateToolhelp32Snapshot");
			_Module32FirstW = (Module32FirstWProc)GetProcAddress(this->hKernel32, "Module32FirstW");
			_Module32NextW = (Module32NextWProc)GetProcAddress(this->hKernel32, "Module32NextW");

			if (_SymInitialize(GetCurrentProcess(), GetSymbolPath(), FALSE) == FALSE)
			{
				return false;
			}

			DWORD symOptions = _SymGetOptions();

			symOptions |= SYMOPT_LOAD_LINES;
			symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;

			symOptions = _SymSetOptions(symOptions);

			return (initialized = true);
		}

		public: void Uninitialize()
		{
			if (_SymCleanup != NULL)
				_SymCleanup(GetCurrentProcess());

			if (this->hDbgHelp != NULL)
				FreeLibrary(this->hDbgHelp);

			if (this->hKernel32 != NULL)
				FreeLibrary(this->hKernel32);

			Reset();
		}

		private: String GetSymbolPath()
		{
			StringBuilder path(4096);

			path.Append(L".;");
			path.Append(Environment::get_CurrentDirectory());
			path.Append(L";");
			path.Append(Path::GetDirectoryName(Environment::get_ModuleFileName()));
			path.Append(L";");

			String var;

			if (Environment::GetVariable(L"_NT_SYMBOL_PATH", var))
			{
				path.Append(var);
				path.Append(L";");
			}

			if (Environment::GetVariable(L"_NT_ALTERNATE_SYMBOL_PATH", var))
			{
				path.Append(var);
				path.Append(L";");
			}

			if (Environment::GetVariable(L"SYSTEMROOT", var))
			{
				path.Append(var);
				path.Append(L";");

				// Also add the "system32"-directory:
				var += L"\\system32";
				path.Append(var);
				path.Append(L";");
			}

			if (Environment::GetVariable(L"SYSTEMDRIVE", var))
			{
				path.Append(L"SRV*");
				path.Append(var);
				path.Append(L"\\websymbols*http://msdl.microsoft.com/download/symbols;");
			}
			else
				path.Append(L"SRV*c:\\websymbols*http://msdl.microsoft.com/download/symbols;");

			return path.ToString();
		}

		/// <summary>
		/// This method returns information about the program stack from the point at which the method is
		/// called.  The first item on the stack is usually an address outside of any module that is
		/// a side effect of the mechanism used to obtain a stack walk for the currently executing thread.  It
		/// can safely be ignored.
		/// </summary>
		/// <param name="stack">An array that will receive the stack information</param>
		public: void GetStack(ArrayList<StackFrame>& stack)
		{
			ArrayList<void*> addresses;
			HANDLE hProcess = ::GetCurrentProcess();
			HANDLE hThread = ::GetCurrentThread();

			stack.Clear();

			if (!Initialize())
				return;

			CONTEXT context;

			::RtlCaptureContext(&context);

			STACKFRAME64 frame64;

			DWORD machineType = InitStackFrameFromContext(&context, &frame64);

			for (Int frameNum = 0; ; frameNum++)
			{
				if (!::StackWalk64(
					machineType,
					hProcess,
					hThread,
					&frame64,
					&context,
					NULL,
					_SymFunctionTableAccess64,
					_SymGetModuleBase64,
					NULL))
				{
					break;
				}

				addresses.Add((void*)frame64.AddrPC.Offset);

				// Catch any stack loop
				if (frame64.AddrPC.Offset == frame64.AddrReturn.Offset)
				{
					break;
				}
			}

			LoadModuleInfo(hProcess);

			for (Int i = 0; i < addresses.get_Count(); i++)
			{
				StackFrame frame;

				ResolveStackFrame(hProcess, addresses[i], frame);
				stack.Add(frame);
			}
		}

		public: void Release()
		{
			Uninitialize();
		}

		private: DWORD InitStackFrameFromContext(CONTEXT *pContext, STACKFRAME64 *pStackFrame)
		{
			ZeroMemory(pStackFrame, sizeof(*pStackFrame));

			#if defined(_M_IX86)
			pStackFrame->AddrPC.Offset      = pContext->Eip;
			pStackFrame->AddrPC.Mode        = AddrModeFlat;
			pStackFrame->AddrStack.Offset   = pContext->Esp;
			pStackFrame->AddrStack.Mode     = AddrModeFlat;
			pStackFrame->AddrFrame.Offset   = pContext->Ebp;
			pStackFrame->AddrFrame.Mode     = AddrModeFlat;
			return IMAGE_FILE_MACHINE_I386;

			#elif defined(_M_AMD64)
			pStackFrame->AddrPC.Offset      = pContext->Rip;
			pStackFrame->AddrPC.Mode        = AddrModeFlat;
			pStackFrame->AddrStack.Offset   = pContext->Rsp;
			pStackFrame->AddrStack.Mode     = AddrModeFlat;
			return IMAGE_FILE_MACHINE_AMD64;

			#else
			#error Unknown Target Machine
			#endif
		}

		private: void LoadModuleInfo(HANDLE hProcess)
		{
			HANDLE hSnap = _CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ::GetProcessId(hProcess));

			MODULEENTRY32 me;

			me.dwSize = sizeof(me);

			if (hSnap != (HANDLE)-1)
			{
				BOOL keepGoing = _Module32FirstW(hSnap, &me);

				while (keepGoing)
				{
					Array<Byte> moduleBytes;
					Array<Byte> exePathBytes;

					Encoding::get_ASCII().GetBytesWithNull(me.szModule, moduleBytes);
					Encoding::get_ASCII().GetBytesWithNull(me.szExePath, exePathBytes);

					DWORD64 base = _SymLoadModule64(
						hProcess,
						NULL,
						(char*)exePathBytes.get_Ptr(),
						(char*)moduleBytes.get_Ptr(),
						(DWORD64)me.modBaseAddr,
						me.modBaseSize);

					(void)base;

					keepGoing = _Module32NextW(hSnap, &me);
				}

				::CloseHandle(hSnap);
			}
		}

		private: void ResolveStackFrame(HANDLE hProcess, void* addr, StackFrame& frame)
		{
			IMAGEHLP_MODULE64_V2 mi = {sizeof(IMAGEHLP_MODULE64_V2)};
			CHAR methodName[MAX_SYMBOL_NAME_LEN];
			DWORD64 offset;
			CHAR sourceFile[MAX_PATH];
			DWORD lineNumber;
			HRESULT hr;
			size_t length;

			// Filling in this field is easy; the others not so much
			frame.address = addr;

			DWORD64 baseAddr = _SymGetModuleBase64(hProcess, (DWORD64)addr);

			if (baseAddr == 0 || !_SymGetModuleInfo64(hProcess, baseAddr, &mi))
			{
				#if defined(_M_IA64)
				hr = StringCchPrintfA(methodName, ARRAY_SIZE(methodName), "0x%016x", (UINT_PTR)addr);
				#else
				hr = StringCchPrintfA(methodName, ARRAY_SIZE(methodName), "0x%08x", (UINT_PTR)addr);
				#endif

				THROW_BAD_HRESULT(hr);

				hr = StringCchLengthA(methodName, INT_MAX, &length);

				THROW_BAD_HRESULT(hr);

				frame.methodName = Encoding::get_ASCII().GetString((Byte*)methodName, static_cast<Int>(length));
				frame.moduleName = L"";
				frame.fileName = L"";
				frame.offset = 0;
				frame.address = addr;
				frame.line = 0;

				return;
			}
			else
			{
				hr = StringCchLengthA(mi.ImageName, INT_MAX, &length);

				THROW_BAD_HRESULT(hr);

				frame.moduleName = Encoding::get_ASCII().GetString((Byte*)mi.ImageName, static_cast<Int>(length));
			}

			if (!GetMethodDetails(hProcess, (DWORD64)addr,
				ARRAY_SIZE(methodName), methodName, ARRAY_SIZE(sourceFile), sourceFile, &lineNumber, &offset))
			{
				frame.offset = (IntPtr)((DWORD64)addr - mi.BaseOfImage);
				frame.address = (void*)mi.BaseOfImage;
			}
			else
			{
				hr = StringCchLengthA(methodName, INT_MAX, (size_t*)&length);

				THROW_BAD_HRESULT(hr);

				frame.methodName = Encoding::get_ASCII().GetString((Byte*)methodName, static_cast<Int>(length));

				hr = StringCchLengthA(sourceFile, INT_MAX, (size_t*)&length);

				THROW_BAD_HRESULT(hr);

				frame.fileName = Encoding::get_ASCII().GetString((Byte*)sourceFile, static_cast<Int>(length));
				frame.offset = (IntPtr)offset;
				frame.address = (BYTE*)addr - frame.offset;
				frame.line = lineNumber;
			}
		}

		// This function needs to be C++ free so that we can use SEH (no destructors)
		private: inline bool GetMethodDetails(
			HANDLE hProcess,
			DWORD64 addr,
			int methodNameSize,
			__out_ecount_z(methodNameSize) char* methodName,
			int sourceFileSize,
			__out_ecount_z(sourceFileSize) char* sourceFile,
			PDWORD lineNumber,
			PDWORD64 offset)
		{
			DEBUG_ASSERT(methodNameSize > 1);
			DEBUG_ASSERT(sourceFileSize > 1);

			// Make space for terminators
			methodNameSize--;
			sourceFileSize--;

			*offset = 0;
			*methodName = 0;
			*sourceFile = 0;
			*lineNumber = 0;

			__try
			{
				int len = sizeof(SYMBOL_INFO) + MAX_SYMBOL_NAME_LEN * sizeof(char);
				PSYMBOL_INFO symInfo = (PSYMBOL_INFO)STACK_ALLOC(len);

				::ZeroMemory(symInfo, len);

				symInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
				symInfo->MaxNameLen = MAX_SYMBOL_NAME_LEN;

				if (_SymFromAddr(hProcess, addr, offset, symInfo))
				{
					char undecName[MAX_SYMBOL_NAME_LEN];

					if (_UnDecorateSymbolName(symInfo->Name, undecName, ARRAY_SIZE(undecName),
						UNDNAME_NO_MS_KEYWORDS | UNDNAME_NO_ACCESS_SPECIFIERS))
					{
						StringCchCopyA(methodName, methodNameSize, undecName);
					}
					else
					{
						StringCchCopyA(methodName, methodNameSize, symInfo->Name);
					}

					// Terminate the name
					methodName[methodNameSize] = 0;

					// Get line info. if there is any
					IMAGEHLP_LINE64 line = {sizeof(IMAGEHLP_LINE64)};
					DWORD lineOffset = 0;

					if (_SymGetLineFromAddr64(hProcess, addr, &lineOffset, &line))
					{
						*lineNumber = line.LineNumber;
						StringCchCopyA(sourceFile, sourceFileSize, line.FileName);
						sourceFile[sourceFileSize] = 0;
						offset -= lineOffset;
					}
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return false;
			}

			return true;
		}

		public: static StackWalker& get_StackWalker()
		{
			return stackwalker;
		}

		public: static StackWalker stackwalker;
	};

	__declspec(selectany) StackWalker StackWalker::stackwalker;
}

#pragma warning(pop)
