/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision: 12072 $
* $Id: wxsscrolledwindow.cpp 12072 2020-04-22 22:27:12Z bluehazzard $
* $HeadURL: file:///svn/p/codeblocks/code/trunk/src/plugins/contrib/wxSmith/wxwidgets/defitems/wxsscrolledwindow.cpp $
*/

#include "wxsscrolledwindow.h"

namespace
{
    wxsRegisterItem<wxsScrolledWindow> Reg(_T("ScrolledWindow"),wxsTContainer, _T("Standard"), 140);

    WXS_ST_BEGIN(wxsScrolledWindowStyles,_T("wxHSCROLL|wxVSCROLL"))
        WXS_ST_CATEGORY("wxScrolledWindow")
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsScrolledWindowEvents)
        WXS_EV_DEFAULTS()
    WXS_EV_END()
}

wxsScrolledWindow::wxsScrolledWindow(wxsItemResData* Data):
    wxsContainer(
        Data,
        &Reg.Info,
        wxsScrolledWindowEvents,
        wxsScrolledWindowStyles)
{}

void wxsScrolledWindow::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/scrolwin.h>"),GetInfo().ClassName,hfInPCH);
            Codef(_T("%C(%W, %I, %P, %S, %T, %N);\n"));
            BuildSetupWindowCode();
            AddChildrenCode();
            if(!m_scrollRate.DefValue)
            {
                Codef(_T("%ASetScrollRate(%d,%d);\n"), m_scrollRate.GetValue1(), m_scrollRate.GetValue2() );
            }
            return;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsScrolledWindow::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

wxObject* wxsScrolledWindow::OnBuildPreview(wxWindow* Parent,long Flags)
{
    // TODO: Use grid-viewing panel when not in exact mode
    wxWindow* NewItem = new wxScrolledWindow( Parent,GetId(),wxDefaultPosition,wxDefaultSize,Style());
    SetupWindow(NewItem,Flags);
    AddChildrenPreview(NewItem,Flags);
    return NewItem;
}

void wxsScrolledWindow::OnEnumContainerProperties(long Flags)
{
    if ( Flags & flSource )
    {
        WXS_TWOLONG( wxsScrolledWindow, m_scrollRate,  _("Default scroll rate"), _("Rate x"), _("Rate y"), _("scrollrate"), -1, -1);
    }
}

