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

#include "PluginWindowWin.h"

#include "WebEmberAPI.h"

#include "WebEmberMessageQueue.h"
#include "WebEmberRunner.h"

#include "WebEmber.h"

boost::thread* WebEmber::mThreadEmber=0;
boost::thread* WebEmber::mThreadMessageQueue=0;

void WebEmber::StaticInitialize()
{
    // Place one-time initialization stuff here; As of FireBreath 1.4 this should only
    // be called once per process
}

void WebEmber::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here. As of FireBreath 1.4 this should
    // always be called just before the plugin library is unloaded

	mThreadEmber->join();
	mThreadMessageQueue->join();
}

WebEmber::WebEmber() :
	mActivePluginWindow(0)
{
}

WebEmber::~WebEmber()
{
    // This is optional, but if you reset m_api (the shared_ptr to your JSAPI
    // root object) and tell the host to free the retained JSAPI objects then
    // unless you are holding another shared_ptr reference to your JSAPI object
    // they will be released here.
    releaseRootJSAPI();
    m_host->freeRetainedObjects();
}

void WebEmber::onPluginReady()
{
    // When this is called, the BrowserHost is attached, the JSAPI object is
    // created, and we are ready to interact with the page and such.  The
    // PluginWindow may or may not have already fire the AttachedEvent at
    // this point.
}

void WebEmber::shutdown()
{
    // This will be called when it is time for the plugin to shut down;
    // any threads or anything else that may hold a shared_ptr to this
    // object should be released here so that this object can be safely
    // destroyed. This is the last point that shared_from_this and weak_ptr
    // references to this object will be valid

}


FB::JSAPIPtr WebEmber::createJSAPI()
{
    // m_host is the BrowserHost
    return boost::make_shared<WebEmberAPI>(FB::ptr_cast<WebEmber>(shared_from_this()), m_host);
}

bool WebEmber::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow * pwin)
{
	FBLOG_INFO("WebEmber::onWindowAttached", "Trying to attach window: " << reinterpret_cast<long>(pwin));
	boost::mutex::scoped_lock lck(mWindowAttachMutex);
	//this could be windowless, to protect from problems, we need dynamic cast.
	FB::PluginWindowWin* wnd = static_cast<FB::PluginWindowWin*>(pwin);
	assert(wnd);
	if(wnd && !WebEmberRunner::isRunning()){
		WebEmberRunner::setRunning(true);
		mActivePluginWindow=pwin;
		mMessageQueue = boost::shared_ptr<WebEmberMessageQueue>(new WebEmberMessageQueue(FB::ptr_cast<WebEmber>(shared_from_this())));
		mRunner = boost::shared_ptr<WebEmberRunner>(new WebEmberRunner(mMessageQueue));
		mThreadMessageQueue = new boost::thread( &WebEmberMessageQueue::messageQueueThread, mMessageQueue );
		mThreadEmber = new boost::thread( &WebEmberRunner::emberThread, mRunner, wnd->getHWND() );
		FBLOG_INFO("WebEmber::onWindowAttached", "Attached window: " << reinterpret_cast<long>(pwin));
	}

    // The window is attached; act appropriately
    return false;
}

bool WebEmber::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow * pwin)
{
	FBLOG_INFO("WebEmber::onWindowDetached", "Trying to detach window: " << reinterpret_cast<long>(pwin));
	boost::mutex::scoped_lock lck(mWindowAttachMutex);
	assert(pwin);
	if(mActivePluginWindow == pwin && WebEmberRunner::isRunning()){
		mRunner->quitEmber();
		pwin=0;
		FBLOG_INFO("WebEmber::onWindowDetached", "Ember window detached.");
	}

    // The window is about to be detached; act appropriately
    return false;
}

	