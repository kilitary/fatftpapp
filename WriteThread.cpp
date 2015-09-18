// ---------------------------------------------------------------------------
#include "FileCheck.h"
#include "main.h"
#include "functions.h"

#include <stdio.h>
#include <vcl.h>
#pragma hdrstop

#include "WriteThread.h"
#pragma package(smart_init)
// ---------------------------------------------------------------------------

// Important: Methods and properties of objects in VCL can only be
// used in a method called using Synchronize, for example:
//
// Synchronize(&UpdateCaption);
//
// where UpdateCaption could look like:
//
// void __fastcall WriteThread::UpdateCaption()
// {
// Form1->Caption = "Updated in a thread";
// }
// ---------------------------------------------------------------------------

__fastcall WriteThread::WriteThread(bool CreateSuspended) : TThread(CreateSuspended)
{

}

// ---------------------------------------------------------------------------
void __fastcall WriteThread::Execute()
{
	NameThreadForDebugging("WriteThread");

	srand(time(NULL) % 32768);
	unsigned long size = rand()*(rand()%7);
	// deb("write size: %d", size);

	HANDLE h;
	char fn[255] = "";
	char alp[] = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890";
	memset(fn, 0, sizeof(fn));
	strcpy(fn, "y:\\");

	for (int i = 3; i < 10; i++)
		fn[i] = alp[rand() % sizeof(alp)];

	strcat(fn, ".txt");
	deb("fn %s %lu", fn, size);
	h = CreateFile(fn, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	// |  |FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH
	if (h == INVALID_HANDLE_VALUE)
	{
		deb("createfile %s", fmterr());
		return;
	}
	// deb("h %x", h);

	char *data;
	data = (char*)HeapAlloc(GetProcessHeap(), 0, (unsigned long)size);
	for (int i = 0; i < size; i++)
		data[i] = rand() % 255;
	DWORD dwWrite = 0;
	// deb("bufsize=%lu",HeapSize(GetProcessHeap(), 0, data));
	// deb("WriteFile(%x, %p, %lu, %p, 0)",h,data,(unsigned long)size,&dwWrite);

	if (!WriteFile(h, data, (unsigned long)size, &dwWrite, 0))
		deb("write %s", fmterr());
	HeapFree(GetProcessHeap(), 0, data);
	CloseHandle(h);
	// deb("done write");
}
// ---------------------------------------------------------------------------
