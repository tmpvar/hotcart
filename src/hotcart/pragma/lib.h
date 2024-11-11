#pragma once
#include <string>
#include <variant>

#include <spdlog/spdlog.h>

struct HotcartPragmaLibInvalid {
  std::string message;
};

struct HotcartPragmaLibGithub {
  std::string user;
  std::string repo;
  std::string path;
  std::string sha;
};

struct HotcartPragmaLibVCPKG {
  std::string package;
  std::string version;
};

typedef std::
  variant<HotcartPragmaLibGithub, HotcartPragmaLibVCPKG, HotcartPragmaLibInvalid>
    HotcartPragmaLib;

static HotcartPragmaLib
HotcartParseLibPragmaGithub(const std::string &pragma) {
  HotcartPragmaLibGithub lib;
  const size_t charCount = pragma.length();
  size_t start = 0;
  int pathSegmentCount = 0;
  for (size_t charIndex = 0; charIndex < charCount; charIndex++) {
    char c = pragma[charIndex];
    switch (c) {
      case ':': {
        return HotcartPragmaLibInvalid{"Unexpected ':' in github pragma"};
      }

      case '/': {
        std::string value = pragma.substr(start, charIndex - start);

        if (pathSegmentCount == 0) {
          lib.user = value;
        } else if (pathSegmentCount == 1) {
          lib.repo = value;
        } else if (pathSegmentCount >= 2) {
          lib.path += value + "/";
        }

        pathSegmentCount++;

        start = charIndex + 1;
        break;
      }

      case '@': {
        std::string value = pragma.substr(start, charIndex - start);
        if(pathSegmentCount == 1) {
          lib.repo = value;
        } else if (pathSegmentCount >= 2) {
          lib.path += value;
        }

        std::string sha = pragma.substr(charIndex + 1, pragma.length() - charIndex - 1);
        lib.sha = sha;
        break;
      }
    }
  }

  return lib;
}

static HotcartPragmaLib
HotcartParseLibPragmaVCPKG(const std::string &pragma) {
  if (pragma.find(':') != std::string::npos) {
    return HotcartPragmaLibInvalid{"Unexpected ':' in vcpkg pragma"};
  }

  auto loc = pragma.find('@');
  if (loc == std::string::npos) {
    return HotcartPragmaLibInvalid{"No version in vcpkg pragma"};
  }

  if (loc == 0) {
    return HotcartPragmaLibInvalid{"No package in vcpkg pragma"};
  }

  return HotcartPragmaLibVCPKG{pragma.substr(0, loc), pragma.substr(loc + 1)};
}

static HotcartPragmaLib
HotcartParseLibPragma(const std::string &pragma) {
  if (!pragma.length()) {
    return HotcartPragmaLibInvalid{"Empty lib pragma"};
  }

  const size_t charCount = pragma.length();

  size_t start = 0;
  int pathSegmentCount = 0;
  for (size_t charIndex = 0; charIndex < charCount; charIndex++) {
    char c = pragma[charIndex];
    switch (c) {
      case ':': {
        start = charIndex + 1;
        std::string type = pragma.substr(0, charIndex);
        if (type == "github") {
          return HotcartParseLibPragmaGithub(pragma.substr(charIndex + 1));
        }

        if (type == "vcpkg") {
          return HotcartParseLibPragmaVCPKG(pragma.substr(charIndex + 1));
        }
        return HotcartPragmaLibInvalid{
          fmt::format("Unknown lib pragma source '{}'", type)};
      }
    }
  }

  return HotcartPragmaLibInvalid{"No source in lib pragma"};
}

#if defined(HOTCART_TEST) || defined(BUILT_WITH_CLANGD)
  #include <doctest/doctest.h>

// LCOV_EXCL_START
TEST_CASE("Parsing lib pragma: invalid") {
  // Empty
  {
    HotcartPragmaLib lib = HotcartParseLibPragma("");
    CHECK(std::holds_alternative<HotcartPragmaLibInvalid>(lib));
  }

  // Unknown source
  {
    HotcartPragmaLib lib = HotcartParseLibPragma("invalid:bla-blah");
    CHECK(std::holds_alternative<HotcartPragmaLibInvalid>(lib));
  }

  // Invalid format
  {
    HotcartPragmaLib lib = HotcartParseLibPragma("bla-blah");
    CHECK(std::holds_alternative<HotcartPragmaLibInvalid>(lib));
  }
}

TEST_CASE("Parsing lib pragma: 'github:'") {
  // Github
  {
    HotcartPragmaLib lib = HotcartParseLibPragma("github:user/project/path/to/dir@sha");
    const auto &github = std::get<HotcartPragmaLibGithub>(lib);
    CHECK(github.user == "user");
    CHECK(github.repo == "project");
    CHECK(github.path == "path/to/dir");
    CHECK(github.sha == "sha");
  }

  // Github no path
  {
    HotcartPragmaLib lib = HotcartParseLibPragma("github:user/repo@sha");
    const auto &github = std::get<HotcartPragmaLibGithub>(lib);
    CHECK(github.user == "user");
    CHECK(github.repo == "repo");
    CHECK(github.path == "");
    CHECK(github.sha == "sha");
  }

  // Github extra colon
  {
    HotcartPragmaLib lib = HotcartParseLibPragma(
      "github:github:user/project/path/to/dir@sha");
    CHECK(std::holds_alternative<HotcartPragmaLibInvalid>(lib));
  }
}

TEST_CASE("Parsing lib pragma: 'vcpkg:'") {
  // VCPKG
  {
    HotcartPragmaLib lib = HotcartParseLibPragma("vcpkg:package@version");
    const auto &vcpkg = std::get<HotcartPragmaLibVCPKG>(lib);
    CHECK(vcpkg.package == "package");
    CHECK(vcpkg.version == "version");
  }

  // VCPKG extra colon
  {
    HotcartPragmaLib lib = HotcartParseLibPragma("vcpkg:vcpkg:package@version");
    CHECK(std::holds_alternative<HotcartPragmaLibInvalid>(lib));
  }

  // VCPKG no package
  {
    HotcartPragmaLib lib = HotcartParseLibPragma("vcpkg:@version");
    CHECK(std::holds_alternative<HotcartPragmaLibInvalid>(lib));
  }

  // VCPKG no version
  {
    HotcartPragmaLib lib = HotcartParseLibPragma("vcpkg:package");
    CHECK(std::holds_alternative<HotcartPragmaLibInvalid>(lib));
  }
}
// LCOV_EXCL_STOP
#endif