cmake_minimum_required(VERSION 3.12)

project(ChannelModeling VERSION 2.3)

add_library(default INTERFACE)
target_compile_options(default INTERFACE -O3)
target_compile_features(default INTERFACE cxx_std_20)
target_include_directories(default INTERFACE include)
target_precompile_headers(default INTERFACE pch.h)


add_executable(test1 Examples/test.cpp src/channel.cpp)
target_link_libraries(test1 default)

add_library(BasicModels src/channel.cpp)
target_compile_definitions(BasicModels PUBLIC NDEBUG)
target_link_libraries(BasicModels default)

add_executable(Markov2 Examples/MarkovResearchSec.cpp)
target_link_libraries(Markov2 default)

add_executable(Test2 Examples/test2.cpp)
target_link_libraries(Test2 default)