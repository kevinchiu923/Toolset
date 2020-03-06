#pragma once

namespace Toolset
{
	template<typename T>
	class Singleton
	{
	public:
        static T* getInstance() {
			static T instance{ token{} };
			return &instance;
		}

		Singleton(const Singleton&) = delete;
		Singleton& operator= (const Singleton) = delete;

	protected:
		struct token {};
		Singleton() {}
	};
}
