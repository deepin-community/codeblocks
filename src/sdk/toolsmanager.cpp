/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision: 12998 $
 * $Id: toolsmanager.cpp 12998 2022-10-30 16:47:04Z wh11204 $
 * $HeadURL: file:///svn/p/codeblocks/code/trunk/src/sdk/toolsmanager.cpp $
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/intl.h>
    #include <wx/process.h>
    #include <wx/menu.h>
    #include <wx/msgdlg.h>

    #include "toolsmanager.h"
    #include "manager.h"
    #include "macrosmanager.h"
    #include "configmanager.h"
    #include "logmanager.h"
    #include "pipedprocess.h"
    #include "globals.h"
    #include "sdk_events.h"
#endif

#include <wx/mdi.h>
#include <wx/listimpl.cpp>
#include "configuretoolsdlg.h"

template<> ToolsManager* Mgr<ToolsManager>::instance = nullptr;
template<> bool  Mgr<ToolsManager>::isShutdown = false;

WX_DEFINE_LIST(ToolsList);

const int idToolsConfigure = wxNewId();
const int idToolProcess = wxNewId();

BEGIN_EVENT_TABLE(ToolsManager, wxEvtHandler)
    EVT_MENU(idToolsConfigure, ToolsManager::OnConfigure)

    EVT_IDLE(ToolsManager::OnIdle)

    EVT_PIPEDPROCESS_STDOUT(idToolProcess, ToolsManager::OnToolStdOutput)
    EVT_PIPEDPROCESS_STDERR(idToolProcess, ToolsManager::OnToolErrOutput)
    EVT_PIPEDPROCESS_TERMINATED(idToolProcess, ToolsManager::OnToolTerminated)
END_EVENT_TABLE()

ToolsManager::ToolsManager()
    : m_Menu(nullptr),
    m_pProcess(nullptr),
    m_Pid(0)
{
    LoadTools();
    Manager::Get()->GetAppWindow()->PushEventHandler(this);
}

ToolsManager::~ToolsManager()
{
    // this is a core manager, so it is removed when the app is shutting down.
    // in this case, the app has already un-hooked us, so no need to do it ourselves...
//    Manager::Get()->GetAppWindow()->RemoveEventHandler(this);

    m_ItemsManager.Clear();

    // free-up any memory used for tools
    m_Tools.DeleteContents(true);
    m_Tools.Clear();
}

void ToolsManager::CreateMenu(cb_unused wxMenuBar* menuBar)
{
}

void ToolsManager::ReleaseMenu(cb_unused wxMenuBar* menuBar)
{
}

