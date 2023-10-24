# brainfrick-llvm

Yet another brainfrick compiler. 

# Example

```
+[-->-[>>+>-----<<]<--<---]>-.>>>+.>>..+++[.>]<<<<.+++.------.<<-.>>>>+.
```

```
Hello, World!
```

# Usage
Requires LLVM 18.0.0 or above.
```
clang++ `llvm-config --cxxflags --ldflags --system-libs --libs core` main.cpp <bf_file>
lli ./out.ll
```