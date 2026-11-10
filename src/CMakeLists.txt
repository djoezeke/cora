set(CORA_LIBRARY_SOURCES
    Cora/AIR/AIR.cpp
    Cora/AST/AST.cpp
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
