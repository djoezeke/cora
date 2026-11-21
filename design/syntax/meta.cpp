// =========================================================================
// 1. Class Templates Recursion

#include <type_traits>

// Value computation via recursion
template <unsigned N>
struct Factorial {
    static constexpr unsigned value = N * Factorial<N - 1>::value;
};

template <>
struct Factorial<0> {
    static constexpr unsigned value = 1;
};

// Type transformation (Custom Type Trait)
template <typename T>
struct RemovePointer {
    using type = T;
};

template <typename T>
struct RemovePointer<T*> {
    using type = T;
};

template <typename T>
using RemovePointer_t = typename RemovePointer<T>::type;


// =========================================================================
// 1. Non-Intrusive Serialization (Reflection + template for)
// =========================================================================
#include <iostream>
#include <meta>
#include <string>

template <typename T>
void serialize(const T& obj, std::ostream& os) {
    os << "{ ";
    constexpr auto ctx = std::meta::access_context::current();
    bool first = true;

    // template for unrolls at compile time over heterogeneous members
    template for (constexpr auto member : std::meta::nonstatic_data_members_of(^^T, ctx)) {
        if (!first) os << ", ";
        first = false;

        // identifier_of gets the field name; obj.[:member:] splices the member access
        os << '"' << std::meta::identifier_of(member) << "\": " << obj.[:member:];
    }
    os << " }\n";
}

// Plain aggregate struct—zero boilerplate required
struct Player {
    int id;
    double health;
    std::string name;
};

// Usage:
// Player p{1, 98.5, "Aria"};
// serialize(p, std::cout); // Output: { "id": 1, "health": 98.5, "name": Aria }

// =========================================================================
//1. Compile-Time Serialization (Member Pointers & Fold Expressions)

#include <iostream>
#include <tuple>
#include <string>

template <typename T>
struct Serializer {
    // Expects T to define a static `fields()` returning a tuple of member pointers
    static void serialize(const T& obj, std::ostream& os) {
        os << "{ ";
        std::apply([&](auto... member_ptrs) {
            size_t count = 0;
            // Fold expression iterates over every member pointer at compile time
            ((os << (count++ ? ", " : "") << obj.*member_ptrs), ...);
        }, T::fields());
        os << " }\n";
    }
};

struct Player {
    int id;
    double health;
    std::string name;

    // Register fields once; serialization adapts automatically
    static constexpr auto fields() {
        return std::make_tuple(&Player::id, &Player::health, &Player::name);
    }
};

// Usage:
// Player p{1, 98.5, "Aria"};
// Serializer<Player>::serialize(p, std::cout); // Output: { 1, 98.5, Aria }

// =========================================================================
// 2. Type Lists as std::vector<std::meta::info>
// =========================================================================
#include <meta>
#include <vector>
#include <ranges>

// Manipulate types using normal C++ vectors and loops at compile time
consteval auto add_pointers_to_integrals(std::vector<std::meta::info> types) {
    std::vector<std::meta::info> result;
    for (auto t : types) {
        if (std::meta::is_integral_type(t)) {
            // std::meta::add_pointer operates directly on reflection values
            result.push_back(std::meta::add_pointer(t));
        }
    }
    return result;
}

// Evaluate at compile time and splice back into a concrete type
constexpr auto input_types = std::vector{^^int, ^^double, ^^char, ^^float};
constexpr auto ptr_types = add_pointers_to_integrals(input_types);

// Splice the 2nd reflected type (char*) into a variable declaration
typename[:ptr_types[1]:] char_ptr = nullptr;
static_assert(std::is_same_v<decltype(char_ptr), char*>);

// usage:
// for (auto t : ptr_types) {
//     std::cout << std::meta::type_name(t) << "\n"; // Outputs: int*, char*
// }

// =========================================================================
// 3. Type Lists (Compile-Time Type Manipulation)
#include <tuple>
#include <type_traits>

template <typename... Ts>
struct TypeList {
    static constexpr size_t size = sizeof...(Ts);
};

// 1. Get type at Index N (O(1) lookup via std::tuple_element)
template <size_t N, typename List>
struct TypeAt;

template <size_t N, typename... Ts>
struct TypeAt<N, TypeList<Ts...>> {
    using type = std::tuple_element_t<N, std::tuple<Ts...>>;
};

template <size_t N, typename List>
using TypeAt_t = typename TypeAt<N, List>::type;

// 2. Transform every type in the list (e.g., add pointer to all types)
template <template <typename> class Trait, typename List>
struct Transform;

template <template <typename> class Trait, typename... Ts>
struct Transform<Trait, TypeList<Ts...>> {
    using type = TypeList<typename Trait<Ts>::type...>;
};

template <template <typename> class Trait, typename List>
using Transform_t = typename Transform<Trait, List>::type;

// Usage:
// using MyTypes = TypeList<int, double, char>;
// static_assert(std::is_same_v<TypeAt_t<1, MyTypes>, double>);
// using PtrTypes = Transform_t<std::add_pointer, MyTypes>; // TypeList<int*, double*, char*>


