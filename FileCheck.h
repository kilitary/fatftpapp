//---------------------------------------------------------------------------

#ifndef FileCheckH
#define FileCheckH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class FileCheck : public TThread
{
protected:
	void __fastcall Execute();
public:
	__fastcall FileCheck(bool CreateSuspended);
	void __fastcall Do(void);
};
//---------------------------------------------------------------------------
#endif
