 #!/bin/bash
g++ -std=c++17 -DCURL_STATICLIB \
    -I./lib-l/curl/include \
    -I./lib-l/json \
    -I./lib-l/mbedtls/include/everest/kremlib \
    -I./lib-l/mbedtls/include/everest/kremlin \
    -I./llib-lib/mbedtls/include/everest/vs2010 \
    -I./llib-lib/mbedtls/include/everest \
    -I./lib-l/mbedtls/include/mbedtls \
    -I./lib-l/mbedtls/include/psa \
    -I./lib-l/zlib/include \
    -I./test/lib \
    test/main.cpp \
    -Wl,--start-group \
    ./test/lib/libSLagent-l.a \
    ./lib-l/curl/lib/libcurl.a \
    ./lib-l/mbedtls/lib/libmbedtls.a \
    ./lib-l/mbedtls/lib/libmbedx509.a \
    ./lib-l/mbedtls/lib/libmbedcrypto.a \
    ./lib-l/zlib/lib/libz.a \
    -Wl,--end-group \
    -lpthread -ldl -lrt \
    -o ./test/bin/test_linux