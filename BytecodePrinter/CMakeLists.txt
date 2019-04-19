add_llvm_library( LLVMBytecodePrinter MODULE
    BytecodePrinter.cpp

    PLUGIN_TOOL
    opt
)
