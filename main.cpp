// ---------------------------------------------------------------------------

#include <vcl.h>

#pragma link "madExcept"
#pragma link "madLinkDisAsm"

#include "main.h"
#include "FtpThread.h"

#include "FileCheck.h"
#include <pngimage.hpp>
#include <vector>
#include "EventThread.h"
#include "WriteThread.h"
#include "../fatftp/ff.h"
#include "functions.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#pragma hdrstop
using namespace std;
EventThread *et = 0;
extern HINSTANCE hi;
TForm1 *Form1;
unsigned long ntmt = 0;
unsigned long tmtsum = 0;
unsigned long tmt = 0;
bool showftptraff = true;
bool showedeb = false;
bool stopftpthread = false;
bool showcachedeb = false;
bool showevents = false;
bool showdev = false;
bool formshown = false;
extern bool mounted;
FtpThread *ft = 0;
double memmb = 0;

int evcsenters = 0;
int pasvconnecttm = 0;
unsigned long eventid_int = 0;
vector<FATEVENT>events;
int eventslockline = 0;

bool _eventstrylock(char *func, int line)
{
	if (!et)
		return false;

	if (et->evcs->TryEnter())
	{
		eventslockline = line;
		evcsenters++;

		return true;
	}
	return false;
}

bool _eventslock(char *func, int line)
{
	bool trye;

	if (!et)
		return false;

	et->evcs->Enter();

	evcsenters++;
	eventslockline = line;
	return true;
}

int _eventslocked(char *func, int line)
{
	if (!et)
		return false;
	return eventslockline;
}

bool _eventsunlock(char *func, int line)
{
	if (!et)
		return false;

	et->evcs->Leave();

	evcsenters--;

	eventslockline = 0;

	return true;
}

unsigned long eventid(void)
{

	return eventid_int++;
}

LPVOID event(int type, LARGE_INTEGER numevent, LPVOID arg, LPVOID arg2, LPVOID arg3, LPVOID arg4,
	LPVOID arg5, LPVOID arg6, LPVOID arg7, LPVOID arg8)
{
	static DWORD lasteventtime = GetTickCount();
	FATEVENT e;

	if (events.size() >= 100)
		return 0;
	// OutputDebugString("event() start");

	// deb("event(%d, %x", type, arg);

	DWORD stms = GetTickCount();
	// while (eventslocked())
	// {
	// deb("eventslocked @ %05d, waiting %5d ms...", eventslocked(), GetTickCount() - stms);
	// Sleep(40 + (rand() % 200));
	//
	// }

	e.type = type;
	int ss = 0;
	LPVOID x = 0;

	if ((type == E_DEB || type == E_FTPCONTROL || type == E_SERVERBANNER || type == E_FATWRITEFILE)
		&& arg)
	{
		ss = strlen((char*)arg) + 10;
		if (!ss)
			return arg;

		try
		{
			// deb("HeapAlloc(%d, %d)",GetProcessHeap(),ss+1);
			x = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ss + 1);
			if (!x)
			{
				deb("HeapAlloc %d: %s", ss + 1, fmterr());
			}
		}
		catch(Exception & e)
		{
			fdeb("except while alocing mem for E_DEB arg %p, ss= %d,x=%p", arg, ss, x);
			char sss[1111];
			deunicode(e.Message.c_str(), sss, sizeof(sss));
			fdeb("except %s", sss);
			int err = GetLastError();
			fdeb("err %d: %s", err, fmterr(err));
			fdeb("string=%s", arg);

		}

		if (!x)
		{
			deb("no buf allocated for x %p %d", x, strlen((char*)arg));
			// ds;
			e.arg = arg;

			// e.arg2 = (LPVOID)0;
		}
		else
		{
			// e.arg2 = (LPVOID)ss;
			strcpy((char*)x, (char*)arg);
			// sprintf(x, "%4X| %s", GetCurrentThreadId(), arg);
			e.arg = x;

			// deb("allocated deb buf %p = %lu (ss %d)", arg, HeapSize(GetProcessHeap(), 0, x),
			// ss + 1);
		}
		e.arg2 = arg2;
	}
	else
	{
		e.arg = arg;
		e.arg2 = arg2;
	}

	e.ticks = GetTickCount();
	e.arg3 = arg3;
	e.arg4 = arg4;
	e.arg5 = arg5;
	e.arg6 = arg6;
	e.arg7 = arg7;
	e.id = eventid(); // numevent;
	e.arg8 = arg8;
	// eventid_int = e.id;
	// e.timearrive = (LARGE_INTEGER)numevent;
	e.thread = GetCurrentThreadId();
	// deb("e.thread=%4x %s",e.thread,e.type==0?(char*)e.arg:"");

	// deb("inserting e %d...", type);
	try
	{
		eventslock();

		events.push_back(e);
		eventsunlock();
	}
	catch(...)
	{
		fdeb("except while events adding event %d", type);
	}

	// if (GetTickCount() - lasteventtime >= 10)
	// Application->ProcessMessages();
	// events.insert(events.begin(), e);

	// deb("inserted #%4d", events.size());
	lasteventtime = GetTickCount();
}

void __fastcall TForm1::IdleHandler(TObject *Sender, bool &Done)
{

	memmb = (double)heapsize();
}

