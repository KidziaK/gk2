#include "dxApplication.h"
#include "exceptions.h"
#include "CommCtrl.h"

using namespace std;
using namespace mini;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);
	auto exitCode = EXIT_FAILURE;
	try
	{
		DxApplication app(hInstance);
		exitCode = app.Run();
	}
	catch (Exception& e)
	{
		MessageBox(nullptr, e.getMessage().c_str(), L"B³¹d", MB_OK | MB_ICONERROR);
		exitCode = e.getExitCode();
	}
	catch (exception& e)
	{
		string s(e.what());
		MessageBox(nullptr, wstring(s.begin(), s.end()).c_str(), L"B³¹d", MB_OK | MB_ICONERROR);
	}
	catch (const char* str)
	{
		string s(str);
		MessageBox(nullptr, wstring(s.begin(), s.end()).c_str(), L"B³¹d", MB_OK | MB_ICONERROR);
	}
	catch (const wchar_t* str)
	{
		MessageBox(nullptr, str, L"B³¹d", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		MessageBox(nullptr, L"Nieznany B³¹d", L"B³¹d", MB_OK | MB_ICONERROR);
	}
	return exitCode;
}