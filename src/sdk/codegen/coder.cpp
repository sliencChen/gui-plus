/*
 * coder.cpp
 *
 *  Created on: 2019-5-6
 *      Author: Brant
 */

#include <wx/filename.h>
#include "sdk_common.h"
#include "codegen/coder.h"
#include "codegen/utf8reader.h"

//namespace
//{
//    EncodingDetector* LoadFile(const wxString& FileName)
//    {
//        if ( !wxFileExists(FileName) ) return nullptr;
//
//        LoaderBase* Loader = FileManager::Get()->Load(FileName,false);
//        if ( !Loader ) return nullptr;
//
//        EncodingDetector* Detector = new EncodingDetector(Loader);
//        delete Loader;
//
//        if ( !Detector->IsOK() )
//        {
//            delete Detector;
//            return nullptr;
//        }
//
//        return Detector;
//    }
//}

static wxsCoder SingletonObject;
wxsCoder* wxsCoder::Singleton = &SingletonObject;

wxsCoder::wxsCoder()
{
//    FlushTimer.SetOwner(this,1);
//    Connect(wxEVT_TIMER,(wxObjectEventFunction)&wxsCoder::FlushTimerEvent);
}

wxsCoder::~wxsCoder()
{
    FlushAll();
}

void wxsCoder::AddCode(const wxString& FileName,const wxString& Header,const wxString& End,const wxString& Code,bool Immediately,bool CodeHasHeader,bool CodeHasEnd)
{
    wxMutexLocker Lock(DataMutex);

    wxString FixedFileName = NormalizeFileName(FileName);
    if ( FixedFileName.IsEmpty() )
    {
        return;
    }

    // Find changing file
    int Index = CodeChangesFiles.Index(FileName);
    if ( Index==wxNOT_FOUND )
    {
        Index = CodeChangesFiles.Count();
        CodeChangesFiles.Add(FileName);
        CodeChanges.Add(0);
    }

    // Add entry to list of waiting changes
    CodeChange* Change = new CodeChange;
    Change->Header = Header;
    Change->End = End;
    Change->Code = Code;
    Change->CodeHasHeader = CodeHasHeader;
    Change->CodeHasEnd = CodeHasEnd;
    Change->Next = CodeChanges[Index];
    CodeChanges[Index] = Change;

    // If the change has already been put onto queue, delete it
    for ( CodeChange *Prev=Change, *This=Prev->Next; This; Prev=This, This=This->Next )
    {
        if ( This->Header==Header && This->End==End )
        {
            Prev->Next = This->Next;
            delete This;
            This = Prev;
        }
    }

    if ( Immediately )
    {
        FlushFile(FixedFileName);
    }
}

wxString wxsCoder::GetCode(const wxString& FileName,const wxString& Header,const wxString& End,bool IncludeHeader,bool IncludeEnd)
{
    wxMutexLocker Lock(DataMutex);

    wxString FixedFileName = NormalizeFileName(FileName);
    FlushFile(FixedFileName);

    int TabSize = 4;

    Utf8Reader reader(FixedFileName);
	if ( !reader.IsOK() ) return _T("");

	wxString Content = reader.GetWxStr();

	int Position = Content.First(Header);
	if ( Position == -1 ) return _T("");
	int SpacesCut = 0;
	int SpacesPos = Position;
	while ( --SpacesPos >= 0 )
	{
		wxChar ch = Content.GetChar(SpacesPos);
		if ( ch == _T('\t') ) SpacesCut += TabSize;
		else if ( (ch==_T('\n')) || (ch==_T('\r')) ) break;
		else SpacesCut++;
	}

	if ( !IncludeHeader ) Position += Header.Length();
	Content.Remove(0,Position);
	int EndPosition = Content.First(End);
	if ( EndPosition == -1 ) return _T("");
	if ( IncludeEnd ) EndPosition += End.Length();
	Content.Remove(EndPosition);
	return CutSpaces(Content,SpacesCut);
}

