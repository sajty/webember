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

#ifndef WEBEMBERRUNNER_H
#define WEBEMBERRUNNER_H

#ifdef _WIN32
#include "Win/WebEmberLinker.h"
#endif

#include "WebEmber.h"
#include <windows.h>
#include <string>
#include <vector>

class WebEmber;
class WebEmberLinker;
FB_FORWARD_PTR(WebEmberRunner)
/**
 * @brief This class will run ember and will make a platform independent interface for it. 
 */
class WebEmberRunner{
public:
	/**
	 * @brief Ctor
	 * @param plugin This should be the owner of the linker.
	 */
	WebEmberRunner(WebEmberMessageQueueWeakPtr queue);

	virtual ~WebEmberRunner();

	/**
	 * @brief Links the DLL and runs ember.
	 * @param hwnd Window handle of the owner window.
	 * @returns return code of the ember's main().
	 * This function should be called from a separate thread.
	 * The thread should start form WebEmberLinker::EmberThread.
	 * It will only return, when ember is shut down.
	 */
	int runEmber(HWND hwnd);
	
	/**
	 * @brief Signals quit to the Ember thread.
	 */
	void quitEmber();

	/**
	 * @brief Starting point for the ember thread.
	 */
	static int emberThread(boost::shared_ptr<WebEmberRunner> linker, HWND hwnd);

	static void setRunning(bool state)
	{
		sRunning=state;
	}
	static bool isRunning()
	{
		return sRunning;
	}

private:
	/**
	 * @brief Saves the state of the ember thread.
	 */
	static bool sRunning;

	WebEmberLinker mLinker;

	/**
	 * @brief The MessageQueue associated with ember.
	 * This is needed, because Ember thread will start the queue shutdown.
	 */
	WebEmberMessageQueueWeakPtr mQueue;
};

#endif // WEBEMBERRUNNER_H