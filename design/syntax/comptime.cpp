
// Core Keywords & Specifiers


// Keyword	Standard	Target	Compile-Time Guarantee
// constexpr	C++11	Variables, functions, constructors	Flexible. Evaluates at compile time if inputs are known, or falls back to runtime.
// consteval	C++20	Functions only	Mandatory. Must evaluate at compile time; triggers a compiler error otherwise.
// constinit	C++20	Static/thread-local variables	Mandatory initialization. Forces compile-time initialization without making the variable const.


// • constexpr (The Workhorse). Marking a function constexpr allows it to run during compilation when given constant inputs, while still working as a normal function for runtime data. Since C++14 and C++20, constexpr supports loops, local variables, lambdas, and even standard containers like std::vector and std::string (as long as memory is freed before compile-time evaluation ends).
// • consteval (Immediate Functions). Introduced in C++20, consteval enforces a strict compile-time contract. If you pass a runtime variable into a consteval function, the build fails immediately.
// • constinit (Safe Globals). Solves the "static initialization order fiasco" by guaranteeing a static variable is initialized at compile time, while still allowing you to mutate it later at runtime.

#include <array>
#include <concepts>
#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

// ============================================================================
// 1. CONSTEXPR & CONSTEVAL FUNCTIONS (C++11 - C++23)
// ============================================================================

// C++11/14/20: Flexible. Runs at compile time if inputs are constant, else runtime.
constexpr int compute(int x) {
    int sum = 0;
    for (int i = 1; i <= x; ++i) {
        sum += i;
    }
    return sum;
}

// C++20: Immediate function. MUST execute at compile time; compiler error otherwise.
consteval uint32_t fnv1a_hash(const char* str) {
    uint32_t hash = 2166136261u;
    while (*str) {
        hash ^= static_cast<uint8_t>(*str++);
        hash *= 16777619u;
    }
    return hash;
}

// C++20/23: Dynamic memory allocation inside constexpr (must be freed before returning)
constexpr std::size_t count_even_numbers(int max) {
    std::vector<int> vec; // Dynamic allocation at compile time!
    for (int i = 0; i < max; ++i) {
        if (i % 2 == 0) vec.push_back(i);
    }
    return vec.size(); // 'vec' is destroyed here, freeing memory before compile-time ends
}

// C++20: Runtime vs. Compile-time branching inside a single constexpr function
constexpr int dual_mode_calc(int x) {
    if consteval { // C++23 syntax (C++20 used: if (std::is_constant_evaluated()))
        return x * 10; // Compile-time path
    } else {
        return x * 2;  // Runtime path
    }
}

// ============================================================================
// 2. CONSTEXPR CLASSES & CONSTRUCTORS (C++11 / C++20)
// ============================================================================

struct Point {
    int x = 0;
    int y = 0;

    constexpr Point(int x_val, int y_val) : x(x_val), y(y_val) {}
    constexpr Point operator+(const Point& other) const {
        return {x + other.x, y + other.y};
    }
    // C++20: Virtual destructors and virtual functions can also be constexpr!
    constexpr virtual int magnitude_sq() const { return x * x + y * y; }
    constexpr virtual ~Point() = default;
};

// ============================================================================
// 3. CONSTINIT (C++20)
// ============================================================================

// Guarantees compile-time initialization (prevents static init order fiasco),
// but the variable remains MUTABLE at runtime.
constinit int global_counter = compute(5);

// ============================================================================
// 4. CONSTEXPR LAMBDAS (C++17 / C++20)
// ============================================================================

// Lambdas are implicitly constexpr since C++17 if they meet the rules
constexpr auto factorial = [](this auto self, int n) -> int { // C++23 deducing 'this' recursion
    return (n <= 1) ? 1 : n * self(n - 1);
};

// ============================================================================
// 5. TEMPLATE METAPROGRAMMING, CONCEPTS & IF CONSTEXPR (C++11 - C++20)
// ============================================================================

// Classic Template Metaprogramming (Value & Type traits)
template <int N>
struct Fib {
    static constexpr int value = Fib<N - 1>::value + Fib<N - 2>::value;
};
template <> struct Fib<0> { static constexpr int value = 0; };
template <> struct Fib<1> { static constexpr int value = 1; };

// C++14 Variable Template
template <int N>
inline constexpr int Fib_v = Fib<N>::value;

// C++20 Concepts (Compile-time type constraints)
template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

// C++17 'if constexpr' (Discards false branches completely at compile time)
template <Arithmetic T>
constexpr auto stringify_or_double(T val) {
    if constexpr (std::is_floating_point_v<T>) {
        return val * 2.0; // Discarded if T is int
    } else {
        return val + 100; // Discarded if T is double
    }
}

