 #!/bin/bash 
mkdir ./test/bin
g++ -std=c++17 -DCURL_STATICLIB \
    -I./test/lib \
    ./test/main.cpp \
    -Wl,--start-group \
    ./test/lib/libSLagent-l.a \
    ./lib-l/curl/lib/libcurl.a \
    ./lib-l/mbedtls/lib/libmbedtls.a \
    ./lib-l/mbedtls/lib/libmbedx509.a \
    ./lib-l/mbedtls/lib/libmbedcrypto.a \
    ./lib-l/zlib/lib/libz.a \
    -Wl,--end-group \
    -lpthread -ldl -lrt \
    -o ./test/bin/syagent
