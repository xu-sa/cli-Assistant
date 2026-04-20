 #!/bin/bash 
mkdir -p ./test/bin
g++ -std=c++17 -DCURL_STATICLIB \
    -I./test/lib \
    ./test/main.cpp \
    -Wl,--start-group \
    ./test/lib/libsagentl.a \
    ./lib/Lcurl/lib/libcurl.a \
    ./lib/Lmbedtls/lib/libmbedtls.a \
    ./lib/Lmbedtls/lib/libmbedx509.a \
    ./lib/Lmbedtls/lib/libmbedcrypto.a \
    ./lib/Lzlib/lib/libz.a \
    -Wl,--end-group \
    -lpthread -ldl -lrt \
    -o ./test/bin/syagent
