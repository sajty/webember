#ifndef WEBEMBERX11_H
#define WEBEMBERX11_H
#include "PluginWindowX11.h"
#include "PluginEvents/X11Event.h"

#include <gdk/gdk.h>
#include <X11/Xlib.h>
class WebEmber;

/**
 * @brief This class contains X11 specific code to inject SDL into the browser.
 *
 * This will handle every event and send it to SDL. Fixes lot of issues, what SDL is not handling correctly.
 */
class WebEmberX11
{
public:
	/**
	 * @brief Ctor
	 *
	 * @param plugin Owner of this object. Normally its WebEmber, and the owner should not be destructed before this.
	 */
	WebEmberX11(WebEmber* plugin);

	/**
	 * @brief Initialize SDL in the browser window.
	 *
	 * This needs to be called from the main thread or some versions of firefox will assert!
	 */
	bool initSDL(FB::PluginWindow *pluginwindow);

	/**
	 * @brief Event handling.
	 */
	bool onX11Event(FB::X11Event *evt, FB::PluginWindow * pwin);

private:
	/**
	 * @brief Keyboard focus state.
	 */
	bool mFocus;

	/**
	 * @brief SDL visibility state.
	 */
	bool mVisible;

	/**
	 * @brief Plugin window.
	 */
	Window mWindow;

	/**
	 * @brief Display of the SDL client.
	 */
	Display* mDisplaySDL;

	/**
	 * @brief Current active plugin window object.
	 */
	FB::PluginWindowX11* mPluginWindow;

	/**
	 * @brief The main plugin object and the owner of this object.
	 */
	WebEmber* mPlugin;

	/**
	 * @brief Fake mask to inject messages to SDL.
	 */
	const int mFakeMask;

	/**
	 * @brief Injects Mouse button event to SDL.
	 *
	 * SDL is not grabbing the focus by itself and we need to grab the focus,
	 * but only 1 client can get button events or we get BadAccess error.
	 * Solution: We get the button, handle focus, then inject the button with fake mask.
	 * Fake mask is needed for the reason mentioned above, if 2 clients on the same window has ButtonPressMask, we get BadAccess error.
	 */
	void injectButtonToSDL(GdkEventButton* event);

	/**
	 * @brief Injects Mouse motion event to SDL.
	 *
	 * Mouse motion is working in SDL when enabled mask for it, until the mouse button is pressed.
	 * Only the client who gets mouse button press event will get mouse motion events while button is not released.
	 * Since we inject buttons to SDL, we need to inject this too.
	 */
	void injectMotionToSDL(GdkEventMotion* event);

	/**
	 * @brief Injects Visibility event to SDL.
	 *
	 * As in the SDL source-code SDL needs unmap event to hide itself and disable the buffer.
	 * We will do that for tab change support.
	 * Well, it has some timing issues at the moment.
	 * SDL is doing lot of SDL_Lock_EventThread and XSync which makes trouble
	 * and we would need to do XSync here to execute this before Firefox draws another tab,
	 * but that can make a dependency loop, because we call this from the Window Manager thread.
	 */
	void injectVisibilityToSDL(bool visible);

	/**
	 * @brief Injects Key event to SDL.
	 *
	 * Key events are for some reason not sent to SDL, if firebreath client gets the keys.
	 * In Xorg docs it says that all clients should receive it, so its a bit weird.
	 */
	void injectKeyToSDL(GdkEventKey* event);
};
#endif