__fastcall TForm1::TForm1(TComponent * Owner) : TForm(Owner)
{
	events.clear();
	deb("hinstance %x", hi);

	deb("rand %d", rand());
	// HeapSetInformation(GetProcessHeap(), HeapEnableTerminationOnCorruption, 0, 0);

	Application->OnIdle = IdleHandler;

	double hs = (double)heapsize();
	et = new EventThread(false);

	deb("heapsize %.4f MB", hs / 1024.0 / 1024.0);
	// memcpy((void*)rand(),(void*)rand(),rand());

	deblog->Column[0]->Width = Width - 20;
	deblog->Font->Handle = getfont("lucida console", 7, 8, 4);

	ff_Init();

	deb("fatftp.dll build crc=%x", ff_Version());

	readaheadsize->Position = 4 * 1024 * 1024;

	Application->HintPause = 0;
	Application->HintShortPause = 0;
	Application->HintHidePause = 10000000000;

	showdev = Form1->dev->Checked;
	TPngImage *dwn;
	dwn = new TPngImage();
	dwn->LoadFromResourceName(0, "dwn");

	readaheadsize->Hint =
		 "������ ������ ������������ (����������������) ������. ������ ������ �� ������ �������.\r\n������ ����������� ������ ������������ fatftp.dll � ����������� �� �������� ����� ff_ReadAheadMinCache(size).\r\n� ������� ��� ��������� AVI ����� ������� �������� ������� � ������ �� 100 �� 300 ��.";
	deb("form init one");
	ff_AddEventHandler(event);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject * Sender)
{
	showedeb = edeb->Checked;
	showftptraff = ftptraff->Checked;
	showcachedeb = cache->Checked;
	if (anon->Checked)
	{
		user->Text = "";
		pass->Text = "";
	}

	if (ft)
	{
		stopftpthread = true;
		Sleep(100);
		// ft->WaitFor();
		ft->Terminate();
	}

	stopftpthread = false;
	ft = new FtpThread(false);
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::deblogDblClick(TObject * Sender)
{
	events.clear();
	deblog->Clear();
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject * Sender, TCloseAction & Action)
{
	stopftpthread = true;

	try
	{
		// ft->Terminate();
		// ff_Clear();
		kill_processes("fatftpapp");

	}
	catch(...)
	{
		deb("excep formclose");
	}
}
// ---------------------------------------------------------------------------

void __fastcall TForm1::filelistClick(TObject * Sender)
{
	FileCheck *fc = new FileCheck(false);
}

// ---------------------------------------------------------------------------

void __fastcall TForm1::deblogMouseDown(TObject * Sender, TMouseButton Button, TShiftState Shift,
	int X, int Y)
{
	if (Button != mbRight)
		return;

	HGLOBAL hglbCopy;
	LPTSTR lptstrCopy;

	char *buf;
	buf = new char[4 * 1024 * 1024];

	memset(buf, 0, 4 * 1024 * 1024);

	for (int i = 0; i < deblog->Items->Count; i++)
	{

		char szs[1111];

		deunicode(deblog->Items->Item[i]->Caption.c_str(), szs, sizeof(szs));
		// strcpy(szs,deblog->Items->Item[i]->SubItems[0].Text.c_str());
		// deb("add %s", szs);
		strcat(buf, szs);
		strcat(buf, "\r\n");

	}
	// deb("copylen %d", strlen(buf));

	if (!OpenClipboard(NULL))
	{
		deb("oppen clip: %s", fmterr());
		return;

	}

	// deb("op clip");
	EmptyClipboard();
	hglbCopy = GlobalAlloc(GMEM_MOVEABLE, strlen(buf) + 1);
	if (hglbCopy == NULL)
	{
		deb("globalalloc: %s", fmterr());
		CloseClipboard();
		return;
	}
	lptstrCopy = (char*)GlobalLock(hglbCopy);
	strcpy(lptstrCopy, buf);
	// lptstrCopy[strlen(buf)] = (TCHAR)0; // null character

	if (!SetClipboardData(CF_TEXT, hglbCopy))
		deb("fail set clip: %s", fmterr());
	else
		deb("clip ok");
	GlobalUnlock(hglbCopy);
	GlobalFree(hglbCopy);
	CloseClipboard();
	delete[]buf;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::ftptraffClick(TObject * Sender)
{
	showftptraff = Form1->ftptraff->Checked;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::eventslClick(TObject * Sender)
{
	showevents = eventsl->Checked;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::tcpnodelayClick(TObject * Sender)
{
	ff_SocketNoDelay(Form1->tcpnodelay->Checked);

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::oobinlineClick(TObject * Sender)
{

	ff_SocketOOBInline(Form1->oobinline->Checked);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::anonClick(TObject * Sender)
{
	if (anon->Checked)
	{
		user->Text = "";
		pass->Text = "";
	}
	else
	{
		user->Text = "xyu";
		pass->Text = "xyu";
	}
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::edebClick(TObject * Sender)
{
	showedeb = edeb->Checked;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject * Sender)
{
	ff_Debug(1);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::FormResize(TObject * Sender)
{
	deblog->Width = Width - 15;
	deblog->Column[0]->Width = Width - 20;
	deblog->Height = (Height - tcpnodelay->Top) - 45;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::cacheClick(TObject * Sender)
{
	showcachedeb = cache->Checked;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::readaheadsizeChange(TObject * Sender)
{
	unsigned long rac;

	rac = (unsigned long)readaheadsize->Position;
	// deb("����������� ��� %.2f MB (%lu �)", (double)rac / 1024.0 / 1024.0, rac);

	ff_ReadAheadMinCache(rac);

}

// ---------------------------------------------------------------------------
void __fastcall TForm1::devClick(TObject *Sender)
{
	showdev = Form1->dev->Checked;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::FormShow(TObject *Sender)
{

	formshown = true;
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::deblogKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	static int nextq = 0;

	deblog->Font->Handle = getfont("lucida console", 7, 8, nextq);

	if (nextq++ >= 6)
		nextq = 0;
	deb("nextquality %d", nextq);
}

// ---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender)
{
	WriteThread *wt = new WriteThread(false);
}
// ---------------------------------------------------------------------------
