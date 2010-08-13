// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#pragma warning(push)

#pragma warning(disable:4482)

#include <Pseudo\String.hpp>
#include <Pseudo\Exception.hpp>
#include <Pseudo\Environment.hpp>
#include <Pseudo\ValueType.hpp>
#include <Pseudo\TextWriter.hpp>
#include <Pseudo\FileStream.hpp>
#include <Pseudo\StreamWriter.hpp>

#include <varargs.h>

namespace Pseudo
{
    class Coord : public COORD
    {
        public: Coord()
        {
            X = 0;
            Y = 0;
        }
        
        public: Coord(Short X, Short Y)
        {
            this->X = X;
            this->Y = Y;
        }
        
        public: Coord(const COORD& other)
        {
            *this = other;
        }
        
        public: Coord& operator=(const COORD& other)
        {
            this->X = other.X;
            this->Y = other.Y;
            return *this;
        }
        
        public: bool operator==(const COORD& other)
        {
            return (this->X == other.X && this->Y == other.Y);
        }
        
        public: bool operator!=(const COORD& other)
        {
            return !(*this == other);
        }
        
        public: COORD Right(Short bufferWidth, Short dX)
        {
            Coord co;
            
            co.X = X + dX;
            
            Short dY = (co.X / bufferWidth);

            co.X -= dY * bufferWidth;            
            co.Y = Y + dY;
            
            return co;
        }
        
        public: COORD Right(Short bufferWidth)
        {
            if (X == bufferWidth - 1)
                return Coord(0, Y + 1);
            else
                return Coord(X + 1, Y);
        }
        
        public: COORD Left(Short bufferWidth)
        {
            if (X == 0)
                return Coord(bufferWidth - 1, Y - 1);
            else
                return Coord(X - 1, Y);
        }
        
        public: static Int Distance(COORD start, COORD end, Int bufferWidth)
        {
            if (start.Y == end.Y)
                return end.X - start.X;
            else
                return (bufferWidth - 1 - start.X) + ((end.Y - start.Y) * bufferWidth) + end.X;
        }
    };

