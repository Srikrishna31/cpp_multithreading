#include <iostream>
#include <tuple>
#include <utility>
#include <type_traits>

// IsLastOf

template <typename Type, typename... Args>
struct IsLastOf;

template <typename T>
struct IsLastOf<T> : std::integral_constant<bool, false> {};

template <typename T, typename Other>
struct IsLastOf<T, Other> : std::is_same<T, std::decay_t<Other>> {};

// Modified code
template <class T, class First, class... Args>
struct IsLastOf<T, First, Args...> : IsLastOf<T, Args...> {};

// Actual code
//template <class T, class First, class... Args>
//struct IsLastOf<T, First, Args...> {
//  using Type = typename IsLastOf<T, Args...>::Type;
//};


// GetLastIf

template <typename Type, Type Fallback>
struct GetLastIf {
  template <typename... Args, typename std::enable_if<!IsLastOf<Type, Args...>::value, int>::type = 1>
  static Type Get(Args&&... args) {
    return Fallback;
  }

  template <typename... Args, typename std::enable_if<IsLastOf<Type, Args...>::value, int>::type = 1>
  static Type Get(Args&&... args) {
    auto tuple = std::tuple<Args&&...>(std::forward<Args>(args)...);
    return std::get<sizeof...(Args) - 1>(tuple);
  }
};

// Test func

template <typename T, typename... Args>
void test(const char* name) {
    std::cout << name << " -> "
              << IsLastOf<T, Args...>::value << std::endl;
}

int main() {

    std::cout << "=== IsLastOf ===\n";

    test<int, char, double, float, int>("Last is int");        // 1
    test<int, char, double, float, double>("Last not int");    // 0
    test<double, int, float, char, double>("Last is double");  // 1
    test<char, int, float, double, char>("Last is char");      // 1

    std::cout << "\n=== GetLastIf ===\n";

    std::cout << GetLastIf<int, -1>::Get(1, 2, 4, 3, 12,34,21,45,21,566,636) << std::endl;       // 66
    std::cout << GetLastIf<int, -1>::Get(1, 2, 3.5) << std::endl;     // -1

    double x = 426;
    std::cout << GetLastIf<int, -1>::Get(5, 1,2,3,x) << std::endl;          // 426

    // Static assert

    // 1 arg
    static_assert(IsLastOf<int, int>::value, "FAIL");
    static_assert(!IsLastOf<int, double>::value, "FAIL");

    // 2 args
    static_assert(IsLastOf<int, char, int>::value, "FAIL");
    static_assert(!IsLastOf<int, int, double>::value, "FAIL");

    // 4 args
    static_assert(IsLastOf<int, char, double, float, int>::value, "FAIL");
    static_assert(!IsLastOf<int, char, double, float, double>::value, "FAIL");

    static_assert(IsLastOf<char, int, float, double, char>::value, "FAIL");
    static_assert(!IsLastOf<char, int, float, double, int>::value, "FAIL");

    return 0;
}