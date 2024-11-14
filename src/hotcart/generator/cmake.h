// Generate CMakeLists.txt for Cartridges

#include <spdlog/spdlog.h>

#include "../common.h"
#include <filesystem>
#include <spdlog/fmt/bundled/format.h>
#include <vector>

namespace fs = std::filesystem;

static std::string
GenerateCartridgeCMakeLists(const std::vector<RequestedLibGithub> &libs,
                            const fs::path &sourcePath) {
  spdlog::info("Generating CMakeLists.txt for cartridge");

  std::string cmakeLists = "cmake_minimum_required(VERSION 3.16)\n"
                           "project(cartridge)\n"
                           "include(FetchContent)\n";

  const std::string
    FetchContent = "FetchContent_Declare(\n"
                   "  {user}-{repo}\n"
                   "  GIT_REPOSITORY https://github.com/{user}/{repoAndPath}\n"
                   "  GIT_TAG {sha}\n"
                   "  GIT_SUBMODULES_RECURSE true\n"
                   "  GIT_SHALLOW true\n"
                   "  GIT_PROGRESS true\n"
                   ")\n"
                   "FetchContent_MakeAvailable({user}-{repo})\n";

  for (const auto &lib : libs) {
    std::string repoAndPath = lib.repo;
    if (lib.path != "") {
      repoAndPath += "/" + lib.path;
    }

    std::string tmp = fmt::format(FetchContent,
                                  fmt::arg("user", lib.user),
                                  fmt::arg("repo", lib.repo),
                                  fmt::arg("repoAndPath", repoAndPath),
                                  fmt::arg("sha", lib.sha));
    cmakeLists += tmp;
  }

  cmakeLists += fmt::format("add_library(cartridge SHARED {})\n", sourcePath.string());

  if (libs.size() > 0) {
    cmakeLists += "target_link_libraries(cartridge PRIVATE\n";
    for (const auto &lib : libs) {
      cmakeLists += fmt::format("  {}-{}\n", lib.user, lib.repo);
    }
    cmakeLists += ")\n";
  }

  return cmakeLists;
}

#if defined(HOTCART_TEST) || defined(BUILT_WITH_CLANGD)
  #include <doctest/doctest.h>

// LCOV_EXCL_START
TEST_CASE("CMake: generate cartridge CMakelists") {

  // No libs
  {
    const std::string expect = "cmake_minimum_required(VERSION 3.16)\n"
                               "project(cartridge)\n"
                               "include(FetchContent)\n"
                               "add_library(cartridge SHARED source.cpp)\n";
    const std::string result = GenerateCartridgeCMakeLists({}, "source.cpp");
    CHECK(result == expect);
  }

  // With libs
  {
    const std::vector<RequestedLibGithub> libs = {
      {.user = "user", .repo = "repo", .path = "path/to/dir", .sha = "sha"},
    };
    const std::string result = GenerateCartridgeCMakeLists(libs, "source.cpp");

    const std::string
      expect = "cmake_minimum_required(VERSION 3.16)\n"
               "project(cartridge)\n"
               "include(FetchContent)\n"
               "FetchContent_Declare(\n"
               "  user-repo\n"
               "  GIT_REPOSITORY https://github.com/user/repo/path/to/dir\n"
               "  GIT_TAG sha\n"
               "  GIT_SUBMODULES_RECURSE true\n"
               "  GIT_SHALLOW true\n"
               "  GIT_PROGRESS true\n"
               ")\n"
               "FetchContent_MakeAvailable(user-repo)\n"
               "add_library(cartridge SHARED source.cpp)\n"
               "target_link_libraries(cartridge PRIVATE\n"
               "  user-repo\n"
               ")\n";
    CHECK(result == expect);
  }
}
// LCOV_EXCL_STOP
#endif