bool ToolsManager::Execute(const cbTool* tool)
{
    if (m_pProcess)
    {
        cbMessageBox(_("Another tool is currently executing.\n"
                        "Please allow for it to finish before launching another tool..."),
                        _("Error"), wxICON_ERROR);
        return false;
    }

    if (!tool)
        return false;

    wxString cmdline;
    wxString cmd = tool->GetCommand();
    wxString params = tool->GetParams();
    wxString dir = tool->GetWorkingDir();

    // hack to force-update macros
    Manager::Get()->GetMacrosManager()->RecalcVars(nullptr, nullptr, nullptr);

    Manager::Get()->GetMacrosManager()->ReplaceMacros(cmd);
    Manager::Get()->GetMacrosManager()->ReplaceMacros(params);
    Manager::Get()->GetMacrosManager()->ReplaceMacros(dir);

    if (tool->GetLaunchOption() == cbTool::LAUNCH_NEW_CONSOLE_WINDOW)
    {
#ifndef __WXMSW__
        // for non-win platforms, use m_ConsoleTerm to run the console app
        wxString term = Manager::Get()->GetConfigManager(_T("app"))->Read(_T("/console_terminal"), DEFAULT_CONSOLE_TERM);
        term.Replace(_T("$TITLE"), _T("'") + tool->GetName() + _T("'"));
        cmdline << term << _T(" ");
        #define CONSOLE_RUNNER "cb_console_runner"
#else
        #define CONSOLE_RUNNER "cb_console_runner.exe"
#endif
        wxString baseDir = ConfigManager::GetExecutableFolder();
        if (wxFileExists(baseDir + wxT("/" CONSOLE_RUNNER)))
            cmdline << baseDir << wxT("/" CONSOLE_RUNNER " ");
    }

    if (!cmdline.Replace(_T("$SCRIPT"), cmd << _T(" ") << params))
        // if they didn't specify $SCRIPT, append:
        cmdline << cmd;

    if(!(Manager::Get()->GetMacrosManager()))
        return false; // We cannot afford the Macros Manager to fail here!
                      // What if it failed already?
    wxSetWorkingDirectory(dir);

    // log info so user can troubleshoot
    dir = wxGetCwd(); // read in the actual working dir
    Manager::Get()->GetLogManager()->Log(wxString::Format(_("Launching tool '%s': %s (in %s)"), tool->GetName(), cmdline, dir));

    bool pipe = true;
    int flags = wxEXEC_ASYNC;

    switch (tool->GetLaunchOption())
    {
        case cbTool::LAUNCH_NEW_CONSOLE_WINDOW:
            pipe = false; // no need to pipe output channels...
            break;

        case cbTool::LAUNCH_VISIBLE:
        case cbTool::LAUNCH_VISIBLE_DETACHED:
            flags |= wxEXEC_NOHIDE;
            pipe = false;
            break;

        case cbTool::LAUNCH_HIDDEN: // fall-through
        default:
            break; // use the default values of pipe and flags...
    }

    if (tool->GetLaunchOption() == cbTool::LAUNCH_VISIBLE_DETACHED)
    {
        int pid = wxExecute(cmdline, flags);

        if (!pid)
        {
            cbMessageBox(_("Couldn't execute tool. Check the log for details."), _("Error"), wxICON_ERROR);
            return false;
        }
        else
        {
            CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, LogManager::app_log);
            Manager::Get()->ProcessEvent(evtSwitch);        // switch to default log
         }
    }
    else
    {
        m_pProcess = new PipedProcess(&m_pProcess, this, idToolProcess, pipe, dir);
        m_Pid = wxExecute(cmdline, flags, m_pProcess);

        if (!m_Pid)
        {
            cbMessageBox(_("Couldn't execute tool. Check the log for details."), _("Error"), wxICON_ERROR);
            delete m_pProcess;
            m_pProcess = nullptr;
            m_Pid = 0;
            return false;
        }
        else
        {
            CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, LogManager::app_log);
            Manager::Get()->ProcessEvent(evtSwitch);        // switch to default log
        }
    }

    return true;
} // end of Execute

void ToolsManager::AddTool(const cbTool* tool, bool save)
{
    if (tool)
    {
        InsertTool(m_Tools.GetCount(), tool, save);
    }
} // end of AddTool

void ToolsManager::InsertTool(int position, const cbTool* tool, bool save)
{
    m_Tools.Insert(position, new cbTool(*tool));
    if (save)
    {
        SaveTools();
    }
} // end of InsertTool

void ToolsManager::RemoveToolByIndex(int index)
{
    int idx = 0;
    for (ToolsList::iterator it = m_Tools.begin(); it != m_Tools.end(); ++it)
    {
        if (idx == index)
        {
            m_Tools.erase(it);
            SaveTools();
            return;
        }
        ++idx;
    }
}

cbTool* ToolsManager::GetToolByMenuId(int id)
{
    for (ToolsList::iterator it = m_Tools.begin(); it != m_Tools.end(); ++it)
    {
        cbTool* tool = *it;
        if (tool->GetMenuId() == id)
            return tool;
    }
    return nullptr;
}

cbTool* ToolsManager::GetToolByIndex(int index)
{
    int idx = 0;
    for (ToolsList::iterator it = m_Tools.begin(); it != m_Tools.end(); ++it)
    {
        cbTool* tool = *it;
        if (idx == index)
            return tool;
        ++idx;
    }
    return nullptr;
}

void ToolsManager::LoadTools()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager("tools");
    wxArrayString list = cfg->EnumerateSubPaths("/");
    for (unsigned int i = 0; i < list.GetCount(); ++i)
    {
        cbTool tool;
        tool.SetName( cfg->Read("/" + list[i] + "/name"));
        if (tool.GetName().empty())
            continue;
        tool.SetCommand(cfg->Read("/" + list[i] + "/command"));
        if (tool.GetCommand().empty())
            continue;
        tool.SetParams(cfg->Read("/" + list[i] + "/params"));
        tool.SetWorkingDir(cfg->Read("/" + list[i] + "/workingDir"));
        tool.SetLaunchOption(static_cast<cbTool::eLaunchOption>(cfg->ReadInt("/" + list[i] + "/launchOption")));

        AddTool(&tool, false);
    }

    Manager::Get()->GetLogManager()->Log(wxString::Format(_("Configured %zu tools"), m_Tools.GetCount()));
}

