// ============================================================================
// Function Template
// ============================================================================
#include <iostream>

template <typename T>
T getMax(T a, T b) {
    return (a > b) ? a : b;
}

int main() {
    std::cout << getMax(10, 20) << '\n';       // Deduces int
    std::cout << getMax(3.14, 2.71) << '\n';   // Deduces double
}

// ============================================================================
// class Template
// ============================================================================

template <typename T>
class Box {
private:
    T value;
public:
    Box(T v) : value(v) {}
    T getValue() const { return value; }
};

// Usage: Box<int> intBox(42);

// ============================================================================
// C++20 Constraint
// ============================================================================

#include <concepts>

// Restricts T to types that support standard arithmetic addition
template <typename T>
requires std::integral<T> || std::floating_point<T>
T add(T a, T b) {
    return a + b;
}

// ============================================================================
// C++20 Concept
// ============================================================================

#include <concepts>
#include <iostream>

// Defining a custom concept using a requires expression
template <typename T>
concept PrintableAndAddable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>; // Must support '+' returning T
    { std::cout << a };                    // Must support stream output
};

// 1. Constrained template parameter syntax (terse & common)
template <PrintableAndAddable T>
void addAndPrint(T a, T b) {
    std::cout << (a + b) << '\n';
}

// 2. Abbreviated function template syntax (C++20 shorthand)
void addAndPrintShort(PrintableAndAddable auto a, PrintableAndAddable auto b) {
    std::cout << (a + b) << '\n';
}

// ============================================================================

#include <concepts>
#include <iostream>
#include <string>

// 1. Define the Concept (the compile-time interface contract)
// Any type T satisfies 'Speaker' if calling t.speak() is valid 
// and returns something convertible to std::string.
template <typename T>
concept Speaker = requires(const T t) {
    { t.speak() } -> std::convertible_to<std::string>;
};

// 2. Define standalone concrete types (No inheritance required!)
struct Dog {
    std::string name;
    std::string speak() const {
        return name + " says Woof!";
    }
};

struct Robot {
    int id;
    std::string speak() const {
        return "Unit " + std::to_string(id) + " says Beep!";
    }
};

// 3. Polymorphic function constrained by the Concept
// Uses abbreviated function template syntax (`Speaker auto`)
void makeItSpeak(const Speaker auto& s) {
    std::cout << s.speak() << "\n";
}

int main() {
    Dog d{"Rex"};
    Robot r{42};

    // Compiler generates specialized, inlined functions for Dog and Robot
    makeItSpeak(d);
    makeItSpeak(r);
}

// ============================================================================
//  Pack Indexing
// ============================================================================

#include <cstddef>

// Extract the Nth type directly without std::tuple_element
template <std::size_t I, typename... Ts>
using NthType = Ts...[I];

// Extract the Nth argument directly without recursion
template <std::size_t I, typename... Args>
constexpr decltype(auto) getNthArg(Args&&... args) {
    return args...[I];
}

// ============================================================================
// Static Reflection & Expansion Statements
// ============================================================================

#include <meta>
#include <string>
#include <string_view>

// Generic enum-to-string converter for any enum type
template <typename E>
requires std::is_enum_v<E>
constexpr std::string_view enumToString(E value) {
    template for (constexpr auto e : std::meta::enumerators_of(^^E)) {
        if (value == [:e:]) {
            return std::meta::identifier_of(e);
        }
    }
    return "unknown";
}

// ============================================================================
//  Pack Indexing
// ============================================================================

#include <meta>
#include <string>
#include <vector>
#include <type_traits>
#include <iostream>

// Forward declaration for recursive calls
template <typename T>
std::string to_json(const T& val);

// Overloads for primitives, strings, and containers
std::string to_json(bool b) { return b ? "true" : "false"; }
std::string to_json(int n) { return std::to_string(n); }
std::string to_json(double d) { return std::to_string(d); }
std::string to_json(const std::string& s) { return "\"" + s + "\""; }

template <typename T>
std::string to_json(const std::vector<T>& vec) {
    std::string out = "[";
    for (std::size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) out += ", ";
        out += to_json(vec[i]);
    }
    return out + "]";
}

