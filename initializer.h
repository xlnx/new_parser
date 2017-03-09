#pragma once
#include <vector>
#include <type_traits>
#include <initializer_list>
struct element			// base of element literal type
{
	long long value;
	constexpr element(long long v): value(v) {}
};
struct init_element
{
};

template <typename T, template <typename...> class F>
struct is_instance_of: std::integral_constant<bool, false>
{};
template <typename... Args, template <typename...> class F>
struct is_instance_of<F<Args...>, F>: std::integral_constant<bool, true>
{}; 

template <typename T, typename = typename
	std::enable_if<std::is_class<T>::value>::type>
struct initializer: std::vector<T>
{
	using value_type = T;
	initializer(const T& elem):
		std::vector<T>({elem}) {}
	initializer(const initializer<T>& list):
		std::vector<T>(list) {}
	initializer(const std::initializer_list<T>& list):
		std::vector<T>(list) {}
	template <typename U, typename = typename
			std::enable_if<
				std::is_constructible<T, const U&>::value
			>::type>
		initializer(const U& elem):
			std::vector<T>({T(elem)}) {}
	template <typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				std::is_constructible<T, const U&>::value
			>::type>
		initializer(const initializer<U>& list)
		{ for (auto& elem: list) std::vector<T>::push_back(elem); }
	template <typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				std::is_constructible<T, const U&>::value
			>::type>
		initializer(const std::initializer_list<U>& list)
		{ for (auto& elem: list) std::vector<T>::push_back(elem); }
};

template <typename T, typename = typename
	std::enable_if<std::is_base_of<element, T>::value>::type>
using element_list = initializer<T>;
template <typename T, typename = typename
	std::enable_if<std::is_base_of<init_element, T>::value>::type>
using init_element_list = initializer<T>;


// element concat operator
template <typename T>
inline element_list<T> operator + (const T& e1, const T& e2)
{
	return element_list<T>({e1, e2});
}
template <typename T, typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				!is_instance_of<T, element_list>::value &&
				!is_instance_of<T, std::initializer_list>::value &&
				std::is_constructible<T, const U&>::value
			>::type>
inline element_list<T> operator + (const U& e1, const T& e2)
{
	return element_list<T>({T(e1), e2});
}
template <typename T, typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				!is_instance_of<T, element_list>::value &&
				!is_instance_of<T, std::initializer_list>::value &&
				std::is_constructible<T, const U&>::value
			>::type>
inline element_list<T> operator + (const T& e1, const U& e2)
{
	return element_list<T>({e1, T(e2)});
}
template <typename T>
inline const element_list<T>& operator + (const element_list<T>& list, const T& e)
{
	const_cast<element_list<T>&>(list).push_back(e);
	return list;
}
template <typename T, typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				std::is_constructible<T, const U&>::value
			>::type>
inline const element_list<T>& operator + (const element_list<T>& list, const U& e)
{
	const_cast<element_list<T>&>(list).push_back(T(e));
	return list;
}
template <typename T, typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				std::is_constructible<T, const U&>::value
			>::type>
inline element_list<T> operator + (const element_list<U>& list, const T& e)
{
	auto l = element_list<T>(list);
	l.push_back(e);
	return l;
}

// concat operator
template <typename T>
inline initializer<T> operator | (const T& e1, const T& e2)
{
	return initializer<T>({e1, e2});
}
template <typename T, typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				!is_instance_of<T, initializer>::value &&
				!is_instance_of<T, std::initializer_list>::value &&
				std::is_constructible<T, const U&>::value
			>::type>
inline initializer<T> operator | (const U& e1, const T& e2)
{
	return initializer<T>({T(e1), e2});
}
template <typename T, typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				!is_instance_of<T, initializer>::value &&
				!is_instance_of<T, std::initializer_list>::value &&
				std::is_constructible<T, const U&>::value
			>::type>
inline initializer<T> operator | (const T& e1, const U& e2)
{
	return initializer<T>({e1, T(e2)});
}
template <typename T>
inline const initializer<T>& operator | (const initializer<T>& list, const T& e)
{
	const_cast<initializer<T>&>(list).push_back(e);
	return list;
}
template <typename T, typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				std::is_constructible<T, const U&>::value
			>::type>
inline const initializer<T>& operator | (const initializer<T>& list, const U& e)
{
	const_cast<initializer<T>&>(list).push_back(T(e));
	return list;
}
template <typename T, typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				std::is_constructible<T, const U&>::value
			>::type>
inline initializer<T> operator | (const initializer<U>& list, const T& e)
{
	auto l = initializer<T>(list);
	l.push_back(e);
	return l;
}
