#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

// 윈도우 관련
#include <windows.h>
#include <memory>
#include <stdio.h>
#include <iostream>

#include <cassert>
#include <exception>
#include <crtdbg.h>

#include <array>
#include <list>
#include <vector>
#include <string>


#pragma region D2D 관련 =========================
// COM 및 DirectX 인터페이스
#include <wrl.h>              // ComPtr
#include <d3d11.h>                   // Direct3D 11
#include <d3dcompiler.h>             // Shader 컴파일
#include <dxgi1_6.h>                 // DXGI 1.6 (Windows 10 이상 최신 스왑체인)
#include <d2d1_3.h>                  // Direct2D 1.3 (ID2D1Factory4)
#include <d2d1_3helper.h>            // D2D1::Helper 클래스들
#include <dwrite_3.h>                // DirectWrite (최신 텍스트 엔진)
#include <wincodec.h>                // WIC (이미지 로딩)

// Direct3D & DXGI
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// Direct2D & DirectWrite
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

// WIC (Windows Imaging Component)
#pragma comment(lib, "windowscodecs.lib")
using Microsoft::WRL::ComPtr;

#pragma endregion D2D 관련 ==========================

// 자료구조 알고리즘 관려
#include <array>