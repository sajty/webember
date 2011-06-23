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

#ifndef WEBEMBERAPI_H
#define WEBEMBERAPI_H

#include <string>
#include <sstream>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "WebEmber.h"

class WebEmberAPI : public FB::JSAPIAuto
{
public:

/**
 * @brief Constructor for your JSAPI object.  You should register your methods, properties, and events
 *        that should be accessible to Javascript from here.
 *
 * @see FB::JSAPIAuto::registerMethod
 * @see FB::JSAPIAuto::registerProperty
 * @see FB::JSAPIAuto::registerEvent
 */
    WebEmberAPI(const WebEmberPtr& plugin, const FB::BrowserHostPtr& host);

/**
 * @brief Destructor.  Remember that this object will not be released until
 *          the browser is done with it; this will almost definitely be after
 *          the plugin is released.
 */
    virtual ~WebEmberAPI();
/**
 * @brief Gets a reference to the plugin that was passed in when the object
 *        was created.  If the plugin has already been released then this
 *        will throw a FB::script_error that will be translated into a
 *        javascript exception in the page.
 */
    WebEmberPtr getPlugin();
private:
    WebEmberWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;
};

#endif // WEBEMBERAPI_H

