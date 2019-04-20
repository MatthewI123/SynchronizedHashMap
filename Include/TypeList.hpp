#pragma once

#include <tuple>
#include <utility>

template<typename... Type>
struct TypeList;

namespace Implementation::TypeList
{
	template<std::size_t Index, typename... Type>
	using Get = std::tuple_element_t<Index, std::tuple<Type...>>;

	template<typename, typename...>
	struct PopBackHelper;

	template<typename... Type, size_t... Index>
	struct PopBackHelper<std::index_sequence<Index...>, Type...>
	{
		using type = ::TypeList<Get<Index, Type...>...>;
	};

	template<typename Head, typename... Tail>
	struct PopBack
	{
		using type = typename PopBackHelper<std::make_index_sequence<sizeof...(Tail)>, Head, Tail...>::type;
	};

	template<typename... Type>
	struct Join
	{
		using type = ::TypeList<Type...>;
	};

	template<typename... Type1, typename... Type2>
	struct Join<::TypeList<Type1...>, ::TypeList<Type2...>>
	{
		using type = typename Join<Type1..., Type2...>::type;
	};

	template<std::size_t Index, std::size_t Count, typename... Type>
	struct Subsequence
	{
		using type = typename Join<
			::TypeList<Get<Index, Type...>>,
			typename Subsequence<Index + 1, Count - 1, Type...>::type
		>::type;
	};

	template<std::size_t Index, typename Head, typename... Tail>
	struct Subsequence<Index, 0, Head, Tail...> // shift left
	{
		using type = ::TypeList<>;
	};
}

template<typename...>
struct TypeList
{
	static constexpr std::size_t Size = 0;

	using Tuple = std::tuple<>;

	template<typename Type>
	using PushFront = TypeList<Type>;

	template<typename Type>
	using PushBack = TypeList<Type>;
};

template<typename Head, typename... Tail>
struct TypeList<Head, Tail...>
{
	static constexpr std::size_t Size = 1 + sizeof...(Tail);

	template<size_t Index>
	using Type = Implementation::TypeList::Get<Index, Head, Tail...>;

	using Tuple = std::tuple<Head, Tail...>;

	template<typename Type>
	using PushFront = TypeList<Type, Head, Tail...>;

	template<typename Type>
	using PushBack = TypeList<Head, Tail..., Type>;

	using PopFront = TypeList<Tail...>;
	using PopBack = typename Implementation::TypeList::template PopBack<Head, Tail...>::type;

	template<std::size_t Index, std::size_t Count>
	using Subsequence = typename Implementation::TypeList::template Subsequence<Index, Count, Head, Tail...>::type;
};
