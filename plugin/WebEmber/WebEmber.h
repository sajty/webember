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

#ifndef WEBEMBER_H
#define WEBEMBER_H


#include "PluginWindow.h"
#include "PluginEvents/KeyboardEvents.h"
#include "PluginEvents/MouseEvents.h"

#include "PluginEvents/AttachedEvent.h"

#include "PluginCore.h"

#include <boost/thread/thread.hpp>
#ifndef _WIN32
#include "X11/WebEmberX11.h"
#include "PluginEvents/X11Event.h"
#include <X11/Xlib.h>
#include <gtk/gtk.h>
#endif
FB_FORWARD_PTR(WebEmber)
FB_FORWARD_PTR(WebEmberRunner)
FB_FORWARD_PTR(WebEmberMessageQueue)
/**
 * @brief WebEmber
 */
class WebEmber: public FB::PluginCore
{
public:

	/**
	 * @brief Called from PluginFactory::globalPluginInitialize()
	 *
	 * @see FB::FactoryBase::globalPluginInitialize
	 */
	static void StaticInitialize();

	/**
	 * @brief Called from PluginFactory::globalPluginDeinitialize()
	 *
	 * @see FB::FactoryBase::globalPluginDeinitialize
	 */
	static void StaticDeinitialize();

	/**
	 * Note that your API is not available
	 * at this point, nor the window.  For best results wait to use
	 * the JSAPI object until the onPluginReady method is called
	 */
	WebEmber();

	virtual ~WebEmber();

	/**
	 * When this is called, the BrowserHost is attached, the JSAPI object is
	 * created, and we are ready to interact with the page and such. The
	 * PluginWindow may or may not have already fire the AttachedEvent at
	 * this point.
	 */
	void onPluginReady();

	/**
	 * This will be called when it is time for the plugin to shut down;
	 * any threads or anything else that may hold a shared_ptr to this
	 * object should be released here so that this object can be safely
	 * destroyed. This is the last point that shared_from_this and weak_ptr
	 * references to this object will be valid.
	 */
	void shutdown();

	WebEmberRunnerPtr getRunner()
	{
		return mRunner;
	}

	WebEmberMessageQueuePtr getMessageQueue()
	{
		return mMessageQueue;
	}

	FB::BrowserHostPtr getBrowserHost()
	{
		return m_host;
	}

	/**
	 * @brief Creates an instance of the JSAPI object that provides your main
	 *        Javascript interface.
	 *
	 * Note that m_host is your BrowserHost and shared_ptr returns a
	 * FB::PluginCorePtr, which can be used to provide a
	 * boost::weak_ptr<WebEmber> for your JSAPI class.
	 *
	 * Be very careful where you hold a shared_ptr to your plugin class from,
	 * as it could prevent your plugin class from getting destroyed properly.
	 */
	virtual FB::JSAPIPtr createJSAPI();

	/**
	 * If you want your plugin to always be windowless, set this to true
	 * If you want your plugin to be optionally windowless based on the
	 * value of the "windowless" param tag, remove this method or return
	 * FB::PluginCore::isWindowless()
	 */
	virtual bool isWindowless()
	{
		return false;
	}

	BEGIN_PLUGIN_EVENT_MAP()
		EVENTTYPE_CASE(FB::AttachedEvent, onWindowAttached, FB::PluginWindow)
		EVENTTYPE_CASE(FB::DetachedEvent, onWindowDetached, FB::PluginWindow)
#ifndef _WIN32
		EVENTTYPE_CASE(FB::X11Event, onX11Event, FB::PluginWindow)
#endif
	END_PLUGIN_EVENT_MAP()

	/** BEGIN EVENTDEF -- DON'T CHANGE THIS LINE **/
	virtual bool onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *);
	virtual bool onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *);
#ifndef _WIN32
	virtual bool onX11Event(FB::X11Event *evt, FB::PluginWindow *);
#endif
	/** END EVENTDEF -- DON'T CHANGE THIS LINE **/

private:
#ifndef _WIN32
	WebEmberX11 mX11;
#endif
	WebEmberRunnerPtr mRunner;
	WebEmberMessageQueuePtr mMessageQueue;
	FB::PluginWindow* mActivePluginWindow;
	boost::mutex mWindowAttachMutex;
	static boost::thread* mThreadEmber;
	static boost::thread* mThreadMessageQueue;
	void startEmber();
	std::string getWindowHandle(FB::PluginWindow * pwnd);
};

#endif

