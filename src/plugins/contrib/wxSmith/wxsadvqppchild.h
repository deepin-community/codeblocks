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
* $Id: wxsadvqppchild.h 12722 2022-02-23 09:31:26Z wh11204 $
* $HeadURL: file:///svn/p/codeblocks/code/trunk/src/plugins/contrib/wxSmith/wxsadvqppchild.h $
*/

#ifndef WXSADVQPPCHILD_H
#define WXSADVQPPCHILD_H

#include <cbplugin.h>

#include <wx/panel.h>
#include "wxsadvqpp.h"

/** \brief Child of advanced quick props panel */
class PLUGIN_EXPORT wxsAdvQPPChild : public wxPanel
{
    public:

        /** \brief Ctor */
        wxsAdvQPPChild(wxsAdvQPP* Parent,const wxString Title);

        /** \brief Dctor */
        virtual ~wxsAdvQPPChild();

        /** \brief Getting container class
         *  \note You should always compare returned value to 0 since
         *        container may be accidentally unbinded from quick properties
         *        panel.
         */
        inline wxsPropertyContainer* GetPropertyContainer()
        {
            return Parent->GetPropertyContainer();
        }

    protected:

        /** \brief Function which notifies that content of panel should be reloaded */
        virtual void Update() = 0;

        /** \brief Function which can be called to notify that user changed value of any property in this panel */
        inline void NotifyChange()
        {
            Parent->NotifyChange();
        }

    private:

        wxsAdvQPP* Parent;

        friend class wxsAdvQPP;
};

#endif
