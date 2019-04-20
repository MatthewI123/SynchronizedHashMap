#pragma once

#include <array>
#include <limits>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include "TypeList.hpp"

extern "C"
{
#include <unistd.h>
}

namespace Implementation::Arguments
{
	template<typename T, typename = void>
	struct AcceptableType : std::false_type { };

	template<typename T>
	struct AcceptableType<T, std::enable_if_t<std::is_same_v<T, bool>>> : std::true_type
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
	struct AcceptableType<T, std::enable_if_t<std::is_integral_v<T>>> : std::true_type
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
	struct AcceptableType<T, std::enable_if_t<std::is_floating_point_v<T>>> : std::true_type
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
	struct AcceptableType<T, std::enable_if_t<std::is_same_v<T, std::string>>> : std::true_type
	{
		static constexpr auto func = [](const char* str)
		{
			return std::string(str);
		};
	};
}


namespace Implementation::Arguments
{
	template<typename OptionDescriptor, std::size_t Index, typename OptionValues>
	void HandleOption(char opt, const std::array<char, OptionDescriptor::Size>& options, OptionValues& values)
	{
		if (opt == options[Index]) {
			using type = typename OptionDescriptor::template Type<Index>;

			if constexpr (!std::is_same_v<type, bool>) {
				using trait = AcceptableType<type>;
				static_assert(trait::value, "unaccepted argument type");

				std::get<Index>(values) = trait::func(optarg);
			} else {
				std::get<Index>(values) = true;
			}
		}
	}

	template<typename OptionDescriptor, typename OptionValues, std::size_t... OptionIndex>
	void HandleOption(char opt, const std::array<char, OptionDescriptor::Size>& options, OptionValues& values, std::index_sequence<OptionIndex...>)
	{

		(... , HandleOption<OptionDescriptor, OptionIndex>(opt, options, values));
	}

	template<typename OptionDescriptor, std::size_t Index>
	void AppendOption(const std::array<char, OptionDescriptor::Size>& options, std::string& optstring)
	{
		optstring += options[Index];

		if constexpr (!std::is_same_v<typename OptionDescriptor::template Type<Index>, bool>)
			optstring += ':';
	}

	template<typename OptionDescriptor, std::size_t... OptionIndex>
	void AppendOptions(const std::array<char, OptionDescriptor::Size>& options, std::string& optstring, std::index_sequence<OptionIndex...>)
	{
		(... , AppendOption<OptionDescriptor, OptionIndex>(options, optstring));
	}
}


namespace Implementation::Arguments
{
	template<typename ArgumentDescriptor, std::size_t Index, typename ArgumentValues>
	void HandleArgument(ArgumentValues& arguments, int count, char* argv[])
	{
		if (Index < count) {
			using type = typename ArgumentDescriptor::template Type<Index>;
			using trait = AcceptableType<type>;

			std::get<Index>(arguments) = trait::func(argv[optind + Index]);
		}
	}

	template<typename ArgumentDescriptor, typename ArgumentValues, std::size_t... ArgumentIndex>
	void HandleArguments(ArgumentValues& arguments, int count, char* argv[], std::index_sequence<ArgumentIndex...>)
	{
		(... , HandleArgument<ArgumentDescriptor, ArgumentIndex>(arguments, count, argv));
	}
}

namespace Implementation::Arguments
{
	template<typename OptionDescriptor, typename ArgumentDescriptor, typename OptionValues, typename ArgumentValues>
	void ParseArguments(OptionValues& optionValues, ArgumentValues& argumentValues, std::array<char, OptionDescriptor::Size> options, int argc, char* argv[])
	{
		std::string optstring;
		optstring.reserve(OptionDescriptor::Size*2 + 1); // worst case
		optstring += ':';

		AppendOptions<OptionDescriptor>(options, optstring, std::make_index_sequence<OptionDescriptor::Size>());

		{
			int opt;

			// handle options
			while ((opt = getopt(argc, argv, optstring.data())) != -1) {
				if (opt == ':')
					throw std::runtime_error(std::string("option '") + static_cast<char>(optopt) + "' missing");
				else if (opt != '?')
					HandleOption<OptionDescriptor>(static_cast<char>(opt), options, optionValues, std::make_index_sequence<OptionDescriptor::Size>());
			}

			// handle arguments
			HandleArguments<ArgumentDescriptor>(argumentValues, argc - optind, argv, std::make_index_sequence<ArgumentDescriptor::Size>());

		}
	}
}

namespace Implementation::Arguments
{
	template<std::size_t Index, std::size_t Count, typename Tuple>
	constexpr auto TupleSubsequence(Tuple& tuple)
	{
		if constexpr (Count > 0) {
			return std::tuple_cat(
				std::tuple(std::move(std::get<Index>(tuple))),
				TupleSubsequence<Index + 1, Count - 1>(tuple));
		} else {
			return std::tuple();
		}
	}
}

namespace Implementation::Arguments
{
	/** Checks for duplicates within an array.
	 * Not using an unordered_map for a linear solution instead of quadratic because
	 *  OptionCount will typically be small and std::array is constexpr-able.
	 */
	template<std::size_t OptionCount>
	constexpr void CheckDuplicates(const std::array<char, OptionCount>& array)
	{
		for (std::size_t i = 0; i < OptionCount; i++) {
			for (std::size_t j = i + 1; j < OptionCount; j++) {
				if (array[i] == array[j])
					throw std::runtime_error("options cannot contain duplicates");
			}
		}
	}
}

template<std::size_t OptionCount, typename... Type>
constexpr auto ParseArguments(std::array<char, OptionCount> options, std::tuple<Type...> defaults, int argc, char* argv[])
{
	static_assert(sizeof...(Type) > 0, "empty argument list");
	static_assert(sizeof...(Type) >= OptionCount, "option count exceeds argument count");

	Implementation::Arguments::CheckDuplicates(options);

	using optionList = typename TypeList<Type...>::template Subsequence<0, OptionCount>;
	using argumentList = typename TypeList<Type...>::template Subsequence<OptionCount, sizeof...(Type) - OptionCount>;

	auto optionValues = Implementation::Arguments::TupleSubsequence<0, OptionCount>(defaults);
	auto argumentValues = Implementation::Arguments::TupleSubsequence<OptionCount, sizeof...(Type) - OptionCount>(defaults);

	Implementation::Arguments::ParseArguments<optionList, argumentList>(optionValues, argumentValues, options, argc, argv);

	return std::tuple(std::move(optionValues), std::move(argumentValues));
}
