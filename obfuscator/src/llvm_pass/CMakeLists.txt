# LLVM Pass 编译配置

if(LLVM_FOUND)
    # 添加LLVM定义
    add_definitions(-DHAVE_LLVM)

    # LLVM Pass库
    add_library(ObfuscationPass MODULE
        ObfuscationPass.cpp
    )

    # 链接LLVM库
    llvm_map_components_to_libnames(llvm_libs core support)
    target_link_libraries(ObfuscationPass ${llvm_libs})

    # 设置输出目录
    set_target_properties(ObfuscationPass PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )

    message(STATUS "LLVM Pass will be built")

    # 安装规则
    install(TARGETS ObfuscationPass
        LIBRARY DESTINATION lib/obfuscator
    )
else()
    message(WARNING "LLVM not found, LLVM Pass will not be built")
endif()
