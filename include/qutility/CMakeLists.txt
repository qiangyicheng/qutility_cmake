add_subdirectory(message)
add_subdirectory(array_wrapper)
target_link_libraries(${PROJECT_NAME} INTERFACE qutility_array_wrapper)

target_sources(${PROJECT_NAME}
    INTERFACE
    array_wrapper.h
    c_array.h
    crtp_helper.h
    getopt.h
    history.h
    ifmember.h
    matio.h
    message.h
    qutility.h
    traits.h
    box.h
)