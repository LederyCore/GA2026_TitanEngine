#include "pch.h"
#include <iostream>
#include "Win32Window/Win32Window.h"
#include "Engine.h"
#pragma warning(disable : 28251)

using namespace Platform;
using namespace TitanEngine;

#ifdef _DEBUG
int main()
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) return -1;
    {
        Win32Window wnd;
        Engine engine;

        if (engine.Initialize(wnd, L"DebugMode :: D2D Game", 1080, 720))
        {
            engine.Run();
            engine.Finalize();
        }
    }

    CoUninitialize();
    return 0;
}
#else
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) return -1;
    {
        Win32Window wnd;
        Engine engine;

        if (engine.Initialize(wnd, L"D2D Game", 1080, 720))
        {
            engine.Run();
            engine.Finalize();
        }
    }

    CoUninitialize();
    return 0;
}
#endif
