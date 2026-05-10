add_executable(syserver
    ${CMAKE_CURRENT_SOURCE_DIR}/server/main.cpp
)
target_link_libraries(syserver PRIVATE SYagent)
set_target_properties(syserver PROPERTIES
        OUTPUT_NAME "syagent"
        VERSION 1.0.8
        SOVERSION 1
)
target_compile_features(syserver PUBLIC cxx_std_17)

add_executable(schat
    ${CMAKE_CURRENT_SOURCE_DIR}/client/main.c
    ${LIB_DIR}/cJSON/cJSON.c
)
target_include_directories(schat PRIVATE ${LIB_DIR}/cJSON)
target_compile_features(schat PUBLIC c_std_99)
set_target_properties(schat PROPERTIES
        OUTPUT_NAME "schat"
        VERSION 1.0.8
        SOVERSION 1
)

set(INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../test)
if(WIN32)
add_custom_command(TARGET syserver POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:syserver>" 
        "${INSTALL_DIR}/syagent.exe"
    COMMENT "Copying executable to ${INSTALL_DIR}"
)
add_custom_command(TARGET schat POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:schat>" 
        "${INSTALL_DIR}/schat.exe"
    COMMENT "Copying executable to ${INSTALL_DIR}"
)

else()
add_custom_command(TARGET syserver POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:syserver>" 
        "${INSTALL_DIR}/syagent"
    COMMENT "Copying executable to ${INSTALL_DIR}"
)
add_custom_command(TARGET schat POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:schat>" 
        "${INSTALL_DIR}/schat"
    COMMENT "Copying executable to ${INSTALL_DIR}"
)
endif()