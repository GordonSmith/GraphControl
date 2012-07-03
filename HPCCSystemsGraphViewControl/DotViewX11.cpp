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

#include <SvgShapes.h>
#include <SvgParser.h>

#include "DotViewX11.h"
#include "HPCCSystemsGraphViewControlAPI.h"
#include <util.h>
#include <XgmmlParser.h>

CDotView::CDotView(const FB::WindowContextX11 & ctx) : FB::PluginWindowX11(ctx), CDotViewCommon()
{
}

void CDotView::Invalidate()
{
	InvalidateWindow();
}

void CDotView::UpdateWindow()
{
}

int CDotView::GetScrollOffsetX()
{
	return m_ptOffset.x;
}

int CDotView::GetScrollOffsetY()
{
	return m_ptOffset.y;
}

void CDotView::SetScrollOffset(int x, int y)
{
    m_ptOffset.x = x < 0 ? 0 : (x > m_ptSize.x ? m_ptSize.x : x);
    m_ptOffset.y = y < 0 ? 0 : (y > m_ptSize.y ? m_ptSize.y : y);
    Invalidate();
}

void CDotView::SetScrollSize(int w, int h, bool redraw)
{
	gint width = FB::PluginWindowX11::getWindowWidth();
	gint height = FB::PluginWindowX11::getWindowHeight();

	m_ptSize.x = w - width;
	m_ptSize.y = h - height;
}

bool CDotView::GetClientRectangle(hpcc::RectD & rect)
{
	FB::Rect rect2 = FB::PluginWindowX11::getWindowPosition();
	rect.x = rect2.left;
	rect.y = rect2.top;
	rect.Width = rect2.right - rect2.left;
	rect.Height = rect2.bottom - rect2.top;
	FB::Rect rect3 = FB::PluginWindowX11::getWindowClipping();
	return true;
}

void CDotView::DoPaint(hpcc::RectI bounds)
{
	hpcc::RectI rect(bounds);
	rect.Offset(m_ptOffset.x, m_ptOffset.y);

	m_buffer->Resize(rect.Width(), rect.Height());
	m_gr->DoRender(rect, true);
	m_buffer->Draw(m_canvas, bounds.left, bounds.top);
}

void CDotView::OnLButtonDown(hpcc::PointD point)
{
	m_mouseDown = MOUSEDOWN_NORMAL;
	m_mouseDownPosX = point.x;
	m_mouseDownPosY = point.y;
	m_scrollDownPosX = GetScrollOffsetX();
	m_scrollDownPosY = GetScrollOffsetY();
}

void CDotView::OnLButtonUp(hpcc::PointD point, guint modifierState)
{
	switch (m_mouseDown)
	{
	case MOUSEDOWN_DBLCLK:
	case MOUSEDOWN_MOVED:
		m_mouseDown = MOUSEDOWN_UNKNOWN;
		return;
	}

	m_mouseDown = MOUSEDOWN_UNKNOWN;
	point.Offset(m_ptOffset);
	hpcc::IGraphItem * selectedItem = m_gr->GetItemAt(point.x, point.y);

	bool selChanged = false;
	if (selectedItem)
	{
		if (modifierState & FB::MouseButtonEvent::ModifierState_Control)
		{
			if (m_selection->IsSelected(selectedItem))
				selChanged = m_selection->Deselect(selectedItem);
			else
				selChanged = m_selection->Select(selectedItem, false);
		}
		else
			selChanged = m_selection->Select(selectedItem);
	}

	if (selChanged)
	{
		InvalidateSelection();

		std::vector<int> selection;
		m_selection->GetSelection(selection);

		FB::VariantList items;
		for(std::vector<int>::const_iterator itr = selection.begin(); itr != selection.end(); ++itr)
			items.push_back(*itr);

		m_api->fire_SelectionChanged(items);
	}
}

void CDotView::OnLButtonDblClk(hpcc::PointD point)
{
	m_mouseDown = MOUSEDOWN_DBLCLK;
	point.Offset(m_ptOffset);
	hpcc::PointD worldDblClk(point.x, point.y);
	worldDblClk = m_gr->ScreenToWorld(worldDblClk);

	hpcc::IGraphItem * item = m_gr->GetItemAt(worldDblClk.x, worldDblClk.y, true);
	m_api->fire_MouseDoubleClick(item ? item->GetID() : 0);
}

int CDotView::OnLayoutComplete(void * wParam, void * lParam)
{
	assert(wParam && lParam);
	std::string * dot = (std::string *)wParam;
	std::string * svg = (std::string *)lParam;
	if (boost::algorithm::equals(m_dot, *dot))
	{
		m_svg = *svg;
		hpcc::MergeSVG(m_g, *svg);
		CalcScrollbars();
		CenterOnGraphItem(NULL);
		Invalidate();
		m_api->fire_LayoutFinished();
	}
	delete dot;
	delete svg;
	return 0;
}

void CDotView::InvalidateSelection()
{
	Invalidate();
}

void CDotView::InvalidateScreenRect(const hpcc::RectD & screenRect)
{
	Invalidate();
}

void CDotView::InvalidateWorldRect(const hpcc::RectD & worldRect)
{
	Invalidate();
}

void CDotView::OnMouseMove(hpcc::PointD point)
{
	switch (m_mouseDown)
	{
	case MOUSEDOWN_NORMAL:
	case MOUSEDOWN_MOVED:
		{
			int deltaX = point.x - m_mouseDownPosX;
			int deltaY = point.y - m_mouseDownPosY;

			SetScrollOffset(m_scrollDownPosX - deltaX, m_scrollDownPosY - deltaY);
			if (deltaX || deltaY)
				m_mouseDown = MOUSEDOWN_MOVED;
		}
		break;
	default:
		{
			point.Offset(m_ptOffset);
			hpcc::IGraphItem * hotItem = NULL;
			if (!hotItem)
				hotItem = m_gr->GetItemAt(point.x, point.y);
			if (m_hotItem->Set(hotItem))
			{
				hpcc::RectD invalidateRect;
				if (m_hotItem->GetInvalidateRect(invalidateRect))
					InvalidateWorldRect(invalidateRect);
				if (m_hotItem->GetPrevInvalidateRect(invalidateRect))
					InvalidateWorldRect(invalidateRect);
			}
		}
		break;
	}
}

void CDotView::OnMouseScroll(hpcc::PointD pt, hpcc::PointD delta, guint modifierState)
{
	if (!(modifierState & FB::MouseButtonEvent::ModifierState_Control))
	{
		hpcc::PointD point = pt;
		point.Offset(m_ptOffset);
		hpcc::PointD worldDblClk(point.x, point.y);
		worldDblClk = m_gr->ScreenToWorld(worldDblClk);
		double zDelta = delta.x != 0 ? delta.x : delta.y;
		double scale = m_gr->GetScale();
		if (zDelta > 0)
			m_gr->SetScale(scale + 0.05 * scale);
		else
			m_gr->SetScale(scale - 0.05 * scale);

		CalcScrollbars();
		MoveTo(worldDblClk, pt.x, pt.y);

		m_api->fire_Scaled((int)(m_gr->GetScale() * 100));
	}
}

void CDotView::operator()(const std::string & dot, const std::string & svg)
{
	//PostMessage(UMDV_LAYOUT_COMPLETE, (WPARAM)new std::string(dot), (LPARAM)new std::string(svg));
	OnLayoutComplete(new std::string(dot), new std::string(svg));
}
