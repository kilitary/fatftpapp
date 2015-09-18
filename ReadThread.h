//---------------------------------------------------------------------------

#ifndef ReadThreadH
#define ReadThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class ReadThread : public TThread
{
protected:
	void __fastcall Execute();
public:
	__fastcall ReadThread(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
