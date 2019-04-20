#pragma once

#include <type_traits>

extern "C"
{
#include <endian.h>
}

namespace Network::Traits
{
	template<typename... T>
	struct dependent_false : std::false_type { };
}

namespace Network::Traits
{
	template<typename T, typename = void>
	struct hton { };

	template<typename T>
	struct hton<T, std::enable_if_t<std::is_arithmetic_v<T> && sizeof(T) == 1>>
	{
		static constexpr auto func = [](auto value) constexpr { return value; };
	};

	template<typename T>
	struct hton<T, std::enable_if_t<std::is_arithmetic_v<T> && sizeof(T) == 2>>
	{
		static constexpr auto func = [](auto value) constexpr { return htobe16(value); };
	};

	template<typename T>
	struct hton<T, std::enable_if_t<std::is_arithmetic_v<T> && sizeof(T) == 4>>
	{
		static constexpr auto func = [](auto value) constexpr { return htobe32(value); };
	};

	template<typename T>
	struct hton<T, std::enable_if_t<std::is_arithmetic_v<T> && sizeof(T) == 8>>
	{
		static constexpr auto func = [](auto value) constexpr { return htobe64(value); };
	};

	template<typename T>
	struct hton<T, std::enable_if_t<std::is_enum_v<T>>>
	{
		static constexpr auto func = [](auto value) constexpr {
			using U = std::underlying_type_t<T>;
			return hton<U>::func(static_cast<U>(value));
		};
	};
}

namespace Network::Traits
{
	template<typename T, typename = void>
	struct ntoh { };

	template<typename T>
	struct ntoh<T, std::enable_if_t<std::is_arithmetic_v<T> && sizeof(T) == 1>>
	{
		static constexpr auto func = [](auto value) { return value; };
	};

	template<typename T>
	struct ntoh<T, std::enable_if_t<std::is_arithmetic_v<T> && sizeof(T) == 2>>
	{
		static constexpr auto func = [](auto value) constexpr { return be16toh(value); };
	};

	template<typename T>
	struct ntoh<T, std::enable_if_t<std::is_arithmetic_v<T> && sizeof(T) == 4>>
	{
		static constexpr auto func = [](auto value) constexpr { return be32toh(value); };
	};

	template<typename T>
	struct ntoh<T, std::enable_if_t<std::is_arithmetic_v<T> && sizeof(T) == 8>>
	{
		static constexpr auto func = [](auto value) constexpr { return be64toh(value); };
	};

	template<typename T>
	struct ntoh<T, std::enable_if_t<std::is_enum_v<T>>>
	{
		static constexpr auto func = [](auto value) constexpr {
			using U = std::underlying_type_t<T>;
			return ntoh<U>::func(static_cast<U>(value));
		};
	};
}
