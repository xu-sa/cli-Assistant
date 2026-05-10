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
    target_include_directories(SYagent PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/data
            ${CMAKE_CURRENT_SOURCE_DIR}/utils
            ${CMAKE_CURRENT_SOURCE_DIR}/agent
            ${CMAKE_CURRENT_SOURCE_DIR}/api
            ${CMAKE_CURRENT_SOURCE_DIR}/local_tool
            ${LIB_DIR}/json
            ${LIB_DIR}/Lcurl/include
    )
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
        VERSION 1.0.6
        SOVERSION 1
    )
endif()

