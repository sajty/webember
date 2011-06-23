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

#include "WebEmberAPI.h"

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"

WebEmberAPI::WebEmberAPI(const WebEmberPtr& plugin, const FB::BrowserHostPtr& host) : m_plugin(plugin), m_host(host)
{

}

WebEmberAPI::~WebEmberAPI()
{
}

WebEmberPtr WebEmberAPI::getPlugin()
{
    WebEmberPtr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
}