add_executable(cora main.cpp)
target_link_libraries(cora PRIVATE Cora)
target_compile_features(cora PRIVATE cxx_std_17)
