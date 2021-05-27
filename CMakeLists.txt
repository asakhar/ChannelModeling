cmake_minimum_required(VERSION 3.12)

project(ChannelModeling VERSION 2.3)

find_package(OpenCL REQUIRED)

include(FetchContent)

FetchContent_Declare(
  ldpc_codec
  GIT_REPOSITORY https://github.com/rinat229/LDPC_codes.git
  GIT_TAG main)
FetchContent_MakeAvailable(ldpc_codec)
FetchContent_GetProperties(ldpc_codec)
if(NOT ldpc_codec_POPULATED)
  FetchContent_Populate(ldpc_codec)
  add_subdirectory(${ldpc_codec_SOURCE_DIR})
endif()

FetchContent_Declare(
  bch_linux
  GIT_REPOSITORY https://github.com/mborgerding/bch_codec.git
  GIT_TAG master)
FetchContent_MakeAvailable(bch_linux)
FetchContent_GetProperties(bch_linux)
if(NOT bch_linux_POPULATED)
  FetchContent_Populate(bch_linux)
endif()

add_library(libbch_linux "${bch_linux_SOURCE_DIR}/bch_codec.c")
target_include_directories(libbch_linux INTERFACE ${bch_linux_SOURCE_DIR}
                                                  ${ldpc_codec_SOURCE_DIR})
add_library(BCH_Linux::BCH_Linux ALIAS libbch_linux)

add_library(default INTERFACE)
target_link_libraries(default INTERFACE BCH_Linux::BCH_Linux)
target_compile_options(default INTERFACE -O3)
target_compile_features(default INTERFACE cxx_std_20)
target_include_directories(default INTERFACE include)
target_precompile_headers(default INTERFACE pch.h)

add_executable(test1 Examples/test.cpp src/channel.cpp)
target_link_libraries(test1 default)

add_library(BasicModels src/channel.cpp src/BasicUnits/BCH.cxx)
target_compile_definitions(BasicModels PUBLIC NDEBUG)
target_link_libraries(BasicModels default)
add_library(BasicModels::BasicModels ALIAS BasicModels)

add_executable(Markov2 Examples/MarkovResearchSec.cpp)
target_link_libraries(Markov2 default)

add_executable(Test2 Examples/test2.cpp)
target_link_libraries(Test2 default)


include(FetchContent)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.7.3)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()


add_executable(LDPCTest test1.cpp)
target_link_libraries(LDPCTest PUBLIC default OpenCL::OpenCL nlohmann_json::nlohmann_json)

set(kernels KernelHS_naive.cl
            KernelHS_byindex.cl
            KernelVS_naive.cl
            KernelVS_byindex.cl
            KernelCheck_naive.cl
            KernelCheck_byindex.cl
            KernelLLR.cl)
foreach(kernel IN ITEMS ${kernels})
    file(COPY ${ldpc_codec_SOURCE_DIR}/cl/${kernel} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()