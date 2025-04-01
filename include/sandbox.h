#pragma once
#include <string>
#include <filesystem>

class Sandbox {
public:
  explicit Sandbox(const std::filesystem::path &root);

  const std::filesystem::path &root() const;

  bool isAllowed(const std::filesystem::path &path) const;

  std::filesystem::path resolve(const std::filesystem::path &relPath) const;

private:
  std::filesystem::path Root;
};
