//---------------------------------------------------------------------------

#ifndef FtpThreadH
#define FtpThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class FtpThread : public TThread
{
protected:
	void __fastcall Execute();
public:
	__fastcall FtpThread(bool CreateSuspended);
	void __fastcall Do(void);
};
//---------------------------------------------------------------------------
#endif
