export LLVM_DIR=~/llvm/build/9.0.0

tests=('Bubblesort' 'IntMM' 'Oscar' 'Perm' 'Puzzle' 'Queens' 'Quicksort' 'RealMM' 'Towers' 'Treesort' 'switch')

for ((i=0; i<${#tests[@]}; i++)) do
    file=${tests[${i}]}

    opt -mem2reg ${file}.bc -o ${file}.ssa.bc
    opt -load ../build/BitcodePrinter/LLVMBitcodePrinter.so -bitcode-printer -s ${file} < ${file}.ssa.bc 
done
