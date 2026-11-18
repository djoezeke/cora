set(CORA_DIAG_SOURCES
    Cora/Diag/Diagnostic.cpp
    Cora/Diag/DiagnosticBuilder.cpp
    Cora/Diag/DiagnosticCodes.cpp
    Cora/Diag/DiagnosticEmitter.cpp
    Cora/Diag/DiagnosticEngine.cpp
    Cora/Diag/DiagnosticFlagParser.cpp
)

# The diagnostic layer is self-contained. Keeping it in its own target makes
# it usable by the compiler, tools and tests without pulling in codegen.
add_library(CoraDiag STATIC ${CORA_DIAG_SOURCES})

target_include_directories(CoraDiag
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(CoraDiag PUBLIC cxx_std_17)

if (MSVC)
    target_compile_options(CoraDiag PRIVATE /W4 /permissive-)
else()
    target_compile_options(CoraDiag PRIVATE -Wall -Wextra -Wpedantic)
endif()

set(CORA_LEXER_SOURCES
    Cora/Lexer/Interner.cpp
    Cora/Lexer/Lexer.cpp
    Cora/Lexer/Token.cpp
    Cora/Lexer/TokenStream.cpp
)

add_library(CoraLexer STATIC ${CORA_LEXER_SOURCES})

target_include_directories(CoraLexer
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(CoraLexer PUBLIC cxx_std_17)
target_link_libraries(CoraLexer PUBLIC CoraDiag)

if (MSVC)
    target_compile_options(CoraLexer PRIVATE /W4 /permissive-)
else()
    target_compile_options(CoraLexer PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---------------------------------------------------------------------------
# Parser: the Zig-style AST plus the hand-written recursive-descent parser.
#
# Kept as its own target while the legacy AIR/CIR codegen is migrated to the
# new AST, so the front-end can be built and tested in isolation.
# ---------------------------------------------------------------------------
set(CORA_PARSER_SOURCES
    Cora/AST/AST.cpp
    Cora/Parser/Parser.cpp
)

add_library(CoraParser STATIC ${CORA_PARSER_SOURCES})

target_include_directories(CoraParser
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(CoraParser PUBLIC cxx_std_17)
target_link_libraries(CoraParser PUBLIC CoraLexer CoraDiag)

if (MSVC)
    target_compile_options(CoraParser PRIVATE /W4 /permissive-)
else()
    target_compile_options(CoraParser PRIVATE -Wall -Wextra -Wpedantic)
endif()

set(CORA_LIBRARY_SOURCES
    Cora/AIR/AIR.cpp
    Cora/CIR/CIR.cpp
    Cora/CodeGen/CBackend.cpp
    Cora/Core/Digest.cpp
    Cora/Core/SHA256.cpp
    Cora/Driver/Driver.cpp
    # Cora/Frontend/Frontend.cpp
    Cora/Sema/BorrowChecker.cpp
    Cora/Sema/ComptimeEvaluator.cpp
    Cora/Sema/Monomorphizer.cpp
    Cora/Sema/NameResolver.cpp
    Cora/Sema/PassManager.cpp
    Cora/Sema/Sema.cpp
    Cora/Sema/SymbolTable.cpp
    Cora/Sema/TypeChecker.cpp
    Cora/Sema/Types.cpp
)

add_library(Cora STATIC ${CORA_LIBRARY_SOURCES})

target_include_directories(Cora
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(Cora PUBLIC cxx_std_17)
target_link_libraries(Cora PUBLIC CoraParser CoraDiag CoraLexer)

if (MSVC)
    target_compile_options(Cora PRIVATE /W4 /permissive-)
else()
    target_compile_options(Cora PRIVATE -Wall -Wextra -Wpedantic)
endif()
