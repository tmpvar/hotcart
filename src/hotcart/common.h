#pragma once

#include <string>

struct RequestedLibGithub {
  std::string user;
  std::string repo;
  std::string path;
  std::string ref;
};