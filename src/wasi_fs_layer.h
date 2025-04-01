#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include "sandbox.h"

namespace WASI {
enum class Errno : uint32_t {
  Success = 0,
  Fault = 21,
  NotAllowed = 77,  // Custom: operation not allowed by sandbox.
  NotFound = 44,    // Custom: file not found.
  IOError = 29,     // Generic I/O error.
};

std::optional<FILE *> path_open_read(const Sandbox &sandbox, const std::string &relPath, Errno &error);

std::optional<FILE *> path_open_write(const Sandbox &sandbox, const std::string &relPath, Errno &error);

Errno read_file(const Sandbox &sandbox, const std::string &relPath, std::vector<uint8_t> &Data);

Errno write_file(const Sandbox &sandbox, const std::string &relPath, const std::vector<uint8_t> &Data);

Errno fd_close(FILE *fd);

} // namespace WASI
