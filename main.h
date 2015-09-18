// ---------------------------------------------------------------------------
#pragma once
#ifndef mainH
#define mainH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <pngimage.hpp>

// ---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published: // IDE-managed Components
	TEdit *host;
	TEdit *user;
	TEdit *pass;
	TButton *Button1;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TCheckBox *anon;
	TListView *deblog;
	TListView *filelist;
	TCheckBox *ftptraff;
	TEdit *timeout;
	TLabel *Label4;
	TCheckBox *eventsl;
	TCheckBox *edeb;
	TCheckBox *tcpnodelay;
	TCheckBox *oobinline;
	TEdit *netspd;
	TLabel *pasvdone;
	TButton *Button2;
	TCheckBox *disk;
	TCheckBox *cache;
	TTrackBar *readaheadsize;
	TImage *dwnimg;
	TImage *rlimg;
	TImage *pasvconnecting;
	TEdit *heapsizel;
	TLabel *Label5;
	TCheckBox *dev;
	TButton *Button3;

	void __fastcall Button1Click(TObject *Sender);
	void __fastcall deblogDblClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall filelistClick(TObject *Sender);
	void __fastcall deblogMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X,
		int Y);
	void __fastcall ftptraffClick(TObject *Sender);
	void __fastcall eventslClick(TObject *Sender);
	void __fastcall tcpnodelayClick(TObject *Sender);
	void __fastcall oobinlineClick(TObject *Sender);
	void __fastcall anonClick(TObject *Sender);
	void __fastcall edebClick(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall cacheClick(TObject *Sender);
	void __fastcall readaheadsizeChange(TObject *Sender);
	void __fastcall devClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall deblogKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Button3Click(TObject *Sender);


private: // User declarations
public: // User declarations
	__fastcall TForm1(TComponent* Owner);
	void __fastcall IdleHandler(TObject *Sender, bool &Done);
};

// ---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;

LPVOID event(int type, LARGE_INTEGER numevent, LPVOID arg, LPVOID arg2, LPVOID arg3, LPVOID arg4,
	LPVOID arg5, LPVOID arg6, LPVOID arg7, LPVOID arg8);

typedef struct
{
	int type;
	LPVOID arg;
	LPVOID arg2;
	LPVOID arg3;
	LPVOID arg4;
	LPVOID arg5;
	LPVOID arg6;
	LPVOID arg7;
	LPVOID arg8;
	LARGE_INTEGER timearrive;
	DWORD ticks;
	unsigned long id;
	DWORD thread;
}FATEVENT;

#define eventslock()    _eventslock(__FUNC__,__LINE__)
#define eventsunlock()  _eventsunlock(__FUNC__,__LINE__)
#define eventstrylock() _eventstrylock(__FUNC__,__LINE__)
#define eventslocked() _eventslocked(__FUNC__,__LINE__)
bool _eventslock(char*func, int line);
bool _eventsunlock(char*func, int line);
bool _eventstrylock(char*func, int line);
int _eventslocked(char*func, int line);
unsigned long eventid(void);
// ---------------------------------------------------------------------------
#endif
