set(CURRENT_TARGET_NAME qutility_array_wrapper_cpu)

add_library(${CURRENT_TARGET_NAME} INTERFACE)

include(CheckLibraryExists)
CHECK_LIBRARY_EXISTS("memkind" "hbw_posix_memalign" "" memkind_FOUND)

target_link_libraries(${CURRENT_TARGET_NAME} INTERFACE Boost::headers)

target_sources(${PROJECT_NAME}
    INTERFACE
    base.h
    ddr.h
    hbw.h
    hbw_posix_allocator.h
)

if(${memkind_FOUND})
    message(STATUS "memkind is found. DArrayHBW and ArrayHBW classes will link to memkind.")
    target_link_libraries(${CURRENT_TARGET_NAME} INTERFACE memkind)
else()
    message(STATUS "memkind is not found. DArrayHBW and ArrayHBW classes will behave the same as DArrayDDR and ArrayDDR classes.")
    target_compile_definitions(${CURRENT_TARGET_NAME} INTERFACE QUTILITY_ARRAY_WRAPPER_CPU_DISABLE_MEMKIND)
    target_sources(${PROJECT_NAME}
        INTERFACE
        hbw_debug.h
    )
endif()