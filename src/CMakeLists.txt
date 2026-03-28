

if(WIN32)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../lib-w)
else()
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../lib-l)
endif()
find_library(CURL_STATIC_LIB NAMES curl libcurl PATHS ${LIB_DIR}/curl/lib NO_DEFAULT_PATH)

if(CURL_STATIC_LIB)
    message(STATUS "Found static curl: ${CURL_STATIC_LIB}")
    if(WIN32)
        # find_library(WS2_32 NAMES ws2_32)
        # find_library(WLDAP32 NAMES wldap32)
        # find_library(ADVAPI32 NAMES advapi32)
        # find_library(CRYPT32 NAMES crypt32)
        # find_library(IPHLPAPI NAMES iphlpapi)
        # find_library(NORMALIZ NAMES normaliz)
        set(CURL_LIBRARIES
            ${CURL_STATIC_LIB}
            # ${MBEDTLS_LIB}
            # ${MBEDCRYPTO_LIB}
            # ${MBEDX509_LIB}
            # ${ZLIB_STATIC}
            # ${WS2_32}
            # ${WLDAP32}
            # ${ADVAPI32}
            # ${CRYPT32}
            # ${IPHLPAPI}
            # ${NORMALIZ}
        )
    else()
        # find_library(MBEDTLS_LIB NAMES mbedtls PATHS ${LIB_DIR}/mbedtls/lib NO_DEFAULT_PATH)
        # find_library(MBEDCRYPTO_LIB NAMES mbedcrypto PATHS ${LIB_DIR}/mbedtls/lib NO_DEFAULT_PATH)
        # find_library(MBEDX509_LIB NAMES mbedx509 PATHS ${LIB_DIR}/mbedtls/lib NO_DEFAULT_PATH)
        # find_library(ZLIB_STATIC NAMES z zlibstatic zlib PATHS ${LIB_DIR}/zlib/lib NO_DEFAULT_PATH)

        find_library(PTHREAD_LIB NAMES pthread)
        find_library(DL_LIB NAMES dl)
        find_library(RT_LIB NAMES rt) 
        set(CURL_LIBRARIES
            
            ${CURL_STATIC_LIB}
            # ${MBEDTLS_LIB}
            # ${MBEDCRYPTO_LIB}
            # ${MBEDX509_LIB}
            # ${ZLIB_STATIC}
            ${PTHREAD_LIB}#multi threading
            ${DL_LIB}#dynamic loading
            ${RT_LIB}#real time
        )
    endif()
    add_definitions(-DCURL_STATICLIB)
else()
    message(FATAL_ERROR "Static curl library not found in ${LIB_DIR}/curl/lib")
endif()

add_library(SYagent STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/file/util_parse_file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/json/utils_json.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/net/util_net.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/agent/agent_1.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/agent/agent_2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/agent/agent_3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/agent/agent_web.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/api/interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/local_tool/cli_tool.cpp
)

target_include_directories(SYagent PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/agent
    ${CMAKE_CURRENT_SOURCE_DIR}/api
    ${CMAKE_CURRENT_SOURCE_DIR}/local_tool
)

if(WIN32)
    target_include_directories(SYagent PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib-w/json
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib-w/curl/include
    )
else()
    target_include_directories(SYagent PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib-l/json
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib-l/curl/include
    )
endif()

target_precompile_headers(SYagent PRIVATE
    <string>
    <vector>
)

target_compile_features(SYagent PUBLIC cxx_std_17)

target_link_libraries(SYagent PRIVATE ${CURL_LIBRARIES})

if(WIN32)
target_link_libraries(SYagent PRIVATE 
        ws2_32#provide windows socket
        wldap32#provide ldap
        advapi32#provide advanced api
        crypt32#provide crypto
        iphlpapi#provide ip helper api
        normaliz#provide normalization api
        bcrypt#provide bcrypt for random number generation
        secur32#provide secur32 for SSPI
)
set_target_properties(SYagent PROPERTIES
    OUTPUT_NAME "SLagent-w"
    VERSION 1.0.5
    SOVERSION 1
)
else()
set_target_properties(SYagent PROPERTIES
    OUTPUT_NAME "SLagent-l"
    VERSION 1.0.5
    SOVERSION 1
)
endif()



set(INSTALL_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../test/lib)

add_custom_command(TARGET SYagent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_LIB_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SYagent>" 
        "${INSTALL_LIB_DIR}"
    COMMENT "Copying library to ${INSTALL_LIB_DIR}"
)

add_custom_command(TARGET SYagent POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/api/syagent_interface.h"
        "${INSTALL_LIB_DIR}"
    COMMENT "Copying header to ${INSTALL_LIB_DIR}"
)