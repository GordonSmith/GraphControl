/*******************************************************************************
 * Copyright (C) 2011 HPCC Systems.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/
#include "precompiled_headers.h"

#include "NpapiTypes.h"
#include "HPCCSystemsGraphViewControlAPI.h"

#include "HPCCSystemsGraphViewControl.h"
#ifdef XP_WIN
#include "Win/PluginWindowWin.h"
#endif

#include <SvgParser.h>
#include <GraphvizLayout.h>
#include <agg_basics.h>
#include <iostream>

//  ===========================================================================
///////////////////////////////////////////////////////////////////////////////
/// @fn HPCCSystemsGraphViewControl::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginInitialize()
///
/// @see FB::FactoryBase::globalPluginInitialize
///////////////////////////////////////////////////////////////////////////////
void HPCCSystemsGraphViewControl::StaticInitialize()
{
    // Place one-time initialization stuff here; As of FireBreath 1.4 this should only
    // be called once per process
}

///////////////////////////////////////////////////////////////////////////////
/// @fn HPCCSystemsGraphViewControl::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginDeinitialize()
///
/// @see FB::FactoryBase::globalPluginDeinitialize
///////////////////////////////////////////////////////////////////////////////
void HPCCSystemsGraphViewControl::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here. As of FireBreath 1.4 this should
    // always be called just before the plugin library is unloaded
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  HPCCSystemsGraphViewControl constructor.  Note that your API is not available
///         at this point, nor the window.  For best results wait to use
///         the JSAPI object until the onPluginReady method is called
///////////////////////////////////////////////////////////////////////////////
HPCCSystemsGraphViewControl::HPCCSystemsGraphViewControl()
{
	m_wnd = NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  HPCCSystemsGraphViewControl destructor.
///////////////////////////////////////////////////////////////////////////////
HPCCSystemsGraphViewControl::~HPCCSystemsGraphViewControl()
{
    // This is optional, but if you reset m_api (the shared_ptr to your JSAPI
    // root object) and tell the host to free the retained JSAPI objects then
    // unless you are holding another shared_ptr reference to your JSAPI object
    // they will be released here.
    releaseRootJSAPI();
    m_host->freeRetainedObjects();
}

void HPCCSystemsGraphViewControl::onPluginReady()
{
    // When this is called, the BrowserHost is attached, the JSAPI object is
    // created, and we are ready to interact with the page and such.  The
    // PluginWindow may or may not have already fire the AttachedEvent at
    // this point.
}

void HPCCSystemsGraphViewControl::shutdown()
{
    // This will be called when it is time for the plugin to shut down;
    // any threads or anything else that may hold a shared_ptr to this
    // object should be released here so that this object can be safely
    // destroyed. This is the last point that shared_from_this and weak_ptr
    // references to this object will be valid
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  Creates an instance of the JSAPI object that provides your main
///         Javascript interface.
///
/// Note that m_host is your BrowserHost and shared_ptr returns a
/// FB::PluginCorePtr, which can be used to provide a
/// boost::weak_ptr<HPCCSystemsGraphViewControl> for your JSAPI class.
///
/// Be very careful where you hold a shared_ptr to your plugin class from,
/// as it could prevent your plugin class from getting destroyed properly.
///////////////////////////////////////////////////////////////////////////////
FB::JSAPIPtr HPCCSystemsGraphViewControl::createJSAPI()
{
    // m_host is the BrowserHost
    FB::JSAPIPtr retVal = boost::make_shared<HPCCSystemsGraphViewControlAPI>(FB::ptr_cast<HPCCSystemsGraphViewControl>(shared_from_this()), m_host, m_wnd);
	if(m_wnd)
		m_wnd->m_api = static_cast<HPCCSystemsGraphViewControlAPI *>(retVal.get());
	return retVal;
}

bool HPCCSystemsGraphViewControl::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *)
{
#if defined(XP_UNIX)
    m_wnd->OnLButtonDown(hpcc::PointD(evt->m_x, evt->m_y));
#endif
    return true;
}

bool HPCCSystemsGraphViewControl::onMouseDoubleClick(FB::MouseDoubleClickEvent *evt, FB::PluginWindow *)
{
#if defined(XP_UNIX)
    m_wnd->OnLButtonDblClk(hpcc::PointD(evt->m_x, evt->m_y));
#endif
	return true;
}

bool HPCCSystemsGraphViewControl::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *)
{
#if defined(XP_UNIX)
    m_wnd->OnLButtonUp(hpcc::PointD(evt->m_x, evt->m_y), evt->m_state);
#endif
    return true;
}

bool HPCCSystemsGraphViewControl::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *)
{
#if defined(XP_UNIX)
    m_wnd->OnMouseMove(hpcc::PointD(evt->m_x, evt->m_y));
#endif
    return true;
}

bool HPCCSystemsGraphViewControl::onMouseScroll(FB::MouseScrollEvent *evt, FB::PluginWindow *)
{
#if defined(XP_UNIX)
    m_wnd->OnMouseScroll(hpcc::PointD(evt->m_x, evt->m_y), hpcc::PointD(evt->m_dx, evt->m_dy), evt->m_state);
#endif
    return true;
}

bool HPCCSystemsGraphViewControl::onRefresh(FB::RefreshEvent *evt, FB::PluginWindow *)
{
#if defined(XP_UNIX)
	hpcc::RectI bounds(evt->bounds.left, evt->bounds.top, evt->bounds.right - evt->bounds.left, evt->bounds.bottom - evt->bounds.top);
    m_wnd->DoPaint(bounds);
#endif
	return true;
}

void HPCCSystemsGraphViewControl::DoRender(int x, int y, int width, int height, bool resized)
{
}

#ifdef XP_WIN
bool HPCCSystemsGraphViewControl::onWindows(FB::WindowsEvent *evt, FB::PluginWindow *)
{
	switch(evt->uMsg)
	{
	case WM_ERASEBKGND:
		evt->lRes = 1;
		return true;
	case WM_SIZE:
		{
			//if (CDotView * win = m_Window->get_as<CDotView>())
			{
				CRect rcClient;
				GetClientRect(evt->hWnd, rcClient);
				m_wnd->MoveWindow(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height());
			}
		}
		break;
	case WM_MOUSEWHEEL:
		{
			ATLTRACE("HPCCSystemsGraphViewControl::onWindows\n");
			m_wnd->SendMessage(evt->uMsg, evt->wParam, evt->lParam);
			return true;
		}
		break;
	}
	return false;
}
#endif

bool HPCCSystemsGraphViewControl::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow * pw)
{
#if defined(XP_UNIX)
	if (CDotView * win = GetWindow()->get_as<CDotView>())
	{
		assert(m_wnd == NULL);
		m_wnd = win;
		//m_wnd->LoadTestData();
		if (FB::JSAPIPtr root_api = getRootJSAPI())
		{
			HPCCSystemsGraphViewControlAPI * api = static_cast<HPCCSystemsGraphViewControlAPI *>(root_api.get());
            assert(api != NULL);
			m_wnd->m_api = api;
			api->m_callback = m_wnd;
		}
	}
#elif defined (XP_WIN)
	if (FB::PluginWindowWin * win = GetWindow()->get_as<FB::PluginWindowWin>())
	{
		assert(m_wnd == NULL);
		m_wnd = new CDotView();
		CRect rc;
		::GetClientRect(win->getHWND(), rc);
		m_wnd->Create(win->getHWND(), rc, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_TABSTOP, WS_EX_CLIENTEDGE);
		//m_wnd->LoadTestData();
		if (FB::JSAPIPtr root_api = getRootJSAPI())
		{
			HPCCSystemsGraphViewControlAPI * api = static_cast<HPCCSystemsGraphViewControlAPI *>(root_api.get());
			m_wnd->m_api = api;
			api->m_callback = m_wnd;
		}
	}
#endif
	return false;
}

bool HPCCSystemsGraphViewControl::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *)
{
	assert(m_wnd != NULL);
#if defined(XP_UNIX)
#elif defined (XP_WIN)
	m_wnd->DestroyWindow();
	delete m_wnd;
#endif
	m_wnd = NULL;
	return false;
}

