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
#include "WebEmber.h"

#include "WebEmberAPI.h"

#include "WebEmberMessageQueue.h"
#include "WebEmberRunner.h"
#ifdef _WIN32
#include "PluginWindowWin.h"
#elif !defined(__APPLE__)
#include "PluginWindowX11.h"
#endif

#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>

boost::thread* WebEmber::mThreadEmber = 0;
boost::thread* WebEmber::mThreadMessageQueue = 0;
void WebEmber::StaticInitialize()
{
	// Place one-time initialization stuff here; As of FireBreath 1.4 this should only
	// be called once per process

}

void WebEmber::StaticDeinitialize()
{
	// Place one-time deinitialization stuff here. As of FireBreath 1.4 this should
	// always be called just before the plugin library is unloaded
}

WebEmber::WebEmber() :	
#if defined(USE_X11)
	mX11(this),
#endif
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
	FBLOG_INFO("WebEmber::shutdown", "Waiting for ember to shut down.");
	if(mThreadEmber){
		mThreadEmber->join();
	}
	if(mThreadMessageQueue){
		mThreadMessageQueue->join();
	}
	FBLOG_INFO("WebEmber::shutdown", "Plugin is ready for shutdown");

}

FB::JSAPIPtr WebEmber::createJSAPI()
{
	// m_host is the BrowserHost
	return boost::make_shared<WebEmberAPI>(FB::ptr_cast<WebEmber>(shared_from_this()), m_host);
}

bool WebEmber::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow * pwin)
{
	//It is guaranteed, that each onWindowAttach and onWindowDetached event is called from the same thread on each window.
	FBLOG_INFO("WebEmber::onWindowAttached", "Trying to attach window: " << reinterpret_cast<long>(pwin));
	if (!WebEmberRunner::isRunning()) {
		WebEmberRunner::setRunning(true);
		mActivePluginWindow = pwin;
		startEmber();
	}

	// The window is attached; act appropriately
	return false;
}

bool WebEmber::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow * pwin)
{
	FBLOG_INFO("WebEmber::onWindowDetached", "Trying to detach window: " << reinterpret_cast<long>(pwin));
	assert(pwin);
	if (mActivePluginWindow == pwin && WebEmberRunner::isRunning()) {
		mRunner->quitEmber();
		pwin = 0;
		FBLOG_INFO("WebEmber::onWindowDetached", "Ember window detached.");
	}

	// The window is about to be detached; act appropriately
	return false;
}

std::string WebEmber::getWindowHandle(FB::PluginWindow * pwin)
{
	std::stringstream sstr;
#ifdef _WIN32
	FB::PluginWindowWin* wnd = static_cast<FB::PluginWindowWin*>(pwin);
	sstr << reinterpret_cast<unsigned long>(wnd->getHWND());
#else
	//On Linux, we will init SDL in the plugin, because windowing needs to be done on the main thread.
	//SDL provides the handle globally with SDL_GetWMInfo function.
#endif
	return sstr.str();
}

void WebEmber::startEmber()
{
	WebEmberPtr thisPtr(FB::ptr_cast<WebEmber>(shared_from_this()));
#ifdef USE_X11
	mX11.initSDL(mActivePluginWindow);
#endif
	std::string handle = getWindowHandle(mActivePluginWindow);
	
	mMessageQueue = boost::shared_ptr<WebEmberMessageQueue>(new WebEmberMessageQueue(thisPtr));
	mThreadMessageQueue = new boost::thread(&WebEmberMessageQueue::messageQueueThread, mMessageQueue);
	mRunner = boost::shared_ptr<WebEmberRunner>(new WebEmberRunner(mMessageQueue));
	mThreadEmber = new boost::thread(&WebEmberRunner::emberThread, mRunner, handle);
	FBLOG_INFO("WebEmber::onWindowAttached", "Attached window: " << reinterpret_cast<long>(mActivePluginWindow));
}
#ifdef USE_X11
bool WebEmber::onX11Event(FB::X11Event* event, FB::PluginWindow* wnd)
{
	mX11.onX11Event(event, wnd);
}
#endif