void ToolsManager::SaveTools()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager("tools");
    wxArrayString list = cfg->EnumerateSubPaths("/");
    for (unsigned int i = 0; i < list.GetCount(); ++i)
    {
        cfg->DeleteSubPath(list[i]);
    }

    int count = 0;
    for (ToolsList::iterator it = m_Tools.begin(); it != m_Tools.end(); ++it)
    {
        cbTool* tool = *it;
        wxString elem;

        // prepend a 0-padded 2-digit number to keep ordering
        wxString tmp;
        tmp.Printf(_T("tool%2.2d"), count++);

        elem << '/' << tmp << '/';
        cfg->Write(elem + "name", tool->GetName());
        cfg->Write(elem + "command", tool->GetCommand());
        cfg->Write(elem + "params", tool->GetParams());
        cfg->Write(elem + "workingDir", tool->GetWorkingDir());
        cfg->Write(elem + "launchOption", static_cast<int>(tool->GetLaunchOption()));
    }
}

void ToolsManager::BuildToolsMenu(wxMenu* menu)
{
    // clear previously added menu items
    m_ItemsManager.Clear();

    // add menu items for tools
    m_Menu = menu;
    if (m_Menu->GetMenuItemCount() > 0)
    {
        m_ItemsManager.Add(menu, wxID_SEPARATOR, _T(""), _T(""));
    }

    for (ToolsList::iterator it = m_Tools.begin(); it != m_Tools.end(); ++it)
    {
        cbTool* tool = *it;
        if (tool->GetName() == CB_TOOLS_SEPARATOR)
        {
            m_ItemsManager.Add(menu, wxID_SEPARATOR, _T(""), _T(""));
            continue;
        }
        if (tool->GetMenuId() == -1)
        {
            tool->SetMenuId(wxNewId());
        }
        m_ItemsManager.Add(menu, tool->GetMenuId(), tool->GetName(), tool->GetName());
        Connect(tool->GetMenuId(), -1, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &ToolsManager::OnToolClick);
    }

    if (m_Tools.GetCount() > 0)
    {
        m_ItemsManager.Add(menu, wxID_SEPARATOR, _T(""), _T(""));
    }
    m_ItemsManager.Add(menu, idToolsConfigure, _("&Configure tools..."), _("Add/remove user-defined tools"));
}

int ToolsManager::Configure()
{
    CodeBlocksEvent event(cbEVT_MENUBAR_CREATE_BEGIN);
    Manager::Get()->ProcessEvent(event);

    ConfigureToolsDlg dlg(Manager::Get()->GetAppWindow());
    PlaceWindow(&dlg);
    dlg.ShowModal();
    SaveTools();
    BuildToolsMenu(m_Menu);

    CodeBlocksEvent event2(cbEVT_MENUBAR_CREATE_END);
    Manager::Get()->ProcessEvent(event2);

    return 0;
} // end of Configure

// events

void ToolsManager::OnConfigure(cb_unused wxCommandEvent& event)
{
    Configure();
}

void ToolsManager::OnToolClick(wxCommandEvent& event)
{
    cbTool* tool = GetToolByMenuId(event.GetId());
    if (!Execute(tool))
        cbMessageBox(wxString::Format(_("Could not execute %s"), tool->GetName()));
}

void ToolsManager::OnIdle(wxIdleEvent& event)
{
    if (m_pProcess)
    {
        if (m_pProcess->HasInput())
        {
            event.RequestMore();
        }
    }
    event.Skip();
}

void ToolsManager::OnToolStdOutput(CodeBlocksEvent& event)
{
    Manager::Get()->GetLogManager()->Log("stdout> "+event.GetString());
}

void ToolsManager::OnToolErrOutput(CodeBlocksEvent& event)
{
    Manager::Get()->GetLogManager()->Log("stderr> "+event.GetString());
}

void ToolsManager::OnToolTerminated(CodeBlocksEvent& event)
{
    m_Pid = 0;
    m_pProcess = nullptr;

    Manager::Get()->GetLogManager()->Log(wxString::Format(_("Tool execution terminated with status %d"), event.GetInt()));
}
