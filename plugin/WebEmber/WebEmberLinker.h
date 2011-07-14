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

#ifdef _WIN32
#include <windows.h>
#endif

#include <boost/thread/thread.hpp>

/**
 * @brief This class will link the library and provides an interface for the calls.
 *
 * This is only needed for windows builds.<br>
 * You need to link with imagehlp.lib or libimagehlp.a library.
 */
class WebEmberLinker
{
public:
	WebEmberLinker();
	virtual ~WebEmberLinker();

	/**
	 * @brief Links the library.
	 *
	 * Loads the library into the memory and creates pointers for library function calls.
	 * After this call, you can use all members starting with "call", like WebEmberLinker::callQuitEmber.
	 * You need to use WebEmberLinker::unlink to free the library from memory.
	 *
	 * @returns When the function succeeds, it will return 0.
	 */
	int link(const char* prefix);

	/**
	 * @brief Uninks the library.
	 *
	 * This function is thread-safe and can be called multiple times.
	 */
	void unlink();

	int callRunEmber(const char* windowHandle, const char* prefix);
	void callQuitEmber();

private:

	/**
	 * @brief Name of the Ember library
	 */
	static const char* sLibName; // = "WebEmber.dll";

	//interface for the functions to call.
	//there is no parameter check, you need to ensure that the call is correct or not.

	//here is the platform specific difference
#ifdef _WIN32
	HMODULE mModuleHandle;
	#define CALLTYPE __stdcall
	#define LoadLib LoadLibraryA
	#define GetFunction GetProcAddress
	#define UnloadLib FreeLibrary
#else
	void* mModuleHandle;
	#define CALLTYPE
	#define LoadLib(dllname) dlopen(dllname, RTLD_LAZY)
	#define GetFunction dlsym
	#define UnloadLib dlclose
#endif

	//int __stdcall StartWebEmber(const char* windowHandle, const char* prefix);
	static const char* sFuncNameStart; // = "StartWebEmber";
	typedef int ( CALLTYPE*funcTypeStart)(const char* windowHandle, const char* prefix);
	funcTypeStart pStartWebEmber;

	//void __stdcall QuitWebEmber();
	static const char* sFuncNameQuit; // = "QuitWebEmber";
	typedef void ( CALLTYPE *funcTypeQuit)();
	funcTypeQuit pQuitWebEmber;

	/**
	 * @brief Mutex to protect library memory freeing.
	 */
	boost::mutex mUnloadLibraryMutex;

};

#endif // WEBEMBERLINKER_H