wxString wxsCoder::GetFullCode(const wxString& FileName,wxFontEncoding& Encoding,bool &UseBOM)
{
    wxMutexLocker Lock(DataMutex);

    wxString FixedFileName = NormalizeFileName(FileName);
    FlushFile(FixedFileName);

    Utf8Reader reader(FixedFileName);
	return reader.IsOK() ? reader.GetWxStr() : _T("");
}

void wxsCoder::PutFullCode(const wxString& FileName,const wxString& Code,wxFontEncoding Encoding,bool UseBOM)
{
    wxMutexLocker Lock(DataMutex);

    wxString FixedFileName = NormalizeFileName(FileName);
    int Index = CodeChangesFiles.Index(FixedFileName);
    if ( Index != wxNOT_FOUND )
    {
        for ( CodeChange* Change=CodeChanges[Index]; Change; )
        {
            CodeChange* Next = Change->Next;
            delete Change;
            Change = Next;
        }
        CodeChanges[Index] = 0;
    }

	dsSaveToFile(FixedFileName, Code, Encoding, UseBOM);
}

void wxsCoder::FlushFile(const wxString& FileName)
{
    int Index = CodeChangesFiles.Index(FileName);
    if ( Index == wxNOT_FOUND ) return;

    CodeChange* Changes = CodeChanges[Index];
    if ( !Changes ) return;

	// Reading file content
	wxString EOL;
	bool HasChanged = false;

	//wxStopWatch SW;
	Utf8Reader reader(FileName);
	if ( !reader.IsOK() )
	{
		return;
	}

	wxString Content = reader.GetWxStr();
	while ( Changes )
	{
		CodeChange* Next = Changes->Next;
		ApplyChangesString(Content,Changes->Header,Changes->End,Changes->Code,Changes->CodeHasHeader,Changes->CodeHasEnd,HasChanged,EOL);
		delete Changes;
		Changes = Next;
	}

	if ( HasChanged )
	{
		dsSaveToFile(FileName, Content, wxFONTENCODING_UTF8, false);
	}

    CodeChanges[Index] = 0;
}

bool wxsCoder::ApplyChangesEditor(/*cbEditor* Editor,*/const wxString& Header,const wxString& End,wxString& Code,bool CodeHasHeader,bool CodeHasEnd,wxString& EOL)
{
//    cbStyledTextCtrl* Ctrl = Editor->GetControl();
//    int FullLength = Ctrl->GetLength();
//
//    if ( EOL.IsEmpty() )
//    {
//        // Detecting EOL style in source
//        for ( int i=0; i<FullLength; i++ )
//        {
//            wxChar ch = Ctrl->GetCharAt(i);
//            if ( ch==_T('\n') || ch==_T('\r') )
//            {
//                EOL = ch;
//                if ( ++i < FullLength )
//                {
//                    wxChar ch2 = Ctrl->GetCharAt(i);
//                    if ( (ch2==_T('\n') || ch2==_T('\r')) && ch!=ch2 )
//                    {
//                        EOL.Append(ch2);
//                    }
//                }
//                break;
//            }
//        }
//    }
//
//    // Searching for beginning of section to replace
//    Ctrl->SetSearchFlags(wxSCI_FIND_MATCHCASE);
//    Ctrl->SetTargetStart(0);
//    Ctrl->SetTargetEnd(FullLength);
//    int Position = Ctrl->SearchInTarget(Header);
//
//    if ( Position == -1 )
//    {
//        Manager::Get()->GetLogManager()->DebugLog(F(_("wxSmith: Couldn't find code with header:\n\t\"%s\"\nin file '%s'"),
//                                                    Header.wx_str(),
//                                                    Editor->GetFilename().wx_str()));
//        return false;
//    }
//
//    // Beginning of this code block is in Position, now searching for end
//    Ctrl->SetTargetStart(Position);
//    Ctrl->SetTargetEnd(FullLength);
//    int EndPosition = Ctrl->SearchInTarget(End);
//    if ( EndPosition == -1 )
//    {
//        Manager::Get()->GetLogManager()->DebugLog(F(_("wxSmith: Unfinished block of auto-generated code with header:\n\t\"%s\"\nin file '%s'"),
//                                                    Header.wx_str(),
//                                                    Editor->GetFilename().wx_str()));
//
//        return false;
//    }
//
//    // Fetching indentation
//    wxString BaseIndentation;
//    int IndentPos = Position;
//    while ( --IndentPos >= 0 )
//    {
//        wxChar ch = Ctrl->GetCharAt(IndentPos);
//        if ( (ch == _T('\n')) || (ch == _T('\r')) ) break;
//    }
//    while ( ++IndentPos < Position )
//    {
//        wxChar ch = Ctrl->GetCharAt(IndentPos);
//        BaseIndentation.Append(
//            ( ch == _T('\t') ) ? _T('\t') : _T(' '));
//    }
//
//    Code = RebuildCode(BaseIndentation,Code.c_str(),(int)Code.Length(),EOL);
//
//    // Fixing up positions to contain or not header / ending sequence
//    if ( !CodeHasHeader ) Position += Header.Length();
//    if ( CodeHasEnd ) EndPosition += End.Length();
//
//    // Checking of code has really changed
//    if ( Ctrl->GetTextRange(Position,EndPosition) == Code )
//    {
//        return true;
//    }
//
//    // Make sure that the code we're replacing is not folded. Otherwise scintilla
//    // does some weird things
//
//    int lineEnd = Ctrl->LineFromPosition(EndPosition);
//    for ( int line = Ctrl->LineFromPosition(Position); line <= lineEnd; line++ )
//    {
//        Ctrl->EnsureVisible(line);
//    }
//
//    // Replacing code
//    Ctrl->SetTargetStart(Position);
//    Ctrl->SetTargetEnd(EndPosition);
//    Ctrl->ReplaceTarget(Code);
//    Editor->SetModified();
    return true;
}

