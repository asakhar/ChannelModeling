cmake_minimum_required(VERSION 3.12)

project(ChannelModeling VERSION 0.1)

add_compile_options(-O3)

include_directories(include)

add_executable(test1 test.cpp src/channel.cxx)
target_compile_features(test1 PUBLIC cxx_std_20)

add_library(BasicModels src/channel.cxx)
target_compile_definitions(BasicModels PUBLIC NDEBUG)
target_compile_features(BasicModels PUBLIC cxx_std_20)

