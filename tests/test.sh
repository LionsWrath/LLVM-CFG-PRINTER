export LLVM_DIR=~/llvm/build/9.0.0

tests=('Bubblesort' 'IntMM' 'Oscar' 'Perm' 'Puzzle' 'Queens' 'Quicksort' 'RealMM' 'Towers' 'Treesort')

for ((i=0; i<${#tests[@]}; i++)) do
    file=${tests[${i}]}

    opt -load ../build/BitcodePrinter/LLVMBitcodePrinter.so -bitcode-printer -s ${file} < ${file}.bc 
done
