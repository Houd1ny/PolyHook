// PolyHook.cpp : Defines the entry point for the console application.
//
#include "PolyHook.hpp"
#define CATCH_CONFIG_MAIN
#include "CatchUnitTest.h"

decltype(&MessageBoxA) oMessageBoxA;
int WINAPI hkMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	int Result = oMessageBoxA(hWnd, "Hooked", lpCaption, uType);
	REQUIRE(strcmp("Message",lpText) == 0);
	return -10;
}

TEST_CASE("Hooks MessageBox", "[Detours]")
{
	std::shared_ptr<PLH::Detour> Detour_Ex(new PLH::Detour);
	REQUIRE(Detour_Ex->GetType() == PLH::HookType::Detour);

	Detour_Ex->SetupHook((BYTE*)&MessageBoxA,(BYTE*) &hkMessageBoxA); //can cast to byte* to
	REQUIRE( Detour_Ex->Hook() );
	oMessageBoxA = Detour_Ex->GetOriginal<decltype(&MessageBoxA)>();

	REQUIRE(MessageBoxA(NULL, "Message", "Sample", MB_OK) == -10); //The return value set by our handler
	Detour_Ex->UnHook();
	REQUIRE(MessageBoxA(NULL, "Message", "Sample", MB_OK) == IDOK);

	REQUIRE(Detour_Ex->GetLastError().GetSeverity() != PLH::RuntimeError::Severity::UnRecoverable);
	REQUIRE(Detour_Ex->GetLastError().GetSeverity() != PLH::RuntimeError::Severity::Critical);
}