    class ConsoleScreenBufferInfo : public CONSOLE_SCREEN_BUFFER_INFO
    {
        public: ConsoleScreenBufferInfo()
        {
            ::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE), this);
        }
        
        public: Short get_BufferWidth() { return this->dwSize.X; }
        public: Short get_BufferHeight() { return this->dwSize.Y; }
        public: Coord get_CursorPos() { return this->dwCursorPosition; }
    };

	class Console
	{
	    private: static HANDLE hStdIn;
	    private: static HANDLE hStdOut;
	    private: static HANDLE hStdErr;
	
		private: Console()
		{
		}

		private: virtual ~Console()
		{
		}

        public: static void Write(const String& s)
        {
            get_Out()->Write(s);
        }

		public: static void WriteLine()
		{
			get_Out()->WriteLine();
		}

		public: static void WriteLine(const String& s)
		{
			get_Out()->WriteLine(s);
		}

		public: static void WriteLine(const Char* p, ...)
		{
			va_list argList;
			va_start(argList, p);

			get_Out()->WriteLine(String::Format(p, argList));
		} 

        public: static void DirectWrite(const String& s)
        {
            DWORD written; 
            ::WriteConsole(hStdOut, s.get_Ptr(), s.get_Length(), &written, NULL); 
        }

		public: static TextWriter* get_Out()
		{
			// TODO-johnls-5/11/2008: The encoding should really be the code page of the console, not ASCII
			static StreamWriter out(PSEUDO_NEW FileStream(hStdOut, FileMode::Append), Encoding::get_ASCII());
			
			return &out;
		}

		public: static TextWriter* get_Error()
		{
			// TODO-johnls-5/11/2008: The encoding should really be the code page of the console, not ASCII
			static StreamWriter out(PSEUDO_NEW FileStream(hStdErr, FileMode::Append), Encoding::get_ASCII());
			
			return &out;
		}

		public: static Bool get_EnableProcessedOutput()
		{
            DWORD dwMode;
            
            ::GetConsoleMode(hStdOut, &dwMode);
            
            return (dwMode & ENABLE_PROCESSED_OUTPUT) != 0;
		}
		
		public: static void set_EnableProcessedOutput(Bool value)
		{
            DWORD dwMode;
            
            ::GetConsoleMode(hStdOut, &dwMode);
            
            if (value)
                ::SetConsoleMode(hStdOut, dwMode | ENABLE_PROCESSED_OUTPUT);
            else
                ::SetConsoleMode(hStdOut, dwMode & ~ENABLE_PROCESSED_OUTPUT);
		}
		
		public: static void GetScreenBufferInfo(CONSOLE_SCREEN_BUFFER_INFO& csbi)
		{
		    ::GetConsoleScreenBufferInfo(hStdOut, &csbi);
		}
		
		public: static void SetWindowInfo(Bool absolute, const SMALL_RECT& rect)
		{
		    ::SetConsoleWindowInfo(hStdOut, absolute, &rect);
		}
		
		public: static void SetCursorPosition(COORD coord)
		{
		    ::SetConsoleCursorPosition(hStdOut, coord);
		}
		
		public: static void FillOutputCharacter(Char ch, Int count, COORD coord)
		{
		    DWORD written;
		    
            ::FillConsoleOutputCharacter(hStdOut, ch, count, coord, &written);
		}
		
		public: static void FillOutputAttribute(WORD attrs, Int count, COORD coord)
		{
		    DWORD written;
		    
            ::FillConsoleOutputAttribute(hStdOut, attrs, count, coord, &written);
		}
		
		public: static void ReadInputRecord(INPUT_RECORD& ir)
		{
		    DWORD eventsRead;
            BOOL ok = ::ReadConsoleInputW(hStdIn, &ir, 1, &eventsRead);
            
            if (!ok)
                throw Win32Exception(::GetLastError());
		}
		
		public: static Char ReadOutputCharacter(COORD coord)
		{
		    Char c;
		    DWORD charsRead;
		    
            ::ReadConsoleOutputCharacter(hStdOut, &c, 1, coord, &charsRead);
            
            return c;
		}
		
		public: static String ReadOutputString(COORD coord, Int length)
		{
		    DWORD charsRead;
		    StringBuilder sb(length);
		    
            ::ReadConsoleOutputCharacter(hStdOut, sb.get_Ptr(), length, coord, &charsRead);
            
            *(sb.get_Ptr() + charsRead) = 0;
            
            return sb.ToString();
		}
		
		public: static void ScrollScreenBuffer(const SMALL_RECT& scrollRect, const SMALL_RECT& clipRect, COORD destCoord, const CHAR_INFO& fillChar)
		{
		    ::ScrollConsoleScreenBuffer(hStdOut, &scrollRect, &clipRect, destCoord, &fillChar);
		}
		
		public: static void WriteOutputCharacter(Char c, COORD writeCoord)
		{
		    DWORD charsWritten;

            // TODO-johnls-5/27/2008: Deal with scrolling the screen buffer
		    
		    ::WriteConsoleOutputCharacterW(hStdOut, &c, 1, writeCoord, &charsWritten);
		}

		public: static void WriteOutputCharacter(Char c)
		{
		    ConsoleScreenBufferInfo csbi;
		    Int charsWritten;

            // TODO-johnls-5/27/2008: Deal with scrolling the screen buffer
		    
		    ::WriteConsoleOutputCharacterW(hStdOut, &c, 1, csbi.get_CursorPos(), (DWORD*)&charsWritten);
		}
		
		public: static void WriteOutputString(String& s, COORD writeCoord)
		{
		    Int written;
		    
		    WriteOutputString(s, writeCoord, (Int&)written);
		}
		
		public: static void WriteOutputString(String& s, COORD writeCoord, Int& written)
		{
		    // TODO-johnls-5/27/2008: Deal with scrolling the screen buffer
		    
		    ::WriteConsoleOutputCharacterW(hStdOut, s.get_Ptr(), s.get_Length(), writeCoord, (DWORD*)&written);
		}
		
		public: static void WriteOutputCRLF()
		{
		    ConsoleScreenBufferInfo csbi;
            Coord co(csbi.get_CursorPos());
            
            co.X = 0;
            co.Y++;

            // TODO-johnls-5/27/2008: Deal with scrolling the screen buffer
            
            SetCursorPosition(co);            
		}

		public: static void FlushInputBuffer()
		{
		    ::FlushConsoleInputBuffer(hStdIn);
		}
		
		public: static void WriteInputRecord(const INPUT_RECORD& ir)
		{
		    DWORD eventsWritten;
		    ::WriteConsoleInputW(hStdIn, &ir, 1, &eventsWritten);
		}
		
		public: static void GetCursorInfo(CONSOLE_CURSOR_INFO& cci)
		{
		    ::GetConsoleCursorInfo(hStdOut, &cci);
		}
		
		public: static void SetCursorInfo(const CONSOLE_CURSOR_INFO& cci)
		{
		    ::SetConsoleCursorInfo(hStdOut, &cci);
		}
	};

    // Globals.
    __declspec(selectany) HANDLE Console::hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
    __declspec(selectany) HANDLE Console::hStdIn = ::GetStdHandle(STD_INPUT_HANDLE);
    __declspec(selectany) HANDLE Console::hStdErr = ::GetStdHandle(STD_ERROR_HANDLE);
}

#pragma warning(pop)
