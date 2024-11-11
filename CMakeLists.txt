cmake_minimum_required(VERSION 3.16)
project(sdl-imwindow)

include(FetchContent)

option(HOTCART_ENABLE_TESTS "Enable tests" ON)
option(HOTCART_TEST_COVERAGE_GCOV "Enable GCOV test coverage" ON)

# from: https://discourse.nixos.org/t/get-clangd-to-find-standard-headers-in-nix-shell/11268/10
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

set(SRC ${CMAKE_CURRENT_LIST_DIR}/src)
set(DEPS ${CMAKE_CURRENT_LIST_DIR}/deps)

# spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG        51a0deca2c825f1d4461655a18bb37d6df76646d
)
FetchContent_MakeAvailable(spdlog)

# SimpleCPP
FetchContent_Declare(
  simplecpp
  GIT_REPOSITORY https://github.com/tmpvar/simplecpp.git
  # the tmpvar/pragma-callback branch
  GIT_TAG        2b3c8d5c0317de21c89027eb2057a14e9456d5d9
)
FetchContent_Populate(simplecpp)
add_library(simplecpp STATIC ${simplecpp_SOURCE_DIR}/simplecpp.cpp)
target_include_directories(simplecpp PUBLIC
  ${simplecpp_SOURCE_DIR}
)
add_library(simplecpp::simplecpp ALIAS simplecpp)

# DocOpt
FetchContent_Declare(
  docopt
  GIT_REPOSITORY https://github.com/docopt/docopt.cpp
  GIT_TAG        400e6dd8e59196c914dcc2c56caf7dae7efa5eb3
)
FetchContent_Populate(docopt)

add_library(docopt STATIC ${docopt_SOURCE_DIR}/docopt.cpp)
target_include_directories(docopt PUBLIC
  ${simplecpp_SOURCE_DIR}
)
add_library(docopt::docopt ALIAS docopt)

# EFSW
FetchContent_Declare(
  efsw
  GIT_REPOSITORY https://github.com/SpartanJ/efsw.git
  GIT_TAG        62f785c56b7a34f035193d4cb831921347b586b8
)
FetchContent_MakeAvailable(efsw)

# Executables
add_executable(hotcart
 ${SRC}/hotcart/cli.cpp
)
target_include_directories(hotcart PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(hotcart PRIVATE
  spdlog
  simplecpp::simplecpp
  docopt
  efsw
)

if (HOTCART_ENABLE_TESTS)
  add_executable(hotcart-tests ${SRC}/tests.cpp)
  target_include_directories(hotcart-tests PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
  )

  target_compile_definitions(hotcart-tests PRIVATE -DHOTCART_TEST)

  if (HOTCART_TEST_COVERAGE_GCOV)
    target_compile_options(hotcart-tests PRIVATE -g -O1 --coverage -fprofile-abs-path)
    target_link_options(hotcart-tests PRIVATE --coverage)
  endif()

  target_link_libraries(hotcart-tests PRIVATE
    spdlog
    simplecpp::simplecpp
    docopt::docopt
  )
endif()