// =========================================================================
// 3. CRTP Replaced by Deducing this (C++23) & Reflection
// =========================================================================

#include <iostream>
#include <meta>
#include <string>

// Option A: Deducing `this` (Replaces CRTP for mixins)
struct LoggerMixin {
    template <typename Self>
    void log(this const Self& self) {
        std::cout << "[LOG]: " << self.format_impl() << '\n';
    }
};

struct NetworkPacket : LoggerMixin { // No <NetworkPacket> template argument needed
    int port;
    std::string format_impl() const {
        return "Packet on port " + std::to_string(port);
    }
};

// Usage:
// NetworkPacket packet{8080};
// packet.log(); // Outputs: [LOG]: Packet on port 8080

// ==========================================================================
// 4. CRTP (Curiously Recurring Template Pattern)

#include <iostream>
#include <concepts>

// Optional C++20 Concept to enforce the CRTP contract with clean compiler errors
template <typename T>
concept Printable = requires(const T& t) {
    { t.format_impl() } -> std::convertible_to<std::string>;
};

template <typename Derived>
class LoggerMixin {
public:
    void log() const {
        // Compile-time check ensuring Derived implements the required method
        static_assert(Printable<Derived>, "Derived must implement format_impl()");
        std::cout << "[LOG]: " << static_cast<const Derived*>(this)->format_impl() << '\n';
    }
};

class NetworkPacket : public LoggerMixin<NetworkPacket> {
    int port;
public:
    NetworkPacket(int p) : port(p) {}

    // Implementation called by the base class without virtual dispatch
    std::string format_impl() const {
        return "Packet on port " + std::to_string(port);
    }
};

// Usage:
// NetworkPacket pkt(8080);
// pkt.log(); // Output: [LOG]: Packet on port 8080

// Option B: Reflection-based interface check (Zero inheritance required)
template <typename T>
consteval bool has_format_method() {
    constexpr auto ctx = std::meta::access_context::current();
    template for (constexpr auto fn : std::meta::members_of(^^T, ctx)) {
        if (std::meta::has_identifier(fn) && std::meta::identifier_of(fn) == "format_impl") {
            return true;
        }
    }
    return false;
}
static_assert(has_format_method<NetworkPacket>());

// =========================================================================
// 4. Expression Templates & Pack Indexing (C++26)
// =========================================================================
#include <cstddef>

// Variadic lazy sum node holding N expressions without std::tuple overhead
template <typename... Exprs>
struct MultiAdd {
    // Pack indexing (C++26): access the 0th type directly
    using FirstType = Exprs...[0];
    
    // Store references in a tuple or capture via lambda/struct
    std::tuple<const Exprs&...> exprs;

    constexpr double operator[](size_t i) const {
        double total = 0.0;
        // Expansion statement iterates directly over the tuple elements
        template for (const auto& e : exprs) {
            total += e[i];
        }
        return total;
    }
};

// Usage:
// MultiAdd<Expr1, Expr2, Expr3> sum_node{std::tie(expr1, expr2, expr3)};


// =========================================================================
// 2. Expression Templates (Lazy Vector Math)

#include <vector>
#include <cassert>

// Base expression wrapper (uses CRTP to expose derived indexing)
template <typename Expr>
struct VecExpr {
    double operator[](size_t i) const { return static_cast<const Expr&>(*this)[i]; }
    size_t size() const { return static_cast<const Expr&>(*this).size(); }
};

// Concrete Vector container
class Vec : public VecExpr<Vec> {
    std::vector<double> data;
public:
    Vec(std::initializer_list<double> list) : data(list) {}
    Vec(size_t n) : data(n) {}

    double operator[](size_t i) const { return data[i]; }
    double& operator[](size_t i) { return data[i]; }
    size_t size() const { return data.size(); }

    // Evaluates the entire expression tree in a single loop
    template <typename Expr>
    Vec& operator=(const VecExpr<Expr>& expr) {
        assert(size() == expr.size());
        for (size_t i = 0; i < size(); ++i) {
            data[i] = expr[i];
        }
        return *this;
    }
};

// Lazy Addition Node (stores references, allocates zero memory)
template <typename LHS, typename RHS>
class VecAdd : public VecExpr<VecAdd<LHS, RHS>> {
    const LHS& lhs;
    const RHS& rhs;
public:
    VecAdd(const LHS& l, const RHS& r) : lhs(l), rhs(r) {}
    double operator[](size_t i) const { return lhs[i] + rhs[i]; }
    size_t size() const { return lhs.size(); }
};

// Overloaded operator+ returns a lightweight expression node instead of a Vec
template <typename LHS, typename RHS>
VecAdd<LHS, RHS> operator+(const VecExpr<LHS>& lhs, const VecExpr<RHS>& rhs) {
    return VecAdd<LHS, RHS>(static_cast<const LHS&>(lhs), static_cast<const RHS&>(rhs));
}

// Usage:
// Vec a{1, 2, 3}, b{4, 5, 6}, c{7, 8, 9}, result(3);
// result = a + b + c; // Zero temporary Vecs created; single pass loop
