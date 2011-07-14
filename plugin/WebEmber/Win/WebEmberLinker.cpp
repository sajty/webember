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
#ifndef _WIN32
#error This module can only be compiled on windows.
#endif

#include "WebEmberLinker.h"
#include "WebEmberMessageQueue.h"
#include "WebEmber.h"

#include <windows.h>
#include <imagehlp.h>
#include <iostream>
#include <string>
#include <vector>
#include <strsafe.h>

#pragma comment(lib, "imagehlp.lib")

const char* WebEmberLinker::sDllName = "WebEmber.dll";
const char* WebEmberLinker::sFuncNameStart = "StartWebEmber";
const char* WebEmberLinker::sFuncNameQuit = "QuitWebEmber";

void WebEmberLinker::listDLLFunctions(std::string sADllName, std::vector<std::string>& slListOfDllFunctions)
{
	DWORD *dNameRVAs(0);
	_IMAGE_EXPORT_DIRECTORY *ImageExportDirectory;
	unsigned long cDirSize;
	_LOADED_IMAGE LoadedImage;
	std::string sName;
	slListOfDllFunctions.clear();
	if (MapAndLoad(sADllName.c_str(), NULL, &LoadedImage, TRUE, TRUE))
	{
		ImageExportDirectory = (_IMAGE_EXPORT_DIRECTORY*)
			ImageDirectoryEntryToData(LoadedImage.MappedAddress,
			false, IMAGE_DIRECTORY_ENTRY_EXPORT, &cDirSize);
		if (ImageExportDirectory != NULL)
		{
			dNameRVAs = (DWORD *)ImageRvaToVa(LoadedImage.FileHeader,
				LoadedImage.MappedAddress,
			ImageExportDirectory->AddressOfNames, NULL);
			for(size_t i = 0; i < ImageExportDirectory->NumberOfNames; i++)
			{
				sName = (char *)ImageRvaToVa(LoadedImage.FileHeader,
						LoadedImage.MappedAddress,
					   dNameRVAs[i], NULL);
			 slListOfDllFunctions.push_back(sName);
			}
		}
		UnMapAndLoad(&LoadedImage);
	}
}
 

std::string WebEmberLinker::getMangledName(std::string fncName, std::string dllName)
{
	int found=0;
	std::string name;
	std::vector<std::string> fncs;
	listDLLFunctions(dllName, fncs);
	for(int i=0;i<(int)fncs.size();i++){
		if(fncs[i].find(fncName) != std::string::npos){
			name = fncs[i];
			found++;
		}
	}
	if(found==1){
		return name;
	}else{
		return std::string("");
	}
}

void WebEmberLinker::trimFilename(char* path,int buffsize)
{
	assert(path);//nullpointer not allowed!
	int pos = 0;
	for(int i = 0; path[i] && i != buffsize; i++){
		if(path[i] == '\\' || path[i] == '/')
			pos = i;
	}
	path[pos] = 0;
}

WebEmberLinker::WebEmberLinker() :
	mModuleHandle(0),
	pStartWebEmber(0),
	pQuitWebEmber(0)
{
	

}
int WebEmberLinker::link(){
	assert(!mModuleHandle);
	const int buffsize = MAX_PATH;
	char buffer[buffsize];

	//check the location of the current DLL, we need to know the output filename.
	//WebEmber.dll should be next to npWebEmber.dll
	GetModuleFileNameA(GetModuleHandleA("npWebEmber.dll" ), buffer, buffsize);
	trimFilename(buffer, buffsize);
	
	SetDllDirectoryA(buffer);
	SetCurrentDirectoryA(buffer);

	//normally the browsers are disabling verbose popup messages from the OS on errors,
	//but to get more info on what is the missing DLL when linking recursively, we need to enable this.
	UINT browserErrorMode = GetErrorMode();
	SetErrorMode(0);

	std::string mangledNameStart = getMangledName(sFuncNameStart);
	if(mangledNameStart == ""){
		FBLOG_ERROR("WebEmberLinker::runEmber", "Unable to find function " << sFuncNameStart << " in " << sDllName);
		return 1;
	}

	std::string mangledNameQuit = getMangledName(sFuncNameQuit);
	if(mangledNameQuit == ""){
		FBLOG_ERROR("WebEmberLinker::runEmber", "Unable to find function " << sFuncNameQuit << " in " << sDllName);
		return 2;
	}
	
	//Loads the specified module into the address space of the calling process.
	mModuleHandle = LoadLibraryA(sDllName);
	if(mModuleHandle == NULL){
		FBLOG_ERROR("WebEmberLinker::runEmber", "Unable to load " << sDllName);
		return 3;
	}

	//Retrieves the address of an exported function or variable from the specified DLL.
	pStartWebEmber = (funcTypeStart) GetProcAddress(mModuleHandle, mangledNameStart.c_str());
	if(pStartWebEmber == NULL){
		FBLOG_ERROR("WebEmberLinker::runEmber", "Unable to load function " << mangledNameStart << " in " << sDllName);
		FreeLibrary(mModuleHandle);
		return 4;
	}
	
	//same as above
	pQuitWebEmber = (funcTypeQuit) GetProcAddress(mModuleHandle, mangledNameQuit.c_str());
	if(pQuitWebEmber == NULL){
		FBLOG_ERROR("WebEmberLinker::runEmber", "Unable to load function " << mangledNameQuit << " in " << sDllName);
		FreeLibrary(mModuleHandle);
		return 5;
	}

	//SetErrorMode of the process back to the original.
	SetErrorMode(browserErrorMode);
	return 0;
}
void WebEmberLinker::unlink(){
	boost::mutex::scoped_lock lck(mFreeLibraryMutex);
	if(mModuleHandle){
		FreeLibrary(mModuleHandle);
		mModuleHandle=0;
	}
}
WebEmberLinker::~WebEmberLinker()
{
	//Assert fails, when you have forgotten to call unlink()!
	assert(!mModuleHandle);
}
int WebEmberLinker::callRunEmber(HWND hwnd)
{
	//Assert fails, when you have forgotten to call link()!
	assert(mModuleHandle && pStartWebEmber);
	return pStartWebEmber(hwnd);
}
void WebEmberLinker::callQuitEmber()
{
	//There is a small chance for a race condition, when the user quits ember manually in ember.
	//when the user changes webpage when the FreeLibrary is called it would make segfault.
	boost::try_mutex::scoped_try_lock lck(mFreeLibraryMutex);
	if(lck.owns_lock() && mModuleHandle){
		assert(pQuitWebEmber);
		pQuitWebEmber();
	}
}