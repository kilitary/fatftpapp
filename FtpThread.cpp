// ---------------------------------------------------------------------------
#pragma link "madExcept"
#pragma link "madLinkDisAsm"
#include <vcl.h>
#pragma hdrstop

#include "FtpThread.h"
#include "../fatftp/ff.h"
#include "main.h"
#pragma package(smart_init)

extern bool stopftpthread;
bool mounted = false;

// ---------------------------------------------------------------------------

// Important: Methods and properties of objects in VCL can only be
// used in a method called using Synchronize, for example:
//
// Synchronize(&UpdateCaption);
//
// where UpdateCaption could look like:
//
// void __fastcall FtpThread::UpdateCaption()
// {
// Form1->Caption = "Updated in a thread";
// }
// ---------------------------------------------------------------------------

__fastcall FtpThread::FtpThread(bool CreateSuspended) : TThread(CreateSuspended)
{

}

// ---------------------------------------------------------------------------
void __fastcall FtpThread::Execute()
{
	 //Priority = tpHighest;
	deb("ftpthread %4X", GetCurrentThreadId());
	// Synchronize(Do);
	Do();
}

void __fastcall FtpThread::Do(void)
{
	NameThreadForDebugging("FtpThread");
	// ---- Place thread code here ----

	if (!mounted)
	{
		Form1->dwnimg->Visible = false;
		Form1->rlimg->Visible = false;
		Form1->pasvconnecting->Visible = false;
		ff_SocketBuffers(13111072);
		ff_SocketNoDelay(Form1->tcpnodelay->Checked);
		// ff_ReadAheadMinCache(384000);
		ff_SocketOOBInline(Form1->oobinline->Checked);

		mounted = true;

		char ffhost[255];
		deunicode(Form1->host->Text.c_str(), ffhost, sizeof(ffhost));
		Form1->filelist->Clear();
		Form1->filelist->AddItem("ff_ConnectFTP", 0);

		// deb("execing ff_connectftp");

		if (!ff_ConnectFTP(ffhost, 21))
		{
			mounted = false;
			return;
		}

		if (stopftpthread)
			return;

		char user[111], pass[111];
		deunicode(Form1->user->Text.c_str(), user, sizeof(user));
		deunicode(Form1->pass->Text.c_str(), pass, sizeof(pass));

		deb("user %s pass %s",user,pass);
		// deb("seting butns, add log");
		Form1->filelist->AddItem("ff_AuthFTP", 0);

		Form1->Button1->Caption = "���������";

		// deb("execing ff_authftp()");
		if (!ff_AuthFTP(user, pass))
		{
			mounted = false;
			Form1->Button1->Caption = "����������";
			return;
		}

		if (stopftpthread)
			return;

		// deb("add log ff_ScanFTP");
		Form1->filelist->AddItem("ff_ScanFTP", 0);
		// deb("execing ff_ScanFTP()");
		ff_ScanFTP(0, 50000, 50000);

		if (stopftpthread)
			return;
		// deb("add log ff_MountDisk");
		Form1->filelist->AddItem("ff_MountDisk", 0);
		// deb("execing ff_MountDisk()");
		ff_MountDisk('y');

		if (stopftpthread)
			return;

		// deb("add log scan");
		Form1->filelist->AddItem("scan", 0);

		if (stopftpthread)
			return;
		Form1->filelist->Clear();
		WIN32_FIND_DATA wfd;
		HANDLE hf;
		hf = FindFirstFile("y:\\*", &wfd);
		Form1->filelist->AddItem(wfd.cFileName, 0);
		while (FindNextFile(hf, &wfd))
		{

			Form1->filelist->AddItem(wfd.cFileName, 0);
		}
		char s[111];
		sprintf(s, "ftp://%s:%s@%s:%d [connected]", user, pass, ffhost, 21);
		Form1->Caption = s;
		// Application->ProcessMessages();
		// Form1->filelist->Clear();

		deb("ready");
	}
	else
	{
		mounted = false;
		Form1->Button1->Caption = "����������";
		ff_Clear();

		Form1->timeout->Text = "0";
		Form1->netspd->Text = "0";
	}
}
// ---------------------------------------------------------------------------
