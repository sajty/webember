#ifndef WEBEMBERX11_H
#define WEBEMBERX11_H
#include "PluginWindowX11.h"
#include "PluginEvents/X11Event.h"

#include <gdk/gdk.h>
#include <X11/Xlib.h>
class WebEmber;

/**
 * @brief This class contains X11 specific code, to inject SDL.
 */
class WebEmberX11
{
public:
	WebEmberX11(WebEmber* plugin);
	bool initSDL(FB::PluginWindow *pluginwindow);
	bool onX11Event(FB::X11Event *evt, FB::PluginWindow * pwin);

private:
	bool mFocus;
	bool mVisible;
	Window mWindow;
	Display* mDisplaySDL;
	FB::PluginWindowX11* mPluginWindow;
	WebEmber* mPlugin;

	//This mask will be used to send messages
	const int mFakeMask;

	//Injects Mouse button event to SDL
	void injectButtonToSDL(GdkEventButton* event);

	//Injects Mouse cursor move event to SDL
	void injectMotionToSDL(GdkEventMotion* event);

	//Injects visibility change to SDL.
	void injectVisibilityToSDL();

	void injectKeyToSDL(GdkEventKey* event);
};
#endif
