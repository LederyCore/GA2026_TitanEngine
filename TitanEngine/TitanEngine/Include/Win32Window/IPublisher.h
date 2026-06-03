#pragma once

namespace Platform
{
	class IObserver;

	template<typename T>
	class IPublisher
	{
	public:
		virtual ~IPublisher() = default;
		virtual void AddObserver(T* obs) = 0;
		virtual void RemoveObserver(T* obs) = 0;
	};
}