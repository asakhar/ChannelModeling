cmake_minimum_required(VERSION 3.12)

project(ChannelModeling VERSION 0.1)


add_executable(test test.cpp)
target_compile_features(test PUBLIC cxx_std_20)