// C++17 Fold Expressions (Compile-time variadic pack expansion)
template <typename... Args>
constexpr auto sum_all(Args... args) {
    return (args + ... + 0);
}

// ============================================================================
// 6. NON-TYPE TEMPLATE PARAMETERS (NTTP) (C++20)
// ============================================================================

// C++20 allows literal class types (like std::array or custom structs) as template arguments
template <Point P>
constexpr int get_point_x() {
    return P.x;
}

// ============================================================================
// 7. STATIC_ASSERT & COMPILE-TIME EXECUTION (C++11 / C++17)
// ============================================================================

// Compile-time unit testing (zero runtime cost)
static_assert(compute(5) == 15);
static_assert(fnv1a_hash("hello") == 0x4f9f2cab);
static_assert(count_even_numbers(10) == 5);
static_assert(dual_mode_calc(3) == 30); // Calls 'if consteval' branch
static_assert(factorial(5) == 120);
static_assert(Fib_v<7> == 13);
static_assert(stringify_or_double(5) == 105);
static_assert(stringify_or_double(2.5) == 5.0);
static_assert(sum_all(1, 2, 3, 4) == 10);
static_assert(get_point_x<Point{42, 99}>() == 42);

// Immediately Invoked Lambda Expression (IILE) for complex compile-time lookup tables
constexpr auto lookup_table = []() consteval {
    std::array<int, 5> arr{};
    for (int i = 0; i < 5; ++i) {
        arr[i] = (i + 1) * (i + 1);
    }
    return arr;
}();

static_assert(lookup_table == 16);

// ============================================================================
// MAIN (RUNTIME VERIFICATION)
// ============================================================================

int main() {
    // Modify constinit variable at runtime
    global_counter += 10;

    // Runtime execution of dual_mode_calc
    int runtime_input = 3;
    int runtime_res = dual_mode_calc(runtime_input); // Calls 'else' branch (returns 6)

    // Compile-time constant already baked into binary
    constexpr Point p1{3, 4};
    constexpr Point p2 = p1 + Point{1, 2};

    std::cout << "Constinit global: " << global_counter << "\n";
    std::cout << "Runtime dual_mode: " << runtime_res << "\n";
    std::cout << "Constexpr Point mag^2: " << p2.magnitude_sq() << "\n";

    return 0;
}

// ============================================================================

#include <array>
#include <cstdint>
#include <iostream>
#include <string_view>

// ============================================================================
// 1. CUSTOM COMPILE-TIME ERROR TRIGGERS
// ============================================================================

// Non-constexpr functions cannot be called during constant evaluation.
// Naming them descriptively makes the compiler error act as a custom message!
void ERROR_INVALID_CHARACTER_IN_IP();
void ERROR_OCTET_EXCEEDS_255();
void ERROR_EMPTY_OCTET();
void ERROR_INVALID_OCTET_COUNT();

// ============================================================================
// 2. COMPILE-TIME STRING PARSER WITH VALIDATION
// ============================================================================

struct IPv4 {
    uint32_t packed = 0;

    // 'consteval' constructor forces all string parsing to happen at compile time!
    // Implicit conversion allows passing string literals directly to functions expecting IPv4.
    consteval IPv4(const char* str) {
        std::string_view sv(str);
        std::array<uint32_t, 4> octets{};
        size_t octet_idx = 0;
        uint32_t current_val = 0;
        bool has_digits = false;

        for (char c : sv) {
            if (c >= '0' && c <= '9') {
                has_digits = true;
                current_val = current_val * 10 + (c - '0');
                
                if (current_val > 255) {
                    ERROR_OCTET_EXCEEDS_255(); // Halts compilation here!
                }
            } else if (c == '.') {
                if (!has_digits) {
                    ERROR_EMPTY_OCTET(); // Halts compilation (e.g., "192..1.1")
                }
                if (octet_idx >= 3) {
                    ERROR_INVALID_OCTET_COUNT(); // Halts compilation (too many dots)
                }
                octets[octet_idx++] = current_val;
                current_val = 0;
                has_digits = false;
            } else {
                ERROR_INVALID_CHARACTER_IN_IP(); // Halts compilation (e.g., "192.168.a.1")
            }
        }

        // Validate final octet
        if (!has_digits) {
            ERROR_EMPTY_OCTET(); // Halts compilation (e.g., "192.168.1.")
        }
        if (octet_idx != 3) {
            ERROR_INVALID_OCTET_COUNT(); // Halts compilation (too few dots)
        }
        octets[3] = current_val;

        // Pack into single 32-bit integer at compile time
        packed = (octets[0] << 24) | (octets[1] << 16) | (octets[2] << 8) | octets[3];
    }
};

