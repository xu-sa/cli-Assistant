
find_package(CURL REQUIRED)

if(CURL_FOUND)
    find_library(CURL_STATIC_LIB NAMES libcurl.a curl PATHS /usr/local/lib /usr/lib)
    if(CURL_STATIC_LIB)
        message(STATUS "Found static curl: ${CURL_STATIC_LIB}")
        set(CURL_LIBRARIES ${CURL_STATIC_LIB})
    else()
        message(WARNING "Static curl not found, using default")
    endif()
endif()

add_library(SYagent STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/agent/interact.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/agent/SYagent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/agent/agent_web.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/api/interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/local_tool/cli_tool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mannager/smannager.cpp
)

target_precompile_headers(SYagent PRIVATE
    "json/json.hpp"
    <string>
    <vector>
    <curl/curl.h>
)


target_include_directories(SYagent PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/agent
    ${CMAKE_CURRENT_SOURCE_DIR}/api
    ${CMAKE_CURRENT_SOURCE_DIR}/json
    ${CMAKE_CURRENT_SOURCE_DIR}/local_tool
    ${CMAKE_CURRENT_SOURCE_DIR}/mannager
)

target_compile_features(SYagent PUBLIC cxx_std_17)
target_link_libraries(SYagent PRIVATE ${CURL_LIBRARIES})


set_target_properties(SYagent PROPERTIES
    OUTPUT_NAME "SLagent"
    VERSION 1.0.5
    SOVERSION 1
)
set(INSTALL_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../test/release/lib)

add_custom_command(TARGET SYagent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_LIB_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SYagent>" 
        "${INSTALL_LIB_DIR}"
    COMMENT "Copying header to ${INSTALL_LIB_DIR} \n"   
)
add_custom_command(TARGET SYagent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_LIB_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/api/syagent_interface.h"
            "${INSTALL_LIB_DIR}"
    COMMENT "Copying library to ${INSTALL_LIB_DIR} \n" 
)

