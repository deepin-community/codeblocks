/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef INSERTCLASSMETHODDLG_H
#define INSERTCLASSMETHODDLG_H

#include <wx/arrstr.h>
#include <wx/string.h>

#include "scrollingdialog.h"

class Parser;
class ParserBase;
class Token;
class wxCheckListBox;
class wxCommandEvent;

// When instance this class, *MUST* entered a critical section, and should call ShowModal() rather than the other
class InsertClassMethodDlg : public wxScrollingDialog
{
public:
    InsertClassMethodDlg(wxWindow* parent, ParserBase* parser, const wxString& filename);
    virtual ~InsertClassMethodDlg();

    wxArrayString GetCode() const; // return an array of checked methods

private:
    void FillClasses();
    void FillMethods();
    void OnClassesChange(wxCommandEvent& event);
    void OnCodeChange(wxCommandEvent& event);
    void OnFilterChange(wxCommandEvent& event);

    ParserBase* m_Parser;
    bool        m_Decl;
    wxString    m_Filename;

    DECLARE_EVENT_TABLE();
};

#endif // INSERTCLASSMETHODDLG_H

