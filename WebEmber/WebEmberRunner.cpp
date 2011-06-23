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

#include "WebEmberRunner.h"

#include "WebEmberMessageQueue.h"
#include "WebEmber.h"


#include <iostream>
#include <string>
#include <vector>

bool WebEmberRunner::sRunning=false;


WebEmberRunner::WebEmberRunner(WebEmberMessageQueueWeakPtr queue) :
	mQueue(queue)
{
	
}
WebEmberRunner::~WebEmberRunner()
{
	mLinker.callQuitEmber();
}
int WebEmberRunner::runEmber(HWND hwnd)
{
	FBLOG_INFO("WebEmberLinker::runEmber", "Starting Ember.");
	if(mLinker.link()==0){

		int ret = mLinker.callRunEmber(hwnd);
		if(ret){
			FBLOG_INFO("WebEmberLinker::runEmber", "Shut down WebEmber with error code: " << ret);
		}else{
			FBLOG_INFO("WebEmberLinker::runEmber", "Shut down WebEmber normally.");
		}

		//send quit to message queue
		{
			WebEmberMessageQueuePtr queue(mQueue.lock());
			if (queue) {
				queue->stop();
			}
		}
	}
	mLinker.unlink();
	sRunning=false;
	return 0;
}
void WebEmberRunner::quitEmber()
{
	mLinker.callQuitEmber();
}

int WebEmberRunner::emberThread(boost::shared_ptr<WebEmberRunner> runner, HWND hwnd)
{
	return runner->runEmber(hwnd);
}