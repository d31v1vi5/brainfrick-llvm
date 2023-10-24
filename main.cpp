#include "./src/BrainfrickLLVM.h"

int main(int argc, char **argv) {
  std::string program = R"(42)";
  BrainfrickLLVM vm;
  vm.exec(argv[2]);
}