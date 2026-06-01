#pragma once
#include "IWindow.h"
#include <memory>

namespace Platform
{
	class WindowFactory
	{
	public:
		static std::unique_ptr<IWindow> Create(const std::wstring& windowName, int width, int height);
	};
}