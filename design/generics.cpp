// =========================================================================
// 1.  Function Templates
// =========================================================================
// Allows a single function to operate on different data types.

#include <iostream>

// Basic function template ('typename' and 'class' are interchangeable here)
template <typename T>
T add(T a, T b) {
    return a + b;
}

// Multiple template parameters & trailing return type
template <typename T, typename U>
auto multiply(T a, U b) -> decltype(a * b) {
    return a * b;
}

// Abbreviated Function Template (C++20 shorthand using 'auto')
auto subtract(auto a, auto b) {
    return a - b;
}

int main() {
    std::cout << add(3, 4) << '\n';            // Type deduced as int
    std::cout << add<double>(3.5, 2.1) << '\n'; // Explicit type argument
    std::cout << multiply(3, 2.5) << '\n';      // Deduces T=int, U=double
    std::cout << subtract(5, 2) << '\n';         // Type deduced as int
}

// =========================================================================
// 1.  Struct Templates
// =========================================================================
// Allows structures to hold or manage generic data types.

#include <iostream>

template <typename T, typename U = int> // U has a default type
class Pair {
private:
    T first;
    U second;

public:
    Pair(T f, U s) : first(f), second(s) {}

    T getFirst() const { return first; }
    void print() const; // Member function declared inside, defined outside
};

// Out-of-line member function definition
template <typename T, typename U>
void Pair<T, U>::print() const {
    std::cout << first << ", " << second << '\n';
}

int main() {
    Pair<std::string, double> p1("Pi", 3.14);
    Pair p2("Count", 10); // CTAD (Class Template Argument Deduction - C++17)
    p2.print();
}

// =========================================================================
// 3. Non-Type Template Parameters (NTTP)
// =========================================================================
// Allows passing values (not just types) as template parameters.
// Allows compile-time constants(pointer, literal class types, etc.) to be used in templates.

#include <array>

template <typename T, std::size_t Size>
class FixedBuffer {
    T data[Size];
public:
    constexpr std::size_t capacity() const { return Size; }
};

int main() {
    FixedBuffer<int, 64> buf; // '64' is baked into the type at compile time
}

// =========================================================================
// 4. Variable & Alias Templates
// =========================================================================

#include <vector>

// Variable template (C++14)
template <typename T>
constexpr T pi = T(3.1415926535897932385L);

// Alias template (C++11) - replaces typedef for templates
template <typename T>
using IntMap = std::vector<std::pair<int, T>>;

int main() {
    double r = 2.0;
    double area = pi<double> * r * r;
    IntMap<std::string> myMap;
}

// =========================================================================
// 5. Template Specialization
// =========================================================================
// Customizes behavior for specific types when the generic implementation isn't optimal.

#include <iostream>

// 1. Primary Template
template <typename T, typename U>
struct Storage {
    static void info() { std::cout << "Generic Storage\n"; }
};

// 2. Partial Specialization (only valid for classes/structs, not functions)
template <typename T>
struct Storage<T*, int> {
    static void info() { std::cout << "Pointer + int Storage\n"; }
};

// 3. Full (Explicit) Specialization
template <>
struct Storage<char, char> {
    static void info() { std::cout << "Char-Char Specialized Storage\n"; }
};


// =========================================================================
// 6. Variadic Templates & Fold Expressions
// =========================================================================
// Handles an arbitrary number of template arguments (... parameter packs).

#include <iostream>

// Variadic function template with C++17 Fold Expression
template <typename... Args>
auto sumAll(Args... args) {
    return (args + ...); // Unary right fold: (arg1 + (arg2 + arg3))
}

template <typename... Args>
void printAll(const Args&... args) {
    ((std::cout << args << ' '), ...); // Fold over comma operator
    std::cout << '\n';
}

int main() {
    std::cout << sumAll(1, 2, 3, 4.5) << '\n'; // 10.5
    printAll("C++", 20, "Templates", true);
}

// =========================================================================
// 7. Constraints & Concepts 
// =========================================================================

#include <concepts>
#include <iostream>

// Defining a custom Concept
template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// Syntax 1: 'requires' clause
template <typename T>
requires Numeric<T>
T square1(T x) { return x * x; }

// Syntax 2: Constrained template parameter
template <Numeric T>
T square2(T x) { return x * x; }

// Syntax 3: Trailing 'requires' clause
template <typename T>
T square3(T x) requires std::integral<T> { return x * x; }

// Syntax 4: Abbreviated template with Concept
Numeric auto square4(Numeric auto x) { return x * x; }

// =========================================================================
// 8. Generic Lambdas 
// =========================================================================

int main() {
    // C++14 generic lambda
    auto addLambda = [](auto a, auto b) { return a + b; };

    // C++20 explicit template parameter list on a lambda
    auto sameTypeAdd = []<typename T>(T a, T b) -> T {
        return a + b;
    };
}
