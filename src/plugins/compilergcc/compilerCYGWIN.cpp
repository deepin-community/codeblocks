/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision: 12618 $
 * $Id: compilerCYGWIN.cpp 12618 2021-12-31 12:59:50Z wh11204 $
 * $HeadURL: file:///svn/p/codeblocks/code/trunk/src/plugins/compilergcc/compilerCYGWIN.cpp $
 */

#include <sdk.h>

#include "compilerCYGWIN.h"
#include "cygwin.h"

CompilerCYGWIN::CompilerCYGWIN()
    : CompilerMINGW(_("Cygwin GCC"), "cygwin")
{
    m_Weight = 32;
    Reset();
}

CompilerCYGWIN::~CompilerCYGWIN()
{
}

Compiler * CompilerCYGWIN::CreateCopy()
{
    return (new CompilerCYGWIN(*this));
}

AutoDetectResult CompilerCYGWIN::AutoDetectInstallationDir()
{
    if (platform::windows)
    {
        if (cbIsDetectedCygwinCompiler())
        {
            m_MasterPath = cbGetCygwinCompilerPathRoot();
            return adrDetected;
        }
        else
        {
            m_MasterPath = "C:\\cygwin64";
            return adrGuessed;
        }
    }
    else
    {
        m_MasterPath = cbGetCygwinCompilerPathRoot();
        return adrGuessed;
    }
}
