if(WIN32 OR CYGWIN)
    set(LLVM_LINK_COMPONENTS Core Support)
endif()

add_llvm_library( LLVMBitcodePrinter MODULE
    BitcodePrinter.cpp

    DEPENDS
    intrinsics_gen

    PLUGIN_TOOL
    opt
)
