#include "./src/BrainfrickLLVM.h"

int main(int argc, char **argv) {
  BrainfrickLLVM vm;
  vm.exec(argv[2]);
}