// ============================================================================
// 3. ZERO-COST API USAGE
// ============================================================================

void connect_to_server(IPv4 address) {
    // At runtime, 'address.packed' is already a raw integer constant!
    std::cout << "Connecting to packed IP: 0x" << std::hex << address.packed << "\n";
}

int main() {
    // ✅ VALID: Parsed at compile time. Zero parsing code exists in the binary.
    connect_to_server("192.168.1.1");

    // ❌ INVALID: Uncommenting any line below causes a compile-time error
    // pointing directly to the specific ERROR_* function:

    // connect_to_server("192.168.999.1"); // Error: call to non-constexpr 'ERROR_OCTET_EXCEEDS_255()'
    // connect_to_server("192.168.1");     // Error: call to non-constexpr 'ERROR_INVALID_OCTET_COUNT()'
    // connect_to_server("192.168.abc.1"); // Error: call to non-constexpr 'ERROR_INVALID_CHARACTER_IN_IP()'

    return 0;
}

// ============================================================================

#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <string_view>
#include <tuple>

// ============================================================================
// 1. THE FIXED-STRING WRAPPER (C++20 Structural Type)
// ============================================================================

template <std::size_t N>
struct FixedString {
    char data[N]{};

    // Implicitly converts string literals ("hello") into FixedString<6>
    consteval FixedString(const char (&str)[N]) {
        std::copy_n(str, N, data);
    }

    constexpr std::string_view view() const {
        return {data, N - 1}; // Exclude null terminator
    }
};

// ============================================================================
// 2. COMPILE-TIME PARSER DRIVING TYPE GENERATION
// ============================================================================

void ERROR_UNKNOWN_FORMAT_SPECIFIER();

// Count how many '{...}' tokens exist in the template string
template <FixedString Fmt>
consteval std::size_t count_tokens() {
    std::size_t count = 0;
    for (char c : Fmt.view()) {
        if (c == '{') ++count;
    }
    return count;
}

// Parse the N-th token inside the string and return its corresponding C++ type
template <FixedString Fmt, std::size_t TargetIdx>
consteval auto deduce_token_type() {
    constexpr std::string_view sv = Fmt.view();
    std::size_t current_token = 0;
    std::size_t pos = 0;

    while (pos < sv.size()) {
        auto open = sv.find('{', pos);
        if (open == std::string_view::npos) break;
        auto close = sv.find('}', open);

        if (current_token == TargetIdx) {
            std::string_view token = sv.substr(open + 1, close - open - 1);
            if (token == "int") return int{};
            if (token == "double") return double{};
            if (token == "str") return std::string_view{};
            
            ERROR_UNKNOWN_FORMAT_SPECIFIER(); // Halts build if invalid type name!
        }
        ++current_token;
        pos = close + 1;
    }
}

// Helper to assemble the std::tuple type from all parsed tokens
template <FixedString Fmt, std::size_t... Is>
auto make_parsed_tuple_impl(std::index_sequence<Is...>) {
    return std::tuple<decltype(deduce_token_type<Fmt, Is>())...>{};
}

// ============================================================================
// 3. USER-FACING API (String Literal -> Custom C++ Return Type)
// ============================================================================

template <FixedString Fmt>
auto create_record() {
    constexpr std::size_t num_fields = count_tokens<Fmt>();
    return make_parsed_tuple_impl<Fmt>(std::make_index_sequence<num_fields>{});
}

// ============================================================================
// 4. USAGE & VERIFICATION
// ============================================================================

int main() {
    // The string literal directly dictates the C++ type of 'record'!
    // Compiler deduces: std::tuple<int, double, std::string_view>
    auto record = create_record<"{int}, {double}, {str}">();

    static_assert(std::is_same_v<
        decltype(record),
        std::tuple<int, double, std::string_view>
    >);

    // Assign values with full compile-time type safety
    std::get<0>(record) = 42;
    std::get<1>(record) = 3.14;
    std::get<2>(record) = "Compile-time magic";

    std::cout << std::get<0>(record) << " | "
              << std::get<1>(record) << " | "
              << std::get<2>(record) << "\n";

    // ❌ Compile-time error: calls non-constexpr ERROR_UNKNOWN_FORMAT_SPECIFIER()
    // auto bad = create_record<"{float}">();

    return 0;
}
