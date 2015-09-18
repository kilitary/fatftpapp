//---------------------------------------------------------------------------
#include <vcl.h>
#include <tchar.h>


					  HINSTANCE hi;

//---------------------------------------------------------------------------
USEFORM("main.cpp", Form1);
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE hii, HINSTANCE, LPTSTR, int)
{
     hi=hii;
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TForm1), &Form1);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
