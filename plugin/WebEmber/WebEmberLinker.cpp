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

#include "WebEmberLinker.h"
#include "WebEmberMessageQueue.h"
#include "WebEmber.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <iostream>
#include <string>

#ifdef _WIN32
const char* WebEmberLinker::sLibName = "libWebEmber-0.1.dll";
#elif defined(__APPLE__)
const char* WebEmberLinker::sLibName = "libWebEmber-0.1.dylib";
#else
const char* WebEmberLinker::sLibName = "libWebEmber-0.1.so";
#endif
const char* WebEmberLinker::sFuncNameStart = "StartWebEmber";
const char* WebEmberLinker::sFuncNameQuit = "QuitWebEmber";

WebEmberLinker::WebEmberLinker() :
		mModuleHandle(0), pStartWebEmber(0), pQuitWebEmber(0)
{

}
int WebEmberLinker::link(const char* prefix)
{
	assert(!mModuleHandle);
	std::string libdir(prefix);

#ifdef _WIN32
	if(!libdir.empty()) {
		libdir += "\\bin";
		SetDllDirectoryA(libdir.c_str());
		SetCurrentDirectoryA(libdir.c_str());
		libdir += "\\";
	}

	//normally the browsers are disabling verbose popup messages from the OS on errors,
	//but to get more info on what is the missing DLL when linking recursively, we need to enable this.
	UINT browserErrorMode = GetErrorMode();
	SetErrorMode(0);
#else
	if (!libdir.empty()) {
		libdir += "/lib/";
	}
#endif

	std::string libfile(libdir + sLibName);
	FBLOG_INFO("WebEmberLinker::runEmber", "Loading library: " << libfile);
	//Loads the specified module into the address space of the calling process.
	mModuleHandle = LoadLib(libfile.c_str());
	if (mModuleHandle == NULL) {
#ifndef _WIN32
		FBLOG_ERROR("WebEmberLinker::runEmber", "dlerror: " << dlerror());
#endif
		FBLOG_ERROR("WebEmberLinker::runEmber", "Unable to load " << libfile);
		return 1;
	}
	
	//Retrieves the address of an exported function or variable from the specified DLL.
	pStartWebEmber = (funcTypeStart)GetFunction(mModuleHandle, sFuncNameStart);
	if (pStartWebEmber == NULL) {
#ifndef _WIN32
		FBLOG_ERROR("WebEmberLinker::runEmber", "dlerror: " << dlerror());
#endif
		FBLOG_ERROR("WebEmberLinker::runEmber", "Unable to load function " << sFuncNameStart << " in " << libfile);
		UnloadLib(mModuleHandle);
		return 2;
	}

	//same as above
	pQuitWebEmber = (funcTypeQuit)GetFunction(mModuleHandle, sFuncNameQuit);
	if (pQuitWebEmber == NULL) {
#ifndef _WIN32
		FBLOG_ERROR("WebEmberLinker::runEmber", "dlerror: " << dlerror());
#endif
		FBLOG_ERROR("WebEmberLinker::runEmber", "Unable to load function " << sFuncNameQuit << " in " << libfile);
		UnloadLib(mModuleHandle);
		return 3;
	}

#ifdef _WIN32
	//SetErrorMode of the process back to the original.
	SetErrorMode(browserErrorMode);
#endif
	return 0;
}
void WebEmberLinker::unlink()
{
	boost::mutex::scoped_lock lck(mUnloadLibraryMutex);
	if (mModuleHandle) {
		UnloadLib(mModuleHandle);
		mModuleHandle = 0;
	}
}
WebEmberLinker::~WebEmberLinker()
{
	//Assert fails, when you have forgotten to call unlink()!
	assert(!mModuleHandle);
}
int WebEmberLinker::callRunEmber(const char* windowHandle, const char* prefix)
{
	//Assert fails, when you have forgotten to call link()!
	assert(mModuleHandle && pStartWebEmber);
	return pStartWebEmber(windowHandle, prefix);
}
void WebEmberLinker::callQuitEmber()
{
	//There is a small chance for a race condition, when the user quits ember manually in ember.
	//when the user changes webpage when the FreeLibrary is called it would make segfault.
	boost::try_mutex::scoped_try_lock lck(mUnloadLibraryMutex);
	if (lck.owns_lock() && mModuleHandle) {
		assert(pQuitWebEmber);
		pQuitWebEmber();
	}
}
