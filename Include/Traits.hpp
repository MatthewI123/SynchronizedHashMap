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
		static constexpr auto func = [](auto value) constexpr { return __bswap_constant_16(value); };
	};

	template<typename T>
	struct hton<T, std::enable_if_t<std::is_arithmetic_v<T> && sizeof(T) == 4>>
	{
		static constexpr auto func = [](auto value) constexpr { return __bswap_constant_32(value); };
	};

	template<typename T>
	struct hton<T, std::enable_if_t<std::is_arithmetic_v<T> && sizeof(T) == 8>>
	{
		static constexpr auto func = [](auto value) constexpr { return __bswap_constant_64(value); };
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
		static constexpr auto func = [](auto value) constexpr { return __bswap_constant_16(value); };
	};

	template<typename T>
	struct ntoh<T, std::enable_if_t<std::is_arithmetic_v<T> && sizeof(T) == 4>>
	{
		static constexpr auto func = [](auto value) constexpr { return __bswap_constant_32(value); };
	};

	template<typename T>
	struct ntoh<T, std::enable_if_t<std::is_arithmetic_v<T> && sizeof(T) == 8>>
	{
		static constexpr auto func = [](auto value) constexpr { return __bswap_constant_64(value); };
	};

	template<typename T>
	struct ntoh<T, std::enable_if_t<std::is_enum_v<T>>>
	{
		static constexpr auto func = [](auto value) constexpr {
			using U = std::underlying_type_t<T>;
			return static_cast<T>(ntoh<U>::func(static_cast<U>(value)));
		};
	};
}

namespace Arguments::Traits
{
	template<typename T, typename = void>
	struct argument_type : std::false_type { };

	template<typename T>
	struct argument_type<T, std::enable_if_t<std::is_same_v<T, bool>>> : std::true_type
	{
		static constexpr auto func = [](const char* str)
		{
			std::string_view view(str);

			if (view == "true" || view == "1")
				return true;
			else if (view == "false" || view == "0")
				return false;
			else
				throw std::runtime_error("bad bool value");
		};
	};


	template<typename T>
	struct argument_type<T, std::enable_if_t<std::is_integral_v<T>>> : std::true_type
	{
		static constexpr auto func = [](const char* str)
		{
			constexpr auto checkLimit = [](auto value)
			{
				using limit = std::numeric_limits<T>;

				if (value >= limit::min() && value <= limit::max())
					return static_cast<T>(value);
				else
					throw std::runtime_error("value too large");
			};

			if constexpr (std::is_unsigned_v<T>)
				return checkLimit(std::stoull(str));
			else
				return checkLimit(std::stoll(str));
		};
	};

	template<typename T>
	struct argument_type<T, std::enable_if_t<std::is_floating_point_v<T>>> : std::true_type
	{
		static constexpr auto func = [](const char* str)
		{
			if constexpr (std::is_same_v<T, float>)
				return std::stof(str);
			else if constexpr(std::is_same_v<T, double>)
				return std::stod(str);
			else
				return std::stold(str);
		};
	};

	template<typename T>
	struct argument_type<T, std::enable_if_t<std::is_same_v<T, std::string>>> : std::true_type
	{
		static constexpr auto func = [](const char* str)
		{
			return std::string(str);
		};
	};

	template<typename T>
	struct argument_type<std::optional<T>, std::enable_if_t<argument_type<T>::value>> : std::true_type
	{
		static constexpr auto func = [](const char* str)
		{
			return argument_type<T>::func(str);
		};
	};
}
