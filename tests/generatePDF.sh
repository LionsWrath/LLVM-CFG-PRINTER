export LLVM_DIR=~/llvm/build/9.0.0

dots=($(find . -type f -name "*.dot"))

for ((i=0; i<${#dots[@]}; i++)) do
    file="${dots[${i}]%.*}"

    echo ${file}.dot . . .
    dot -Tpdf ${file}.dot -o ${file}.pdf
    #xdg-open ${file}.pdf
done
