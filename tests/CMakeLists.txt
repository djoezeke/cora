# ===================================================================================
# Copyright © 2026 Sackey Ezekiel Etrue
# Distributed under the terms of the Apache License, Version 2.0.
# ===================================================================================

find_package(Catch2 3 QUIET)

if (NOT Catch2_FOUND)
    include(FetchContent)

    message(STATUS "Catch2 v3 not found, fetching it with FetchContent")

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.9.1
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(Catch2)
endif()

add_executable(CoraCoreTests
    Cora/Core/SourceFile.test.cpp
    Cora/Core/SourceLocation.test.cpp
    Cora/Core/SourceManager.test.cpp
)

target_include_directories(CoraCoreTests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Core is currently header-only, so the tests do not need to link the Cora library.
target_link_libraries(CoraCoreTests
    PRIVATE
        Catch2::Catch2WithMain
)

target_compile_features(CoraCoreTests PRIVATE cxx_std_17)

if (MSVC)
    target_compile_options(CoraCoreTests PRIVATE /W4 /permissive-)
else()
    target_compile_options(CoraCoreTests PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Catch2's TEST_CASE macro uses __COUNTER__, which newer Clang versions warn about.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(CoraCoreTests PRIVATE -Wno-c2y-extensions)
endif()

add_test(NAME CoraCoreTests COMMAND CoraCoreTests)

find_package(Threads REQUIRED)

add_executable(CoraDiagTests
    Cora/Diag/Diagnostic.test.cpp
    Cora/Diag/DiagnosticConfig.test.cpp
    Cora/Diag/DiagnosticEngine.test.cpp
    Cora/Diag/DiagnosticEmitter.test.cpp
    Cora/Diag/DiagnosticFlagParser.test.cpp
)

target_include_directories(CoraDiagTests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(CoraDiagTests
    PRIVATE
        CoraDiag
        Catch2::Catch2WithMain
        Threads::Threads
)

target_compile_features(CoraDiagTests PRIVATE cxx_std_17)

if (MSVC)
    target_compile_options(CoraDiagTests PRIVATE /W4 /permissive-)
else()
    target_compile_options(CoraDiagTests PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(CoraDiagTests PRIVATE -Wno-c2y-extensions)
endif()

add_test(NAME CoraDiagTests COMMAND CoraDiagTests)

add_executable(CoraLexerTests
    Cora/Lexer/Token.test.cpp
    Cora/Lexer/Lexer.test.cpp
    Cora/Lexer/TokenStream.test.cpp
    Cora/Lexer/IncrementalLexer.test.cpp
)

target_include_directories(CoraLexerTests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(CoraLexerTests
    PRIVATE
        CoraLexer
        CoraDiag
        Catch2::Catch2WithMain
)

target_compile_features(CoraLexerTests PRIVATE cxx_std_17)
target_compile_definitions(CoraLexerTests PRIVATE
    CORA_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
)

if (MSVC)
    target_compile_options(CoraLexerTests PRIVATE /W4 /permissive-)
else()
    target_compile_options(CoraLexerTests PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(CoraLexerTests PRIVATE -Wno-c2y-extensions)
endif()

add_test(NAME CoraLexerTests COMMAND CoraLexerTests)

add_executable(CoraParserTests
    Cora/Parser/Parser.test.cpp
    Cora/Parser/AST.test.cpp
    Cora/Parser/IncrementalParser.test.cpp
)

target_include_directories(CoraParserTests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(CoraParserTests
    PRIVATE
        CoraParser
        CoraLexer
        CoraDiag
        Catch2::Catch2WithMain
)

target_compile_features(CoraParserTests PRIVATE cxx_std_17)
target_compile_definitions(CoraParserTests PRIVATE
    CORA_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
)

if (MSVC)
    target_compile_options(CoraParserTests PRIVATE /W4 /permissive-)
else()
    target_compile_options(CoraParserTests PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(CoraParserTests PRIVATE -Wno-c2y-extensions)
endif()

add_test(NAME CoraParserTests COMMAND CoraParserTests)

# add_executable(CoraFrontendTests
#     Cora/Frontend/Frontend.test.cpp
# )

# target_include_directories(CoraFrontendTests
#     PRIVATE
#         ${PROJECT_SOURCE_DIR}/include
# )

# target_link_libraries(CoraFrontendTests
#     PRIVATE
#         Cora
#         Catch2::Catch2WithMain
# )

# target_compile_features(CoraFrontendTests PRIVATE cxx_std_17)
# add_test(NAME CoraFrontendTests COMMAND CoraFrontendTests)

add_executable(CoraSemaTests
    Cora/Sema/Sema.test.cpp
)
target_include_directories(CoraSemaTests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(CoraSemaTests PRIVATE Cora Catch2::Catch2WithMain)
target_compile_features(CoraSemaTests PRIVATE cxx_std_17)
add_test(NAME CoraSemaTests COMMAND CoraSemaTests)

add_executable(CoraCodeGenTests
    Cora/CodeGen/CBackend.test.cpp
)
target_include_directories(CoraCodeGenTests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(CoraCodeGenTests PRIVATE Cora Catch2::Catch2WithMain)
target_compile_features(CoraCodeGenTests PRIVATE cxx_std_17)
add_test(NAME CoraCodeGenTests COMMAND CoraCodeGenTests)

# ===================================================================================
