# LLVM CFG PRINTER

## Information

* LLVM version: `9.0.0svn`;
* CMake version: `3.13.4`;
* Some classes like `ConstantExpr`, `ConstantAggregate` and `PHINode` are recursively built by using its own operands;
* I use the same labels assigned by the llvm on the Basic Blocks to identify each node;

## Compilation

### Compilation outside the LLVM

1. `export LLVM_DIR=<path to llvm build>`
1. `cd <printer base dir>`
1. `mkdir build`
1. `cd build`
1. `cmake ..`
1. `make`

### Compilation inside the LLVM

To add the Pass to the llvm compilation tree:

1. `cp BitcodePrinter <path to llvm source>/llvm/lib/Transforms/`
1. Add `add_subdirectory(BitcodePrinter)` to `<path to llvm source>/llvm/lib/Transforms/CMakeLists.txt`

Then, we can compile the llvm normally (now with our pass included):

1. `cd <path to llvm source>`
1. `mkdir build`
1. `cd build`
1. `cmake .. -G "Unix Makefiles"`
1. `make`

## Execution

### Outside the LLVM compilation

`opt -load BitcodePrinter/LLVMBitcodePrinter.so -bitcode-printer < <filename>.bc`

### Inside the LLVM compilation

`opt -bytecode-printer < <filename>.bc`

### Execution option

The generated .dot files will be in the same directory that the command was run with the name `
<function name>.dot`. If you want to add a suffix name in the name of the `.dot` output files, you can use the `-s <suffix>` option:

`opt -load BitcodePrinter/LLVMBitcodePrinter.so -bitcode-printer -s <suffix> < <filename>.bc`

The resulting files now will have the name `<suffix>_<function name>.dot`. 
