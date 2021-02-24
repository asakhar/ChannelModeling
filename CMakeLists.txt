cmake_minimum_required(VERSION 3.12)

project(ChannelModeling VERSION 0.1)

add_library(default INTERFACE)
target_compile_options(default INTERFACE -O3)
target_compile_features(default INTERFACE cxx_std_20)
target_include_directories(default INTERFACE include)


add_executable(test1 test.cpp src/channel.cxx)
target_link_libraries(test1 default)

add_library(BasicModels src/channel.cxx)
target_compile_definitions(BasicModels PUBLIC NDEBUG)
target_link_libraries(BasicModels default)