// Generic fallback for any aggregate struct using C++26 Reflection
template <typename T>
std::string to_json(const T& obj) {
    static_assert(std::is_aggregate_v<T>, "Expected an aggregate type");
    
    std::string out = "{";
    bool first = true;
    constexpr auto ctx = std::meta::access_context::current();

    // define_static_array lifts the compile-time vector into a constant range for template for
    template for (constexpr auto member : std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, ctx))) {
        
        if (!first) out += ", ";
        first = false;

        // Extract field name as string_view and splice member access directly
        out += "\"" + std::string(std::meta::identifier_of(member)) + "\": ";
        out += to_json(obj.[:member:]);
    }

    return out + "}";
}

// --- Usage ---
struct Item { int id; double price; };
struct Order { std::string customer; bool paid; std::vector<Item> items; };

int main() {
    Order order{"Ada", true, {{101, 19.5}, {202, 42.0}}};
    std::cout << to_json(order) << "\n";
    // Output: {"customer": "Ada", "paid": true, "items": [{"id": 101, "price": 19.500000}, {"id": 202, "price": 42.000000}]}
}

#include <array>
#include <cstddef>
#include <meta>
#include <vector>

template <typename T, std::size_t N>
struct struct_of_arrays_impl {
    // 1. Forward-declare the incomplete target struct inside the instantiation scope
    struct type;

    // 2. Programmatically define its data members at compile time
    consteval {
        constexpr auto ctx = std::meta::access_context::current();
        std::vector<std::meta::info> new_members;

        for (std::meta::info member : std::meta::nonstatic_data_members_of(^^T, ctx)) {
            // Synthesize std::array<MemberType, N> using std::meta::substitute
            std::meta::info array_type = std::meta::substitute(
                ^^std::array,
                {std::meta::type_of(member), std::meta::reflect_constant(N)}
            );

            // Create a member specification preserving the original identifier
            new_members.push_back(std::meta::data_member_spec(
                array_type,
                {.name = std::meta::identifier_of(member)}
            ));
        }

        // Inject the generated fields into the incomplete struct
        std::meta::define_aggregate(^^type, new_members);
    }
};

template <typename T, std::size_t N>
using struct_of_arrays = typename struct_of_arrays_impl<T, N>::type;

// --- Usage ---
struct Particle {
    float x;
    float y;
    float z;
    int id;
};

int main() {
    // Equivalent to:
    // struct { std::array<float, 1024> x, y, z; std::array<int, 1024> id; };
    // struct_of_arrays<Particle, 1024> particles{};

    struct_of_arrays<Particle, 1024> particles;

    particles.x[0] = 1.5f;
    particles.y[0] = 2.5f;
    particles.id[0] = 42;
}

//////////////
#include <meta>
#include <string>
#include <string_view>
#include <iostream>

// 1. Define annotation tag types (must be structural types)
struct Skip {};

struct Rename {
    char name[64]{};
    consteval Rename(std::string_view sv) {
        for (std::size_t i = 0; i < sv.size() && i < 63; ++i) {
            name[i] = sv[i];
        }
    }
};

// 2. Helper consteval functions to inspect member annotations
consteval bool has_skip(std::meta::info member) {
    return !std::meta::annotations_of(member, ^^Skip).empty();
}

consteval std::string_view get_json_key(std::meta::info member) {
    auto renames = std::meta::annotations_of(member, ^^Rename);
    if (!renames.empty()) {
        // Extract the Rename struct and read its stored name
        return std::meta::extract<Rename>(renames.front()).name;
    }
    return std::meta::identifier_of(member);
}

// 3. Serialization function respecting annotations
std::string to_json_val(int v) { return std::to_string(v); }
std::string to_json_val(const std::string& s) { return "\"" + s + "\""; }

template <typename T>
std::string to_json(const T& obj) {
    std::string out = "{";
    bool first = true;
    constexpr auto ctx = std::meta::access_context::current();

    template for (constexpr auto member : std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, ctx))) {
        
        // Skip fields annotated with [[=Skip{}]] at compile time
        if constexpr (!has_skip(member)) {
            if (!first) out += ", ";
            first = false;

            // Use [[=Rename("...")]] if present, otherwise default identifier
            constexpr std::string_view key = get_json_key(member);
            out += "\"" + std::string(key) + "\": " + to_json_val(obj.[:member:]);
        }
    }

    return out + "}";
}

