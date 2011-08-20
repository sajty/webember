#include "WebEmberX11.h"
#include "../WebEmber.h"

#include "PluginWindowX11.h"
#include "logging.h"

#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>

WebEmberX11::WebEmberX11(WebEmber* plugin) :
	mPlugin(plugin),
	mFocus(false),
	mVisible(false),
	mFakeMask(PropertyChangeMask),
	mWindow(0),
	mDisplaySDL(0),
	mPluginWindow(0)
{
}

bool WebEmberX11::initSDL(FB::PluginWindow *pluginwindow)
{
	assert(!mPluginWindow);
	mPluginWindow = static_cast<FB::PluginWindowX11*>(pluginwindow);

	GtkWidget* widget = mPluginWindow->getWidget();

	// Fixes BadWindow error in SDL_SetVideoMode
	gdk_display_flush(gtk_widget_get_display(widget));

	char tmp[64];
	sprintf(tmp, "SDL_WINDOWID=%lu", (unsigned long)mPluginWindow->getWindow());
	putenv(tmp);
	//putenv("SDL_DEBUG=1");

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		FBLOG_FATAL("WebEmberX11::initSDL", "Couldn't initialize SDL: " << SDL_GetError());
		return true;
	}

	if (!SDL_SetVideoMode(800, 600, 0, 0)) {
		FBLOG_FATAL("WebEmberX11::initSDL", "Couldn't create SDL window: " << SDL_GetError());
		return true;
	}

	SDL_SysWMinfo info;
	XSetWindowAttributes attributes;

	SDL_VERSION(&info.version); // this is important!
	SDL_GetWMInfo(&info);
	mDisplaySDL = info.info.x11.display;
	mWindow = info.info.x11.window;
	// Select the events, what SDL should receive.
	// SDL will not receive any messages when SDL_WINDOWID is set.
	{
		attributes.event_mask =
			//KeyPressMask | KeyReleaseMask | //ButtonPressMask | //ButtonReleaseMask |
			PropertyChangeMask// | StructureNotifyMask | //KeymapStateMask |
			//EnterWindowMask  | LeaveWindowMask | ExposureMask | PointerMotionMask |
			//VisibilityChangeMask// | PointerMotionHintMask |
			//FocusChangeMask
			;

		//select events, what SDL should receive.
		XSelectInput(mDisplaySDL, mWindow, attributes.event_mask);

		// Get the atom.
		//Atom WM_DELETE_WINDOW = XInternAtom(mDisplaySDL, "WM_DELETE_WINDOW", False);

		// Send window destory event to SDL to shut down properly.
		//XSetWMProtocols(mDisplaySDL, mWindow, &WM_DELETE_WINDOW, 1);
	}
	return false;
}
void WebEmberX11::deinitSDL(FB::PluginWindow *pluginwindow)
{
	assert(pluginwindow == (FB::PluginWindow*)mPluginWindow);
	XSync(mDisplaySDL,true);
	injectWindowDestroyToSDL();
	XFlush(mDisplaySDL);

}
void WebEmberX11::injectWindowDestroyToSDL()
{

	// Get the atom.
	Atom WM_DELETE_WINDOW = XInternAtom(mDisplaySDL, "WM_DELETE_WINDOW", False);

	XEvent xevent;
	memset(&xevent, 0, sizeof(XEvent));
	xevent.xclient.type = ClientMessage;
	xevent.xclient.serial = 0; //I'm not sure of this.
	xevent.xclient.send_event = True;
	xevent.xclient.display = mDisplaySDL;
	xevent.xclient.window = mWindow;
	xevent.xclient.message_type = WM_DELETE_WINDOW;
	xevent.xclient.format = 32;
	xevent.xclient.data.l[0] = WM_DELETE_WINDOW;

	XSendEvent(mDisplaySDL, mWindow, False, mFakeMask, &xevent);
}
void WebEmberX11::injectButtonToSDL(GdkEventButton* event)
{

	//We need to convert GdkEvent back to XEvent manually.
	XEvent xevent;
	memset(&xevent, 0, sizeof(XEvent));
	xevent.xbutton.type = (event->type == GDK_BUTTON_RELEASE) ? ButtonRelease : ButtonPress;
	xevent.xbutton.serial = 0; //I'm not sure of this.
	xevent.xbutton.send_event = True;
	xevent.xbutton.display = mDisplaySDL;
	xevent.xbutton.window = mWindow;
	xevent.xbutton.root = mPluginWindow->getBrowserWindow();
	xevent.xbutton.subwindow = None;
	xevent.xbutton.time = event->time;
	xevent.xbutton.x = event->x;
	xevent.xbutton.y = event->y;
	xevent.xbutton.x_root = event->x_root;
	xevent.xbutton.y_root = event->y_root;
	xevent.xbutton.state = event->state;
	xevent.xbutton.button = event->button;
	xevent.xbutton.same_screen = True;

	// The problem is that only 1 client can receive ButtonPressMask.
	// The trick is that we resend it with a fake mask which is received by SDL.
	XSendEvent(mDisplaySDL, mWindow, False, mFakeMask, &xevent);
}


