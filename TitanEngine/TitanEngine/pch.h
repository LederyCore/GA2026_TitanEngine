#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

// Windows
#include <windows.h>

// COM 및 DirectX 인터페이스
#include <wrl/client.h>              // ComPtr
#include <d3d11.h>                   // Direct3D 11
#include <d3dcompiler.h>             // Shader 컴파일
#include <dxgi1_6.h>                 // DXGI 1.6 (Windows 10 이상 최신 스왑체인)
#include <d2d1_3.h>                  // Direct2D 1.3 (ID2D1Factory4)
#include <d2d1_3helper.h>            // D2D1::Helper 클래스들
#include <dwrite_3.h>                // DirectWrite (최신 텍스트 엔진)
#include <wincodec.h>                // WIC (이미지 로딩)

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <crtdbg.h>


#define _CRTDBG_MAP_ALLOC