// 4. Apply annotations to struct fields
struct UserAccount {
    int id;

    [[=Rename("user_name")]]
    std::string username;

    [[=Skip{}]]
    std::string password_hash;
};

int main() {
    UserAccount user{42, "grace_hopper", "s3cr3t_h4sh"};
    std::cout << to_json(user) << "\n";
    // Output: {"id": 42, "user_name": "grace_hopper"}
}

///////////////

#include <charconv>
#include <iostream>
#include <meta>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

// 1. Define a structural annotation type for CLI flags
struct Flag {
    const char* long_name;
    const char* short_name;
    const char* help;

    consteval Flag(std::string_view l, std::string_view s, std::string_view h)
        : long_name(std::define_static_string(l)),
          short_name(std::define_static_string(s)),
          help(std::define_static_string(h)) {}
};

// 2. Type-safe string-to-value conversion helpers
template <typename T>
T parse_value(std::string_view str) {
    if constexpr (std::is_same_v<T, std::string>) {
        return std::string(str);
    } else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
        T val{};
        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), val);
        if (ec != std::errc{} || ptr != str.data() + str.size()) {
            throw std::runtime_error("Invalid integer value: " + std::string(str));
        }
        return val;
    } else {
        static_assert(false, "Unsupported CLI option type");
    }
}

// 3. Auto-generated --help printer using reflected annotations
template <typename Config>
void print_help(std::string_view prog_name) {
    std::cout << "Usage: " << prog_name << " [OPTIONS]\n\nOptions:\n";
    constexpr auto ctx = std::meta::access_context::current();

    template for (constexpr auto member : std::define_static_array(
        std::meta::nonstatic_data_members_of(^^Config, ctx))) {
        
        constexpr auto flags = std::meta::annotations_of(member, ^^Flag);
        if constexpr (!flags.empty()) {
            constexpr Flag meta = std::meta::extract<Flag>(flags.front());
            std::cout << "  " << meta.short_name << ", " << meta.long_name
                      << "\t" << meta.help << "\n";
        }
    }
}

// 4. Generic command-line parser
template <typename Config>
Config parse_args(int argc, char* argv[]) {
    Config cfg{};
    constexpr auto ctx = std::meta::access_context::current();

    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            print_help<Config>(argv[0]);
            std::exit(0);
        }

        bool matched = false;

        template for (constexpr auto member : std::define_static_array(
            std::meta::nonstatic_data_members_of(^^Config, ctx))) {
            
            constexpr auto flags = std::meta::annotations_of(member, ^^Flag);
            if constexpr (!flags.empty()) {
                constexpr Flag meta = std::meta::extract<Flag>(flags.front());

                if (arg == meta.long_name || arg == meta.short_name) {
                    matched = true;
                    using FieldType = typename[:std::meta::type_of(member):];

                    // Boolean flags act as standalone switches (--verbose)
                    if constexpr (std::is_same_v<FieldType, bool>) {
                        cfg.[:member:] = true;
                    } else {
                        // Value flags consume the next argv token (--port 8080)
                        if (i + 1 >= argc) {
                            throw std::runtime_error("Missing value for " + std::string(arg));
                        }
                        cfg.[:member:] = parse_value<FieldType>(argv[++i]);
                    }
                }
            }
        }

        if (!matched) {
            throw std::runtime_error("Unknown argument: " + std::string(arg));
        }
    }

    return cfg;
}

// 5. Define application configuration with default values
struct ServerConfig {
    [[=Flag("--host", "-H", "Interface address to bind")]]
    std::string host = "127.0.0.1";

    [[=Flag("--port", "-p", "TCP port to listen on")]]
    int port = 8080;

    [[=Flag("--workers", "-w", "Number of worker threads")]]
    int workers = 4;

    [[=Flag("--verbose", "-v", "Enable debug logging")]]
    bool verbose = false;
};

int main(int argc, char* argv[]) {
    try {
        ServerConfig config = parse_args<ServerConfig>(argc, argv);

        std::cout << "Starting server on " << config.host << ":" << config.port
                  << " with " << config.workers << " workers"
                  << (config.verbose ? " [VERBOSE]\n" : "\n");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