void WebEmberX11::injectKeyToSDL(GdkEventKey* event)
{

	//We need to convert GdkEvent back to XEvent manually.
	XEvent xevent;
	memset(&xevent, 0, sizeof(XEvent));
	if (event->type == GDK_KEY_PRESS) {
		printf("keypress\n");
		xevent.xkey.type = KeyPress;
	} else {
		printf("keyrelease\n");
		xevent.xkey.type = KeyRelease;
	}
	xevent.xkey.serial = 0; //I'm not sure of this.
	xevent.xkey.send_event = True;
	xevent.xkey.display = mDisplaySDL;
	xevent.xkey.window = mWindow;
	xevent.xkey.root = mPluginWindow->getBrowserWindow();
	xevent.xkey.subwindow = None;
	xevent.xkey.time = event->time;
	xevent.xkey.x = 1;
	xevent.xkey.y = 1;
	xevent.xkey.x_root = 1;
	xevent.xkey.y_root = 1;
	xevent.xkey.state = event->state;
	xevent.xkey.keycode = event->hardware_keycode;
	xevent.xkey.same_screen = True;

	XSendEvent(mDisplaySDL, mWindow, False, mFakeMask, &xevent);
}

void WebEmberX11::injectMotionToSDL(GdkEventMotion* event)
{
	//We need to convert GdkEvent back to XEvent manually.
	XEvent xevent;
	memset(&xevent, 0, sizeof(XEvent));
	xevent.xmotion.type = MotionNotify;
	xevent.xmotion.serial = 0; //I'm not sure of this.
	xevent.xmotion.send_event = True;
	xevent.xmotion.display = mDisplaySDL;
	xevent.xmotion.window = mWindow;
	xevent.xmotion.root = mPluginWindow->getBrowserWindow();
	xevent.xmotion.subwindow = None;
	xevent.xmotion.time = event->time;
	xevent.xmotion.x = event->x;
	xevent.xmotion.y = event->y;
	xevent.xmotion.x_root = event->x_root;
	xevent.xmotion.y_root = event->y_root;
	xevent.xmotion.state = event->state;
	xevent.xmotion.is_hint = event->is_hint;
	xevent.xmotion.same_screen = True;

	XSendEvent(mDisplaySDL, mWindow, False, mFakeMask, &xevent);
}

void WebEmberX11::injectVisibilityToSDL(bool visible)
{
	//We need to convert GdkEvent back to XEvent manually.
	XEvent xevent;
	memset(&xevent, 0, sizeof(XEvent));

	if (visible) {
		xevent.xunmap.type = MapNotify;
		xevent.xunmap.serial = 0;
		xevent.xunmap.send_event = True;
		xevent.xunmap.display = mDisplaySDL;
		xevent.xunmap.event = mWindow;
		xevent.xunmap.window = mWindow;
		xevent.xunmap.from_configure = False;
	} else {
		xevent.xmap.type = UnmapNotify;
		xevent.xmap.serial = 0;
		xevent.xmap.send_event = True;
		xevent.xmap.display = mDisplaySDL;
		xevent.xmap.event = mWindow;
		xevent.xmap.window = mWindow;
		xevent.xmap.override_redirect = False;
	}

	XSendEvent(mDisplaySDL, mWindow, False, mFakeMask, &xevent);
}

bool WebEmberX11::onX11Event(FB::X11Event *evt, FB::PluginWindow * pwin)
{
	//Do not handle event if:
	//-It's not the active firebreath window.
	//-When the event is sent by XSendEvent. It could be our injected event and it could loop forever.
	if (pwin != (FB::PluginWindow*)mPluginWindow || evt->m_event->any.send_event == true){
		return true;
	}

	switch (evt->m_event->type) {
	case GDK_MOTION_NOTIFY:
	{
		injectMotionToSDL((GdkEventMotion*)evt->m_event);
	} break;
	case GDK_BUTTON_PRESS:
	{
		GdkEventButton* buttonevent = (GdkEventButton*)evt->m_event;
		injectButtonToSDL(buttonevent);
		if (!mFocus) {
			Display* display = GDK_DISPLAY_XDISPLAY(gtk_widget_get_display(mPluginWindow->getWidget()));
			XSetInputFocus(display, mWindow, RevertToParent, buttonevent->time);
			gtk_widget_grab_focus(mPluginWindow->getWidget());
		}
	} break;
	case GDK_BUTTON_RELEASE:
	{
		injectButtonToSDL((GdkEventButton*)evt->m_event);

	} break;
	case GDK_KEY_RELEASE:
	case GDK_KEY_PRESS:
	{
		injectKeyToSDL((GdkEventKey*)evt->m_event);
	} break;
	case GDK_FOCUS_CHANGE:
	{
		GdkEventFocus* focus = (GdkEventFocus*)evt->m_event;
		if (focus->in) {
			mFocus = true;
		} else {
			mFocus = false;
		}
	} break;
	case GDK_VISIBILITY_NOTIFY:
	{
		GdkEventVisibility* visibility = (GdkEventVisibility*)evt->m_event;
		if (visibility->state != GDK_VISIBILITY_FULLY_OBSCURED) {
			if (!mVisible) {
				mVisible = true;
				injectVisibilityToSDL(mVisible);
				//XFlush(mDisplaySDL);
			}
		} else {
			if (mVisible) {
				mVisible = false;
				injectVisibilityToSDL(mVisible);
				//XFlush(mDisplaySDL);
			}
		}
	} break;
	}

	//Returns true to never use the firebreath's default event handling.
	return true;
}
