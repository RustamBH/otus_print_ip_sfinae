#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <tuple>
#include <type_traits>


namespace mytuple {
	template <size_t I, typename... Types>
	void print_tuple_elem(const std::tuple<Types...>& tp) {
		std::cout << std::get<I>(tp);
		if constexpr (I < sizeof...(Types) - 1) {
			std::cout << '.';
		}
	}

	template <size_t I = 0, typename... Types>
	void print_tuple_(const std::tuple <Types...>& tp)
	{
		if constexpr (I < sizeof...(Types)) {
			print_tuple_elem<I>(tp);
			print_tuple_<I + 1>(tp);
		}
	}

	template <typename... Args>
	void print_tuple(const std::tuple<Args...>& tp)
	{
		print_tuple_(tp);
	}

	template <class T>
	constexpr bool is_same_type_pack()
	{
		return true;
	}

	template <class T, class U, class... Rest>
	constexpr bool is_same_type_pack()
	{
		return std::is_same_v<T, U> && is_same_type_pack<T, Rest ...>();
	}
}

namespace impl {
	template <class Type, class = std::enable_if_t<std::is_integral_v<Type>>>
	void print_ip(const Type& value, int)
	{
		static_assert(std::is_integral_v<Type>, "integral type required.");

		const size_t sz = sizeof(value);
		uint8_t bytes[sz];
		bool is_first = true;

		for (size_t i = 0, j = sz - 1; i < sz; ++i, --j) {
			if (!is_first)
				std::cout << ".";
			bytes[i] = (value >> j * 8) & 0xFF;
			std::cout << static_cast<size_t>(bytes[i]);
			is_first = false;
		}
		std::cout << std::endl;
	}

	template <class Type, class = std::enable_if_t<std::is_same_v <Type, std::string>>>
	void print_ip(const Type& String, long)
	{
		static_assert(std::is_same_v<Type, std::string>, "string type required.");
		std::cout << String << std::endl;
	}

	template <
		template <typename, typename> typename Container,
		typename Type,
		typename Allocator = std::allocator<Type>,
		typename = std::enable_if_t<
		std::is_same_v<Container<Type, Allocator>, std::vector<Type, Allocator>> ||
		std::is_same_v<Container<Type, Allocator>, std::list<Type, Allocator>>>>
		void print_ip(const Container<Type, Allocator>& container, long)
	{
		static_assert (std::is_same_v <Container <Type, Allocator>, std::vector <Type, Allocator>> ||
			std::is_same_v <Container <Type, Allocator>, std::list <Type, Allocator>>, "vector or list type required");

		for (auto iter = std::begin(container); iter != std::end(container); ++iter) {
			if (iter != std::begin(container))
				std::cout << ".";
			std::cout << *iter;
		}
		std::cout << std::endl;
	}

	template <
		template<typename ...Ts> typename Tuple,
		typename... Ts,
		class = std::enable_if_t <std::is_same_v <Tuple <Ts...>, std::tuple <Ts...>>>
	>
		void print_ip(const Tuple<Ts...>& tp, int)
	{
		static_assert(mytuple::is_same_type_pack <Ts...>(), "the same type of tuple required");
		mytuple::print_tuple(tp);
	}

} // impl


template <class T>
void print_ip(const T& value)
{
	impl::print_ip(value, 42);
}


int main()
{
	print_ip(int8_t{ -1 }); // 255
	print_ip(int16_t{ 0 }); // 0.0
	print_ip(int32_t{ 2130706433 }); // 127.0.0.1
	print_ip(int64_t{ 8875824491850138409 });// 123.45.67.89.101.112.131.41
	print_ip(std::string{ "Hello, World!" }); // Hello, World!
	print_ip(std::vector<int>{100, 200, 300, 400}); // 100.200.300.400
	print_ip(std::list<short>{400, 300, 200, 100}); // 400.300.200.100
	print_ip(std::make_tuple(123, 456, 789, 0)); // 123.456.789.0

	return 0;
}