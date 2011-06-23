/*
Copyright (C) 2011 Peter Szucs

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#ifndef WEBEMBERLINKER_H
#define WEBEMBERLINKER_H

#include <windows.h>
#include <string>
#include <vector>
#include <boost/thread/thread.hpp>
/**
 * @brief This class will link the DLL and provides an interface for the calls.
 * 
 * This is only needed for windows builds.<br>
 * You need to link with imagehlp.lib or libimagehlp.a library.
 */
class WebEmberLinker{
public:
	WebEmberLinker();
	virtual ~WebEmberLinker();
	/**
	 * @brief Links the DLL.
	 * 
	 * Loads the DLL into the memory and creates pointers for DLL function calls.
	 * After this call, you can use all members starting with "call", like WebEmberLinker::callQuitEmber.
	 * You need to call WebEmberLinker::unlink to free the library from memory.
	 * 
	 * @returns When the function succeeds, it will return 0.
	 */
	int WebEmberLinker::link();

	/**
	 * @brief Links the DLL.
	 * 
	 * Loads the DLL into the memory and creates pointers for DLL function calls.
	 * After this call, you can use all members starting with "call", like WebEmberLinker::callQuitEmber.
	 * You need to call WebEmberLinker::unlink to free the library from memory.
	 * 
	 * @returns When the function succeeds, it will return 0.
	 */
	void WebEmberLinker::unlink();

	int callRunEmber(HWND hwnd);
	void callQuitEmber();

private:

	/**
	 * @brief Name of the Ember DLL
	 */
	static const char* sDllName; // = "WebEmber.dll";
	
	//interface for the functions to call.
	//there is no parameter check, you need to ensure that the call is correct or not.

	//int __declspec(dllexport) __stdcall StartWebEmber(HWND hwnd);
	static const char* sFuncNameStart; // = "StartWebEmber";
	typedef int ( __stdcall *funcTypeStart) (HWND hwnd);
	funcTypeStart pStartWebEmber;
	
	//void __declspec(dllexport) __stdcall QuitWebEmber();
	static const char* sFuncNameQuit; // = "QuitWebEmber";
	typedef void ( __stdcall *funcTypeQuit) ();
	funcTypeQuit pQuitWebEmber;

	/**
	 * @brief Handle of the dynamically loaded DLL.
	 */
	HMODULE mModuleHandle;

	/**
	 * @brief Mutex to protect DLL memory freeing.
	 */
	boost::mutex mFreeLibraryMutex;

	/**
	 * @brief Lists all mangled ABI names from a DLL.
	 * 
	 * NOTE: you need to link with imagehlp.lib or libimagehlp.a for this function
	 * src: http://stackoverflow.com/questions/4353116/listing-the-exported-functions-of-a-dll
	 */
	void listDLLFunctions(std::string DllName, std::vector<std::string>& slListOfDllFunctions);

	/**
	 * @brief Gets the mangled name from a function name.
	 * 
	 * It will search for the given function name in all mangled names.
	 * For security reasons, it will only succeed when exactly 1 match is found.<br> 
	 * Following rules will ensure that you don't get problems:
	 * - the exported function name can't be a substring of another exported function name
	 * - the exported function name can't be overloaded with multiple parameters
	 */
	std::string getMangledName(std::string fncName, std::string DLLName = sDllName);
	/**
	 * @brief This will trim the path to the last '/' or '\' sign.
	 */
	void trimFilename(char* path,int buffsize = -1);
};

#endif // WEBEMBERLINKER_H