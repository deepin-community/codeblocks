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
* $Revision: 12722 $
* $Id: wxsarraystringeditordlg.h 12722 2022-02-23 09:31:26Z wh11204 $
* $HeadURL: file:///svn/p/codeblocks/code/trunk/src/plugins/contrib/wxSmith/properties/wxsarraystringeditordlg.h $
*/

#ifndef WXSARRAYSTRINGEDITORDLG_H
#define WXSARRAYSTRINGEDITORDLG_H

//(*Headers(wxsArrayStringEditorDlg)
#include <wx/sizer.h>
#include "scrollingdialog.h"
#include <wx/textctrl.h>
//*)

#include <cbplugin.h>

class PLUGIN_EXPORT wxsArrayStringEditorDlg: public wxScrollingDialog
{
    public:

        wxsArrayStringEditorDlg(wxWindow* parent,wxArrayString& Array,wxWindowID id = -1);
        virtual ~wxsArrayStringEditorDlg();

        //(*Identifiers(wxsArrayStringEditorDlg)
        static const long ID_TEXTCTRL1;
        //*)

    protected:

        //(*Handlers(wxsArrayStringEditorDlg)
        void OnOK(wxCommandEvent& event);
        void OnCancel(wxCommandEvent& event);
        //*)

        //(*Declarations(wxsArrayStringEditorDlg)
        wxTextCtrl* Items;
        //*)

    private:

        wxArrayString& Data;

        DECLARE_EVENT_TABLE()
};

#endif
