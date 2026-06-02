#include "pch.h"
#include <iostream>
#include "Win32Window.h"
#include "Engine.h"

#pragma warning(disable : 28251)

using namespace Platform;
using namespace TitanEngine;

#ifdef _DEBUG
int main()
{
    Win32Window wnd;
    Engine engine;

    if (!engine.Initialize(wnd, L"DebugMode :: D2D Game", 1080, 720)) return -1;

    engine.Run();

    engine.Finalize();

    return 0;
}
#else
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    Win32Window wnd;
    Engine engine;

    if (!engine.Initialize(wnd, L"D2D Game", 1080, 720)) return -1;

    engine.Run();

    engine.Finalize();

    return 0;
}
#endif