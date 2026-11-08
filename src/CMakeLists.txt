set(CORA_LIBRARY_SOURCES
    Cora/AIR/AIR.cpp
    Cora/AST/AST.cpp
    Cora/Backend/CBackend.cpp
    Cora/Cache/CIRCache.cpp
    Cora/Cache/ContentAddressedStorage.cpp
    Cora/Cache/ObjectCache.cpp
    Cora/CIR/CIR.cpp
    Cora/CodeGen/ObjectInput.cpp
    Cora/Core/Digest.cpp
    Cora/Core/FileHash.cpp
    Cora/Core/SHA256.cpp
    Cora/Driver/BuildConfig.cpp
    Cora/Driver/CompilationSession.cpp
    Cora/Driver/Driver.cpp
    Cora/Lexer/Lexer.cpp
    Cora/Lexer/Token.cpp
    Cora/Link/Linker.cpp
    Cora/MIR/MIR.cpp
    Cora/Parse/Parser.cpp
)

add_library(Cora STATIC ${CORA_LIBRARY_SOURCES})

target_include_directories(Cora
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(Cora PUBLIC cxx_std_17)

if (MSVC)
    target_compile_options(Cora PRIVATE /W4 /permissive-)
else()
    target_compile_options(Cora PRIVATE -Wall -Wextra -Wpedantic)
endif()
