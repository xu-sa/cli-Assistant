set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../lib)
add_library(SYagent STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/file/util_parse_file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/main/utils_1.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/net/util_net.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/agent/agent_0.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/agent/agent_1.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/agent/agent_2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/agent/agent_3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/agent/agent_4.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/api/interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/local_tool/cli_tool.cpp
)
target_precompile_headers(SYagent PRIVATE
    <string>
    <vector>
)
target_compile_features(SYagent PUBLIC cxx_std_17)

if(WIN32)
    target_compile_definitions(SYagent PUBLIC CURL_STATICLIB)
    target_include_directories(SYagent PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/agent
        ${CMAKE_CURRENT_SOURCE_DIR}/api
        ${CMAKE_CURRENT_SOURCE_DIR}/local_tool
        ${LIB_DIR}/json
        ${LIB_DIR}/Wcurl/include
    )
    # find_library(CURL_LIBRARIES NAMES curl libcurl PATHS ${LIB_DIR}/Wcurl/lib NO_DEFAULT_PATH)
    target_link_libraries(SYagent PRIVATE
            ${LIB_DIR}/Wcurl/lib/libcurl.a
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
        OUTPUT_NAME "sagentw"
        VERSION 1.0.5
        SOVERSION 1
    )
else()
    target_compile_definitions(SYagent PUBLIC CURL_STATICLIB)
    target_include_directories(SYagent PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/data
            ${CMAKE_CURRENT_SOURCE_DIR}/utils
            ${CMAKE_CURRENT_SOURCE_DIR}/agent
            ${CMAKE_CURRENT_SOURCE_DIR}/api
            ${CMAKE_CURRENT_SOURCE_DIR}/local_tool
            ${LIB_DIR}/json
            ${LIB_DIR}/Lcurl/include
        )
    # find_library(CURL_LIBRARIES NAMES curl libcurl PATHS ${LIB_DIR}/Lcurl/lib NO_DEFAULT_PATH)
    # find_library(MBEDTLS_LIBRARIES NAMES mbedtls libmbedtls PATHS ${LIB_DIR}/Lmbedtls/lib NO_DEFAULT_PATH)
    # find_library(MBEDX509_LIBRARIES NAMES mbedx509 libmbedx509 PATHS ${LIB_DIR}/Lmbedtls/lib NO_DEFAULT_PATH)
    # find_library(MBEDCRYPTO_LIBRARIES NAMES mbedcrypto libmbedcrypto PATHS ${LIB_DIR}/Lmbedtls/lib NO_DEFAULT_PATH)
    # find_library(ZLIB_LIBRARIES NAMES z libz PATHS ${LIB_DIR}/Lzlib/lib NO_DEFAULT_PATH)

    target_link_libraries(SYagent PRIVATE
         ${LIB_DIR}/Lcurl/lib/libcurl.a
        ${LIB_DIR}/Lmbedtls/lib/libmbedtls.a
        ${LIB_DIR}/Lmbedtls/lib/libmbedx509.a
        ${LIB_DIR}/Lmbedtls/lib/libmbedcrypto.a
        ${LIB_DIR}/Lzlib/lib/libz.a
        pthread
        dl
        rt
    )
    set_target_properties(SYagent PROPERTIES
        OUTPUT_NAME "sagentl"
        VERSION 1.0.5
        SOVERSION 1
    )
endif()
add_executable(sytest
    ${CMAKE_CURRENT_SOURCE_DIR}/../test/main.cpp
)
target_link_libraries(sytest PRIVATE SYagent)
set_target_properties(sytest PROPERTIES
        OUTPUT_NAME "sytest"
        VERSION 1.0.5
        SOVERSION 1
    )
set(INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../test/bin)

if(WIN32)
add_custom_command(TARGET sytest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:sytest>" 
        "${INSTALL_DIR}/syagent.exe"
    COMMENT "Copying executable to ${INSTALL_DIR}"
)
else()
add_custom_command(TARGET sytest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${INSTALL_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:sytest>" 
        "${INSTALL_DIR}/syagent"
    COMMENT "Copying executable to ${INSTALL_DIR}"
)
endif()
