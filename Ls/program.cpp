#include <Pseudo\Library.hpp>

using namespace Pseudo;

class CommandLineArg
{
private:
	String name;
	String value;	

public:
	CommandLineArg()
	{
		Construct(String::get_Empty(), String::get_Empty());
	}
	CommandLineArg(const CommandLineArg& other)
	{
		Construct(other);
	}
	void Construct(const CommandLineArg& other)
	{
		this->name = other.name;
		this->value = other.value;
	}
	void Construct(String name, String value)
	{
		this->name = name;
		this->value = value;
	}
	String GetName()
	{
		return this->name;
	}
	String GetValue()
	{
		return this->value;
	}	
};

void GetArguments(Char* pCmdLine, ArrayList<CommandLineArg>& args)
{
	String cmdLine;
	Int i;
	Int cmdLineLen;
	String name;
	String value;
	CommandLineArg arg;
	Int j;
	
	cmdLine = pCmdLine;
	cmdLineLen = cmdLine.get_Length();
	i = 0;

	// Skip over the program name; it may or may not be quoted
	if (cmdLine[i] == L'"')
	{
		i = 1;
		while (i < cmdLineLen && cmdLine[i] != L'"')
			i++;
		// Skip over last '"'		
		i++;
	}
	else
	{
		while (i < cmdLineLen && cmdLine[i] != L' ')
			i++;
	}
		
	while (i < cmdLineLen)
	{
		// Skip blanks
		while (i < cmdLineLen && cmdLine[i] == L' ')
			i++;
		
		// If nothing left we are done
		if (i == cmdLineLen)
			break;
		
		// Is this an argument
		if (cmdLine[i] == L'-')
		{
			i++;  // Skip the '-'
			j = i; // Remember the start
			
			// Go to end of text
			while (i < cmdLineLen && cmdLine[i] != L' ')
				i++;
				
			name = cmdLine.SubString(j, i - j); // Grab the argument
			
			// TODO: Single '-' is an error?
			// TODO: Support -name:value format

			// Skip blanks again
			while (i < cmdLineLen && cmdLine[i] == L' ')
				i++;

			// If the next thing is another argument, we are done with this one
			if (i != cmdLineLen && cmdLine[i] == L'-')
			{
				arg.Construct(name, String::get_Empty());
				args.Add(arg);
				continue;
			}
		}

		// Extract the argument value and associate with any argument name
		j = i;
		
		while (i < cmdLineLen && cmdLine[i] != L' ')
			i++;
			
		value = cmdLine.SubString(j, i - j); // Grab the value
		
		arg.Construct(name, value);
		args.Add(arg);
	}
}


void ListFileNames(String glob, bool singleColumn)
{
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;
	ArrayList<String> fileNames;

	fileNames.set_Capacity(100);

	hFind = ::FindFirstFileExW(glob, FindExInfoStandard, &findFileData, FindExSearchNameMatch, NULL, 0);

	if (hFind == INVALID_HANDLE_VALUE) 
		return;

	Int maxFileNameLen = 0;
	
	do
	{
		String fileName;
		
		fileName = findFileData.cFileName;
		
		fileNames.Add(fileName);
		
		Int fileNameLen = fileName.get_Length();
		
		if (fileNameLen > maxFileNameLen)
			maxFileNameLen = fileNameLen;
	}
	while (::FindNextFileW(hFind, &findFileData));

	::FindClose(hFind);

	// TODO-johnls-2010-7: The names always seem to come back sorted ascending alphabetically - is this guaranteed by FFF/FNF?

	// Print in single column
	if (singleColumn)
	{
		for (int i = 0; i < fileNames.get_Count(); i++)
		{
			Console::WriteLine(fileNames[i]);
		}
	}
	else
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		Console::GetScreenBufferInfo(csbi);

		Int adjustedBufferWidth = csbi.dwSize.X - 1;
		// TODO-johnls-2010-7: Check what Unix does for min width?
		Int slotWidth = maxFileNameLen + 1;
		Int offset = 0;
		
		for (int i = 0; i < fileNames.get_Count(); i++)
		{
			Console::Write(fileNames[i]);
			Console::Write(String(L' ', slotWidth - fileNames[i].get_Length()));

			offset += slotWidth;

			// If the next slot will hit the edge of the screen, wrap
			if (offset + slotWidth >= adjustedBufferWidth)
			{
				Console::WriteLine();
				offset = 0;
			}
		}
	}
}

void _cdecl wmain(int argc, WCHAR *argv[])
{
	ArrayList<CommandLineArg> args;
	Bool singleColumn;

	GetArguments(::GetCommandLineW(), args);
	singleColumn = false;

	// Get all the options
	for (int i = 0; i < args.get_Count(); i++)
	{
		if (args[i].GetName() == L"1")
			singleColumn = true;
	}

	bool haveListings = false;

	// Perform all the listings
	for (int i = 0; i < args.get_Count(); i++)
	{
		if (args[i].GetName() == String::get_Empty())
		{
			haveListings = true;
			ListFileNames(args[i].GetValue(), singleColumn);
		}
	}	
	
	// If there are no listings, just use '*'
	if (!haveListings)
	{
		ListFileNames(L"*", singleColumn);
	}
		
}
