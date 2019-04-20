# LLVM CFG PRINTER

## Information

* LLVM version: `9.0.0svn`;
* CMake version: `3.13.4`;
* Some classes like `ConstantExpr`, `ConstantAggregate` and `PHINode`are recursively built by using its own operands;
* I use the same labels assigned by the llvm on the Basic Blocks to identify each node;

## Compilation

1. `export LLVM_DIR=<path to llvm build>`
1. `cd <printer base dir>`
1. `mkdir build`
1. `cd build`
1. `cmake ..`
1. `make`

## Execution

`opt -load BytecodePrinter/LLVMBytecodePrinter.so -bytecode-printer < <filename>.bc`

The generated .dot files will be in the same directory that the command was run with the name `
<function name>.dot`.

### Execution option

If you want to add a suffix name in the name of the `.dot` output files, you can use the `-s <suffix>` option:

`opt -load BytecodePrinter/LLVMBytecodePrinter.so -bytecode-printer -s <suffix> < <filename>.bc`

The resulting files now will have the name `<suffix>_<function name>.dot`. 
