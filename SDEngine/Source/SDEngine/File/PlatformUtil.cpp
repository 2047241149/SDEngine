#include "PlatformUtil.h"
#include <GameWindow.h>
#include <commdlg.h>
#include "SDEngine/Common/CommonFunction.h"

string FileDialog::OpenFile(LPCWSTR fliterName)
{
	OPENFILENAME ofn;
	WCHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GWindowHwnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = fliterName;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetOpenFileName(&ofn) == TRUE)
	{
		return Wstr2Str(ofn.lpstrFile);
	}

	return string();
}

string FileDialog::SaveFile(LPCWSTR fliterName)
{
	OPENFILENAME ofn;
	WCHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GWindowHwnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = fliterName;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetSaveFileName(&ofn) == TRUE)
	{
		return Wstr2Str(ofn.lpstrFile);
	}

	return string();
}