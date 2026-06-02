#include "pch.h"
#include <iostream>
#include "Win32Window.h"

#pragma warning(disable : 28251)

using namespace Platform;

#ifdef _DEBUG
int main()
{
    Win32Window wnd;
    HWND hWnd = wnd.Create(L"sdsad", 1280, 720);

    while (true)
    {

    }
}
#else
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    Win32Window wnd;
    HWND hWnd = wnd.Create(L"sdsad", 1280, 720);

    while (true)
    {

    }
}
#endif