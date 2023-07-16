set(CURRENT_TARGET_NAME qutility_cml_opt)
add_library(${CURRENT_TARGET_NAME} INTERFACE)

find_package(Boost 1.61 COMPONENTS program_options json REQUIRED)

target_sources(${CURRENT_TARGET_NAME} INTERFACE cml_opt.h)

if(${Boost_FOUND})
    message(STATUS "Boost with version>=1.61 is found. cml_opt helper will be enabled")
    target_link_libraries(${CURRENT_TARGET_NAME} INTERFACE
        Boost::headers
        Boost::json
        Boost::program_options)
else()
    message(STATUS "Boost with version>=1.61 is not found.  cml_opt helper will be disabled")
    target_compile_definitions(${CURRENT_TARGET_NAME} INTERFACE QUTILITY_CML_OPT_DISABLE_CML_OPT)
endif()
