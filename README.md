# brainfrick-llvm

Yet another brainfrick compiler. 

# Usage
Requires LLVM 18.0.0 or above.
```
clang++ `llvm-config --cxxflags --ldflags --system-libs --libs core` main.cpp <bffile.>
lli ./out.ll
```