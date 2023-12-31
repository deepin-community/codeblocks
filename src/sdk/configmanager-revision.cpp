/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision: 12708 $
 * $Id: configmanager-revision.cpp 12708 2022-02-08 08:42:14Z wh11204 $
 * $HeadURL: file:///svn/p/codeblocks/code/trunk/src/sdk/configmanager-revision.cpp $
 */

/* ------------------------------------------------------------------------------------------------------------------
*  Decouple the autorevisioning code, so we don't need to recompile the whole manager each time.
*  It's bad enough we have to re-link the SDK...
*/

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "configmanager.h"
    #include <wx/string.h>
#endif

#include "autorevision.h"

wxString ConfigManager::GetRevisionString()
{
    static_assert(wxMinimumVersion<3,0,0>::eval, "wxWidgets 3.0.0 or higher is required");

    return autorevision::svnRevision;
}

unsigned int ConfigManager::GetRevisionNumber()
{
    return autorevision::svn_revision;
}

wxString ConfigManager::GetSvnDate()
{
    return autorevision::svnDate;
}

