#/**********************************************************\
# Auto-Generated Plugin Configuration file
# for WebEmber
#\**********************************************************/

set(PLUGIN_NAME "WebEmber")
set(PLUGIN_PREFIX "EMBER")
set(COMPANY_NAME "WorldForge")

# ActiveX constants:
set(FBTYPELIB_NAME WebEmberLib)
set(FBTYPELIB_DESC "WebEmber 1.0 Type Library")
set(IFBControl_DESC "WebEmber Control Interface")
set(FBControl_DESC "WebEmber Control Class")
set(IFBComJavascriptObject_DESC "WebEmber IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "WebEmber ComJavascriptObject Class")
set(IFBComEventSource_DESC "WebEmber IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID 535D7713-D931-BD44-88C9-35C088E6E08E)
set(IFBControl_GUID 119DB476-513C-7648-96CB-76F4E4981D33)
set(FBControl_GUID 5C67B460-C47F-9A40-A617-C06F64A0113E)
set(IFBComJavascriptObject_GUID A195635E-6D13-6649-8160-D97458E052C2)
set(FBComJavascriptObject_GUID 0DC250DE-AD54-8D44-8879-8A723707E61C)
set(IFBComEventSource_GUID E606D383-951B-8A4B-B1CD-0B2A43C4B2A3)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "WorldForge.WebEmber")
set(MOZILLA_PLUGINID "worldforge.org/WebEmber")

# strings
set(FBSTRING_CompanyName "WorldForge")
set(FBSTRING_FileDescription "This plugin allows to run Ember in your web browser.")
set(FBSTRING_PLUGIN_VERSION "1.0.0.0")
set(FBSTRING_LegalCopyright "Copyright 2011 WorldForge")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "WebEmber")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "WebEmber")
set(FBSTRING_MIMEType "application/x-WebEmber")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

#set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 0)
set(FBMAC_USE_COCOA 1)
set(FBMAC_USE_COREGRAPHICS 1)
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

# If you want to register per-machine on Windows, uncomment this line
set (FB_ATLREG_MACHINEWIDE 1)


add_firebreath_library(log4cplus)