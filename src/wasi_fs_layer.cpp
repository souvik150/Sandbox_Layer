#include "wasi_fs_layer.h"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <iostream>

namespace WASI {

std::optional<FILE *> path_open_read(const Sandbox &sandbox, const std::string &relPath, Errno &error) {
  std::filesystem::path absPath = sandbox.resolve(relPath);
  if (!sandbox.isAllowed(absPath)) {
    error = Errno::NotAllowed;
    return std::nullopt;
  }
  FILE *fd = std::fopen(absPath.string().c_str(), "rb");
  if (!fd) {
    error = Errno::NotFound;
    return std::nullopt;
  }
  error = Errno::Success;
  return fd;
}

std::optional<FILE *> path_open_write(const Sandbox &sandbox, const std::string &relPath, Errno &error) {
  std::filesystem::path absPath = sandbox.resolve(relPath);
  if (!sandbox.isAllowed(absPath)) {
    error = Errno::NotAllowed;
    return std::nullopt;
  }
  
  FILE *fd = std::fopen(absPath.string().c_str(), "wb");
  if (!fd) {
    error = Errno::IOError;
    return std::nullopt;
  }
  error = Errno::Success;
  return fd;
}

Errno fd_close(FILE *fd) {
  if (std::fclose(fd) == 0) {
    return Errno::Success;
  }
  return Errno::IOError;
}

Errno read_file(const Sandbox &sandbox, const std::string &relPath, std::vector<uint8_t> &Data) {
  Errno err;
  auto fdOpt = path_open_read(sandbox, relPath, err);
  if (!fdOpt.has_value()) {
    return err;
  }
  FILE *fd = fdOpt.value();
  
  std::fseek(fd, 0, SEEK_END);
  long fileSize = std::ftell(fd);
  std::fseek(fd, 0, SEEK_SET);
  if (fileSize < 0) {
    fd_close(fd);
    return Errno::IOError;
  }
  Data.resize(static_cast<size_t>(fileSize));
  size_t readCount = std::fread(Data.data(), 1, Data.size(), fd);
  fd_close(fd);
  if (readCount != Data.size()) {
    return Errno::IOError;
  }
  return Errno::Success;
}

Errno write_file(const Sandbox &sandbox, const std::string &relPath, const std::vector<uint8_t> &Data) {
  Errno err;
  auto fdOpt = path_open_write(sandbox, relPath, err);
  if (!fdOpt.has_value()) {
    return err;
  }
  FILE *fd = fdOpt.value();
  size_t writeCount = std::fwrite(Data.data(), 1, Data.size(), fd);
  fd_close(fd);
  if (writeCount != Data.size()) {
    return Errno::IOError;
  }
  return Errno::Success;
}

} // namespace WASI
