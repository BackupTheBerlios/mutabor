//
// This file was automatically generated by wxrc, do not edit by hand.
//

#ifndef __wxresource_h__
#define __wxresource_h__
class ConfigDlg : public wxDialog {
protected:
 wxRadioBox* ToneSystem;
 wxCheckBox* SaveEditor;
 wxCheckBox* ColorBars;
 wxButton* wxID_OK;
 wxButton* wxID_CANCEL;
 wxButton* wxID_HELP;

private:
 void InitWidgetsFromXRC(wxWindow *parent){
  wxXmlResource::Get()->LoadObject(this,parent,_T("ConfigDlg"), _T("wxDialog"));
  ToneSystem = XRCCTRL(*this,"ToneSystem",wxRadioBox);
  SaveEditor = XRCCTRL(*this,"SaveEditor",wxCheckBox);
  ColorBars = XRCCTRL(*this,"ColorBars",wxCheckBox);
  wxID_OK = XRCCTRL(*this,"wxID_OK",wxButton);
  wxID_CANCEL = XRCCTRL(*this,"wxID_CANCEL",wxButton);
  wxID_HELP = XRCCTRL(*this,"wxID_HELP",wxButton);
 }
public:
ConfigDlg(wxWindow *parent=NULL){
  InitWidgetsFromXRC((wxWindow *)parent);
 }
};
class AboutDlg : public wxDialog {
protected:
 wxHtmlWindow* m_htmlWin2;
 wxButton* wxID_OK;

private:
 void InitWidgetsFromXRC(wxWindow *parent){
  wxXmlResource::Get()->LoadObject(this,parent,_T("AboutDlg"), _T("wxDialog"));
  m_htmlWin2 = XRCCTRL(*this,"m_htmlWin2",wxHtmlWindow);
  wxID_OK = XRCCTRL(*this,"wxID_OK",wxButton);
 }
public:
AboutDlg(wxWindow *parent=NULL){
  InitWidgetsFromXRC((wxWindow *)parent);
 }
};
class CompileDlg : public wxDialog {
protected:
 wxStaticText* m_staticText1;
 wxStaticText* filename;
 wxStaticText* m_staticText3;
 wxStaticText* line;
 wxStaticText* m_staticText5;
 wxStaticText* logic;
 wxStaticText* tonelabel;
 wxStaticText* tones;
 wxStaticText* m_staticText9;
 wxStaticText* tunes;
 wxStaticLine* m_staticline1;
 wxStaticText* m_staticText6;
 wxStaticText* tone_system;
 wxStaticText* m_staticText8;
 wxStaticText* intervals;
 wxStaticText* m_staticText10;
 wxStaticText* chars;
 wxStaticText* message;

private:
 void InitWidgetsFromXRC(wxWindow *parent){
  wxXmlResource::Get()->LoadObject(this,parent,_T("CompileDlg"), _T("wxDialog"));
  m_staticText1 = XRCCTRL(*this,"m_staticText1",wxStaticText);
  filename = XRCCTRL(*this,"filename",wxStaticText);
  m_staticText3 = XRCCTRL(*this,"m_staticText3",wxStaticText);
  line = XRCCTRL(*this,"line",wxStaticText);
  m_staticText5 = XRCCTRL(*this,"m_staticText5",wxStaticText);
  logic = XRCCTRL(*this,"logic",wxStaticText);
  tonelabel = XRCCTRL(*this,"tonelabel",wxStaticText);
  tones = XRCCTRL(*this,"tones",wxStaticText);
  m_staticText9 = XRCCTRL(*this,"m_staticText9",wxStaticText);
  tunes = XRCCTRL(*this,"tunes",wxStaticText);
  m_staticline1 = XRCCTRL(*this,"m_staticline1",wxStaticLine);
  m_staticText6 = XRCCTRL(*this,"m_staticText6",wxStaticText);
  tone_system = XRCCTRL(*this,"tone_system",wxStaticText);
  m_staticText8 = XRCCTRL(*this,"m_staticText8",wxStaticText);
  intervals = XRCCTRL(*this,"intervals",wxStaticText);
  m_staticText10 = XRCCTRL(*this,"m_staticText10",wxStaticText);
  chars = XRCCTRL(*this,"chars",wxStaticText);
  message = XRCCTRL(*this,"message",wxStaticText);
 }
public:
CompileDlg(wxWindow *parent=NULL){
  InitWidgetsFromXRC((wxWindow *)parent);
 }
};
class mutabor_box : public wxDialog {
protected:
 wxRadioButton* use_box;
 wxTextCtrl* m_textCtrl2;
 wxRadioButton* guido_box;
 wxFilePickerCtrl* m_filePicker1;
 wxRadioButton* no_box;
 wxRadioBox* m_radioBox2;
 wxButton* wxID_OK;
 wxButton* wxID_CANCEL;
 wxButton* wxID_HELP;

private:
 void InitWidgetsFromXRC(wxWindow *parent){
  wxXmlResource::Get()->LoadObject(this,parent,_T("mutabor_box"), _T("wxDialog"));
  use_box = XRCCTRL(*this,"use_box",wxRadioButton);
  m_textCtrl2 = XRCCTRL(*this,"m_textCtrl2",wxTextCtrl);
  guido_box = XRCCTRL(*this,"guido_box",wxRadioButton);
  m_filePicker1 = XRCCTRL(*this,"m_filePicker1",wxFilePickerCtrl);
  no_box = XRCCTRL(*this,"no_box",wxRadioButton);
  m_radioBox2 = XRCCTRL(*this,"m_radioBox2",wxRadioBox);
  wxID_OK = XRCCTRL(*this,"wxID_OK",wxButton);
  wxID_CANCEL = XRCCTRL(*this,"wxID_CANCEL",wxButton);
  wxID_HELP = XRCCTRL(*this,"wxID_HELP",wxButton);
 }
public:
mutabor_box(wxWindow *parent=NULL){
  InitWidgetsFromXRC((wxWindow *)parent);
 }
};
class MyDialog5 : public wxDialog {
protected:

private:
 void InitWidgetsFromXRC(wxWindow *parent){
  wxXmlResource::Get()->LoadObject(this,parent,_T("MyDialog5"), _T("wxDialog"));
 }
public:
MyDialog5(wxWindow *parent=NULL){
  InitWidgetsFromXRC((wxWindow *)parent);
 }
};

void 
InitXmlResource();
#endif