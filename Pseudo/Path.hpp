// Copyright (c) John Lyon-Smith. All rights reserved.

#pragma once

#ifndef __PSEUDO_PATH_HPP__
#define __PSEUDO_PATH_HPP__

#pragma warning(push)

#include <Pseudo\ValueType.hpp>

namespace Pseudo
{
	/// <summary>
	/// </summary>
	class Path
	{
		private: Path()
		{
		}

		public: static String GetDirectoryName(String const & path)
		{
			Int index = path.LastIndexOf('\\');

			if (index == -1)
				return String::get_Empty();
			else if (index == path.get_Length() - 1)
			    return path;
			else
				return path.SubString(0, index);
		}

		public: static String GetFileName(String const & path)
		{
			Int index = path.LastIndexOf('\\');

			if (index == -1)
				return path;
			else if (index == path.get_Length() - 1)
				return String::get_Empty();
			else
				return path.SubString(index + 1);
		}

		public: static String GetFileNameWithoutExtension(String const & path)
		{
			String fileName(GetFileName(path));
			
			Int index = fileName.LastIndexOf('.');

			if (index == -1)
				return fileName;
			else if (index == 0)
				return String::get_Empty();
			else
				return fileName.SubString(0, index);
		}
		
		public: static String Combine(String part1, String part2)
		{
		    if (!part1.EndsWith('\\'))
		        part1 += '\\';
		    
		    return part1 + part2;
		}
	};
}

#pragma warning(pop)

#endif // __PSEUDO_PATH_HPP__
