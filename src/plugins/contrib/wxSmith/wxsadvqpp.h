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
* $Id: wxsadvqpp.h 12722 2022-02-23 09:31:26Z wh11204 $
* $HeadURL: file:///svn/p/codeblocks/code/trunk/src/plugins/contrib/wxSmith/wxsadvqpp.h $
*/

#ifndef WXSADVQPP_H
#define WXSADVQPP_H

#include <cbplugin.h>

#include "properties/wxsquickpropspanel.h"
#include <wx/button.h>
#include <wx/sizer.h>

class wxsAdvQPPChild;

/** \brief Advanced quick properties panel
 *
 * This panel is extension of standard wxsQuickPropsPanel.
 * It consist of set of wxsAdvQPPChild panels, each one groups set of
 * properties. All wxsAdvQPPChild classes must be manually registered using
 * wxsAdvQPP::Register function
 *
 * Each property group has it's title and can be hidden when it's unnecessary.
 */
class PLUGIN_EXPORT wxsAdvQPP : public wxsQuickPropsPanel
{
    public:
        /** \brief Ctor
         *
         * Number of parameters was reduced comparing to wxsQuickPropsPanel's
         * constructor. The reason is that this panel will be used in editor
         * area and some parameters must have specified values.
         */
        wxsAdvQPP(wxWindow* Parent,wxsPropertyContainer* Container);

        /** \brief Dctor */
        virtual ~wxsAdvQPP();

        /** \brief Function registering child panel */
        void Register(wxsAdvQPPChild* Child,const wxString& Title);

    private:

        /** \brief Function updating content of all child panels */
        void Update();

        /** \brief Overriding NotifyChange function to give access from
         *         wxsAdvQPPChild
         */
        inline void NotifyChange() { wxsQuickPropsPanel::NotifyChange(); }

        /** \brief Notification on toggling button press */
        void OnToggleButton(wxCommandEvent& event);

        WX_DEFINE_ARRAY(wxsAdvQPPChild*,wxArrayAdvQPPChild);
        WX_DEFINE_ARRAY(wxButton*,wxArrayButton);

        wxArrayAdvQPPChild Children;    ///< \brief Array of child panels
        wxArrayButton Buttons;          ///< \brief Buttons used to toggle
        wxBoxSizer* Sizer;              ///< \brief Sizer layouting panels

        DECLARE_EVENT_TABLE()

        friend class wxsAdvQPPChild;
};

#endif
