#include <iostream>
#include "sandbox.h"
#include "wasi_fs_layer.h"
#include <vector>
#include <string>

int main() {
  try {
    std::filesystem::path sandboxRoot = std::filesystem::current_path() / "sandbox_dir";
    Sandbox sandbox(sandboxRoot);

    std::cout << "Sandbox root: " << sandbox.root() << std::endl;

    std::string filename = "sample.txt";
    std::string sampleText = "Hello, WASI sandbox!";
    std::vector<uint8_t> writeData(sampleText.begin(), sampleText.end());

    WASI::Errno err = WASI::write_file(sandbox, filename, writeData);
    if (err != WASI::Errno::Success) {
      std::cerr << "Error writing file. Errno: " << static_cast<uint32_t>(err) << std::endl;
      return 1;
    }
    std::cout << "File written successfully: " << filename << std::endl;

    std::vector<uint8_t> readData;
    err = WASI::read_file(sandbox, filename, readData);
    if (err != WASI::Errno::Success) {
      std::cerr << "Error reading file. Errno: " << static_cast<uint32_t>(err) << std::endl;
      return 1;
    }

    std::string content(readData.begin(), readData.end());
    std::cout << "Read file content: " << content << std::endl;

    std::string outside = "../outside.txt";
    readData.clear();
    err = WASI::read_file(sandbox, outside, readData);
    if (err != WASI::Errno::Success) {
      std::cout << "Access to outside file blocked as expected. Errno: " << static_cast<uint32_t>(err) << std::endl;
    } else {
      std::cerr << "Error: Was able to read a file outside the sandbox!" << std::endl;
      return 1;
    }

  } catch (const std::exception &ex) {
    std::cerr << "Exception: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
