/* ************************************************************************
 * Copyright 2013 Advanced Micro Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ************************************************************************/


#pragma once
#if !defined( amd_unicode_h )
#define amd_unicode_h
#include <ostream>
#include <iostream>

//	Typedefs to support unicode and ansii compilation
#if defined( _UNICODE )
	typedef std::wstring		tstring;
	typedef std::wstringstream	tstringstream;
	typedef std::wifstream		tifstream;
	typedef std::wofstream		tofstream;
	typedef std::wfstream		tfstream;
	static std::wostream&	tout	= std::wcout;
	static std::wostream&	terr	= std::wcerr;
#else
	typedef std::string tstring;
	typedef std::stringstream tstringstream;
	typedef std::ifstream		tifstream;
	typedef std::ofstream		tofstream;
	typedef std::fstream		tfstream;
	static std::ostream&	tout	= std::cout;
	static std::ostream&	terr	= std::cerr;
#endif

#if defined(UNICODE)
  #define _TCHAR wchar_t
  #define _T(x)    L ## x
  #define _tmain wmain
#else
  #define _TCHAR char
  #define _T(x)    x
  #define _tmain main
#endif

#endif
