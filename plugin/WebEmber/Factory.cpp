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

#include "FactoryBase.h"
#include "WebEmber.h"
#include <boost/make_shared.hpp>
#ifdef _WIN32
#include <windows.h>
#endif
class PluginFactory: public FB::FactoryBase
{
public:
	/**
	 * @brief Creates a plugin object matching the provided mimetype
	 *        If mimetype is empty, returns the default plugin
	 */
	FB::PluginCorePtr createPlugin(const std::string& mimetype)
	{
		return boost::make_shared<WebEmber>();
	}

	/**
	 * @see FB::FactoryBase::globalPluginInitialize
	 */
	void globalPluginInitialize()
	{
		WebEmber::StaticInitialize();
	}

	/**
	 * @see FB::FactoryBase::globalPluginDeinitialize
	 */
	void globalPluginDeinitialize()
	{
		WebEmber::StaticDeinitialize();
	}

	/**
	 * @brief Sets the logging level.
	 */
	FB::Log::LogLevel getLogLevel()
	{
#ifdef _DEBUG
		return FB::Log::LogLevel_Debug;
#else
		return FB::Log::LogLevel_Info;
#endif
	}
	/**
	 * @brief Sets the logging method.
	 */
	void getLoggingMethods(FB::Log::LogMethodList& outMethods)
	{

#ifdef _WIN32
		// The next line will enable logging to the console (think: printf).
		outMethods.push_back(std::make_pair(FB::Log::LogMethod_Console, std::string()));
		
		const int buffsize = MAX_PATH;
		char buffer[buffsize];
		GetModuleFileNameA( GetModuleHandleA("npWebEmber.dll" ), buffer, buffsize );
		int pos = 0;
		for(int i = 0; buffer[i] && i != buffsize; i++) {
			if(buffer[i] == '\\' || buffer[i] == '/') {
				pos = i;
			}
		}
		strcpy(&buffer[pos],"/WebEmber.log");
		
		// The next line will enable logging to a logfile.
		outMethods.push_back(std::make_pair(FB::Log::LogMethod_File, buffer));
#elif defined(__APPLE__)
		outMethods.push_back(std::make_pair(FB::Log::LogMethod_Console, std::string()));
#else
		//NOTE: for firefox ~ returns a different directory.
		//to get the current logged in users home, we need to use HOME env var.
		const char* home = getenv("HOME");
		if(home){
			std::string logfile(home);
			logfile += "/.ember";
			mkdir(logfile.c_str());
			logfile += "/WebEmber.log";
			outMethods.push_back(std::make_pair(FB::Log::LogMethod_File, logfile.c_str()));
		}
		outMethods.push_back(std::make_pair(FB::Log::LogMethod_Console, std::string()));
#endif
	}
};

/**
 * @brief Returns the factory instance for this plugin module
 */
FB::FactoryBasePtr getFactoryInstance()
{
	static boost::shared_ptr<PluginFactory> factory = boost::make_shared<PluginFactory>();
	return factory;
}