bool wxsCoder::ApplyChangesString(wxString& BaseContent,const wxString& Header,const wxString& End,wxString& Code,bool CodeHasHeader,bool CodeHasEnd,bool& HasChanged,wxString& EOL)
{
    wxString Content = BaseContent;
    if ( EOL.IsEmpty() )
    {
        // Detecting EOL in this sources
        for ( size_t i=0; i<Content.Length(); i++ )
        {
            wxChar ch = Content.GetChar(i);
            if ( ch==_T('\n') || ch==_T('\r') )
            {
                EOL = ch;
                if ( ++i < Content.Length() )
                {
                    wxChar ch2 = Content.GetChar(i);
                    if ( (ch2==_T('\n') || ch2==_T('\r')) && ch!=ch2 )
                    {
                        EOL.Append(ch2);
                    }
                }
                break;
            }
        }
    }

    // Search for header
    int Position = Content.First(Header);

    if ( Position == -1 )
    {
//        Manager::Get()->GetLogManager()->DebugLog(F(_("wxSmith: Couldn't find code with header:\n\t\"%s\""),Header.wx_str()));
    	wxLogMessage(wxString::Format("Couldn't find code with header:\n\t\"%s\"", Header.wx_str()));
        return false;
    }

    // Skipping header if necessary
    int IndentPos = Position;
    int IndentMax = Position;
    if ( !CodeHasHeader ) Position += Header.Length();

    wxString Result = Content.Left(Position);
    Content.Remove(0,Position);

    int EndPosition = Content.First(End);
    if ( EndPosition == -1 )
    {
//        Manager::Get()->GetLogManager()->DebugLog(F(_("wxSmith: Unfinished block of auto-generated code with header:\n\t\"%s\""),Header.wx_str()));
    	wxLogMessage(wxString::Format("Unfinished block of auto-generated code with header:\n\t\"%s\"", Header.wx_str()));
        return false;
    }

    // Including ending sequence if necessary
    if ( CodeHasEnd ) EndPosition += End.Length();

    // Fetching indentation
    wxString BaseIndentation;
    while ( --IndentPos >= 0 )
    {
        wxChar ch = Result.GetChar(IndentPos);
        if ( (ch == _T('\n')) || (ch == _T('\r')) ) break;
    }
    while ( ++IndentPos < IndentMax )
    {
        wxChar ch = Result.GetChar(IndentPos);
        BaseIndentation.Append(
            ( ch == _T('\t') ) ? _T('\t') : _T(' '));
    }

    Code = RebuildCode(BaseIndentation,Code.c_str(),Code.Length(),EOL);

    // Checking if code has really changed
    if ( Content.Mid(0,EndPosition) == Code )
    {
        return true;
    }

    HasChanged = true;
    Result += Code;
    Result += Content.Remove(0,EndPosition);
    BaseContent = Result;

    return true;
}

