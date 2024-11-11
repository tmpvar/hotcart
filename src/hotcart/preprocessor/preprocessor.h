#include <filesystem>

#include "../pragma/lib.h"
#include <simplecpp.h>
#include <spdlog/spdlog.h>
#include <vector>

namespace fs = std::filesystem;

struct HotcartPreprocessorError {
  std::string file;
  unsigned int line;
  std::string message;
};

struct HotcartPreprocessorResult {
  std::vector<HotcartPragmaLib> requestedLibs;
  std::vector<HotcartPreprocessorError> errors;
  std::vector<std::string> files;
};

static HotcartPreprocessorResult
HotcartPreprocessor(const fs::path &path) {
  // Perform preprocessing
  simplecpp::OutputList outputList;
  std::string filename = path.string();
  simplecpp::DUI dui;

  HotcartPreprocessorResult result = {};

  dui.pragmaTokenCallback = [&result](const simplecpp::Token *start,
                                      const simplecpp::Token *end) {
    if (!start || !end) {
      return;
    }

    const simplecpp::Token *cur = start;
    if (cur->str() == "hotcart") {
      spdlog::debug("process hotcart pragma");

      if (cur->next && cur->next->str() != "(") {
        result.errors.push_back({.file = cur->location.file(),
                                 .line = cur->location.line,
                                 .message = "missing pragma start '('"});
        return;
      }

      cur = cur->next;

      if (!cur->next) {
        result.errors.push_back({.file = cur->location.file(),
                                 .line = cur->location.line,
                                 .message = "unexpected end of hotcart pragma '('"});
        return;
      }
      cur = cur->next;

      while (cur && cur != end) {
        spdlog::debug("pragma token: '{}'", cur->str());
        // hotcart(lib, "github:user/repo@sha")
        // hotcart(lib, "github:user/repo/path/to@sha")
        // hotcart(lib, "vcpkg:package@version")
        if (cur->str() == "lib") {
          cur = cur->next;
          cur = cur->next;

          std::string def = cur->str();
          if (def[0] == '"' || def[0] == '\'') {
            def = def.substr(1, def.length() - 2);
          }

          HotcartPragmaLib lib = HotcartParseLibPragma(def);
          if (!std::holds_alternative<HotcartPragmaLibInvalid>(lib)) {
            result.requestedLibs.push_back(lib);
            return;
          }
        }

        result.errors.push_back(
          {.file = cur->location.file(),
           .line = cur->location.line,
           .message = fmt::format("invalid hotcart pragma '{}'", cur->str())});
        break;
      }

      result.errors.push_back({.file = cur->location.file(),
                               .line = cur->location.line,
                               .message = "missing pragma end ')'"});
    }
  };

  simplecpp::TokenList *rawtokens = new simplecpp::TokenList(filename,
                                                             result.files,
                                                             &outputList);
  rawtokens->removeComments();
  simplecpp::TokenList outputTokens(result.files);
  std::map<std::string, simplecpp::TokenList *> filedata;
  simplecpp::preprocess(outputTokens,
                        *rawtokens,
                        result.files,
                        filedata,
                        dui,
                        &outputList);

  simplecpp::cleanup(filedata);
  delete rawtokens;
  rawtokens = nullptr;

  return result;
}

#if defined(HOTCART_TEST) || defined(BUILT_WITH_CLANGD)
  #include <doctest/doctest.h>

// LCOV_EXCL_START
TEST_CASE("Preprocessor: valid") {
  HotcartPreprocessorResult result = HotcartPreprocessor(
    fs::path(__FILE__).remove_filename() / "fixtures" / "standalone.cpp");
  CHECK(result.requestedLibs.size() == 3);

  HotcartPragmaLibGithub github1 = std::get<HotcartPragmaLibGithub>(
    result.requestedLibs[0]);
  CHECK(github1.user == "user");
  CHECK(github1.repo == "repo");
  CHECK(github1.path == "");
  CHECK(github1.sha == "sha");

  HotcartPragmaLibGithub github2 = std::get<HotcartPragmaLibGithub>(
    result.requestedLibs[1]);
  CHECK(github2.user == "user2");
  CHECK(github2.repo == "cool-repo");
  CHECK(github2.path == "path/to/thing");
  CHECK(github2.sha == "v1.2.3");

  HotcartPragmaLibVCPKG vcpkg = std::get<HotcartPragmaLibVCPKG>(result.requestedLibs[2]);
  CHECK(vcpkg.package == "package");
  CHECK(vcpkg.version == "version");

  CHECK(result.errors.size() == 4);
  CHECK(result.errors[0].message == "missing pragma start '('");
  CHECK(result.errors[1].message == "invalid hotcart pragma 'invalid'");
  CHECK(result.errors[2].message == "missing pragma end ')'");
  CHECK(result.errors[3].message == "unexpected end of hotcart pragma '('");
}

// LCOV_EXCL_STOP
#endif