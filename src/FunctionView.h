// Taken from here: https://github.com/SuperV1234/vittorioromeo.info/blob/master/extra/passing_functions_to_functions/function_view.hpp

#pragma once

#include <type_traits>
#include <functional>

namespace dd
{
	template <typename...>
	using void_t = void;

	template <class T, class R = void, class = void>
	struct is_callable : std::false_type
	{
	};

	template <class T>
	struct is_callable<T, void, void_t<std::invoke_result_t<T>>> : std::true_type
	{
	};

	template <class T, class R>
	struct is_callable<T, R, void_t<std::invoke_result_t<T>>>
		: std::is_convertible<std::invoke_result_t<T>, R>
	{
	};

	template <typename TSignature>
	class FunctionView;

	template <typename TReturn, typename... TArgs>
	class FunctionView<TReturn( TArgs... )> final
	{
	private:
		void* m_ptr { nullptr };
		TReturn( *m_erasedFn )(void*, TArgs...) { nullptr };

	public:
		template <typename T, typename = std::enable_if_t<
			is_callable<T&(TArgs...)>::value &&
			!std::is_same_v<std::decay_t<T>, FunctionView>>>
			FunctionView( T&& x ) noexcept
			: m_ptr { (void*) std::addressof( x ) }
		{
			m_erasedFn = []( void* ptr, TArgs... xs ) -> TReturn
			{
				return (*reinterpret_cast<std::add_pointer_t<T>>(ptr))(
					std::forward<TArgs>( xs )...);
			};
		}

		FunctionView()
		{
		}

		template <typename T, typename = std::enable_if_t<
			is_callable<T&(TArgs...)>::value &&
			!std::is_same_v<std::decay_t<T>, FunctionView>>>
			FunctionView& operator=( T&& x ) noexcept
		{
			m_ptr = (void*) std::addressof( x );
			m_erasedFn = []( void* ptr, TArgs... xs ) -> TReturn
			{
				return (*reinterpret_cast<std::add_pointer_t<T>>(ptr))(
					std::forward<TArgs>( xs )...);
			};
		}

		decltype(auto) operator()( TArgs... xs ) const
			noexcept(noexcept(m_erasedFn( m_ptr, std::forward<TArgs>( xs )... )))
		{
			return m_erasedFn( m_ptr, std::forward<TArgs>( xs )... );
		}

		bool valid() const
		{
			return m_ptr != nullptr;
		}
	};
}