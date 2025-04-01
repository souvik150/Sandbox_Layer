#include "sandbox.h"
#include <stdexcept>

Sandbox::Sandbox(const std::filesystem::path &root) : Root(root) {
  if (!std::filesystem::exists(Root) || !std::filesystem::is_directory(Root)) {
    throw std::runtime_error("Sandbox root does not exist or is not a directory");
  }
  Root = std::filesystem::canonical(Root);
}

const std::filesystem::path &Sandbox::root() const {
  return Root;
}

bool Sandbox::isAllowed(const std::filesystem::path &path) const {
  try {
    auto normPath = std::filesystem::absolute(path).lexically_normal();
    
    return std::mismatch(Root.begin(), Root.end(), normPath.begin()).first == Root.end();
  } catch (...) {
    return false;
  }
}

std::filesystem::path Sandbox::resolve(const std::filesystem::path &relPath) const {
  auto absPath = Root / relPath;
  return std::filesystem::absolute(absPath).lexically_normal();
}
