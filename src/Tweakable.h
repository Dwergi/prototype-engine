#pragma once

namespace dd
{
	template <typename T>
	struct Tweakable
	{
		Tweakable(T& value, std::string name);

		void Draw();

	private:

		T& m_value;
		std::string m_name;
	};

	template <typename T>
	Tweakable<T>::Tweakable(T& value, std::string name) :
		m_value(value),
		m_name(name)
	{
		DD_TODO("Flesh this out. Should be called from IDebugPanel draw.");
	}
}