wxString wxsCoder::RebuildCode(wxString& BaseIndentation,const wxChar* Code,int CodeLen,wxString& EOL)
{
    wxString Tab;
    bool UseTab = true;
    int TabSize = 4;

    if ( !UseTab )
    {
        Tab.Append(_T(' '),TabSize);
    }

    if ( EOL.IsEmpty() )
        EOL = wxT("\n");//GetEOLStr();

    BaseIndentation.Prepend(EOL);

    wxString Result;
    Result.reserve(CodeLen+10);

    while ( *Code )
    {
        switch ( *Code )
        {
            case _T('\n'):
                {
                    while (!Result.IsEmpty() &&
                           (Result.Last() == _T(' ') || Result.Last() == _T('\t')))
                        Result.RemoveLast();
                    Result << BaseIndentation;
                    break;
                }
            case _T('\t'): if ( UseTab ) { Result << Tab; break; }
            // no break
            default:       Result << *Code;
        }
        Code++;
    }

    return Result;
}

wxString wxsCoder::CutSpaces(wxString Code,int Count)
{
    int TabSize = 4;
    if ( TabSize < 1 ) TabSize = 4;

    // Changing to \n line end mode
    wxString Result;

    for(;;)
    {
        int PosN = Code.Find(_T("\n"));
        int PosR = Code.Find(_T("\r"));

        if ( ( PosN < 0 ) && ( PosR < 0 ) ) break;

        int Pos;
        if ( PosN < 0 ) Pos = PosR;
        else if ( PosR < 0 ) Pos = PosN;
        else Pos = (PosN < PosR) ? PosN : PosR;

        Result.Append(Code.Left(Pos));
        Code.Remove(0,Pos);
        while ( Code.Length() )
        {
            if ( ( Code[0] != _T('\n') ) &&
                 ( Code[0] != _T('\r') ) ) break;
            Code.Remove(0,1);
        }
        int LeftSpaces = Count;
        while ( Code.Length() && LeftSpaces > 0 )
        {
            if ( Code[0] == _T(' ') ) LeftSpaces--;
            else if ( Code[0] == _T('\t') ) LeftSpaces -= TabSize;
            else break;
            Code.Remove(0,1);
        }
        Result.Append(_T('\n'));
    }

    Result.Append(Code);
    return Result;
}

wxString wxsCoder::NormalizeFileName(const wxString& FileName)
{
    // Updating the file name in case there are some ".." or "." enteries which prevents from finding
    // opened editor for file
    wxFileName FixedNameObject(FileName);
    FixedNameObject.Normalize(wxPATH_NORM_DOTS);
    return FixedNameObject.GetFullPath();
}

void wxsCoder::Flush(int Delay)
{
//    if ( Delay<=0 )
//    {
//        FlushTimer.Stop();
        FlushAll();
//    }
//    else
//    {
//        FlushTimer.Start(Delay,true);
//    }
}

void wxsCoder::FlushAll()
{
    //wxStopWatch SW;
    for ( int i=0; i<(int)CodeChangesFiles.Count(); i++ )
    {
        FlushFile(CodeChangesFiles[i]);
    }
    CodeChanges.Clear();
    CodeChangesFiles.Clear();
//    //Manager::Get()->GetLogManager()->DebugLog(F(_T("wxSmith: Flushing of code done in %d ms"),SW.Time()));
}

//void wxsCoder::FlushTimerEvent(cb_unused wxTimerEvent& event)
//{
//    FlushAll();
//}
