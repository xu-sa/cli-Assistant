g++ -std=c++17 -static -DCURL_STATICLIB ^
-I./lib-w/curl/include ^
-I./test/lib test/main.cpp ^
-Wl,--start-group ^
./test/lib/libSLagent-w.a ^
./lib-w/curl/lib/libcurl.a ^
-Wl,--end-group -lws2_32 -lwldap32 -ladvapi32 -lcrypt32 -liphlpapi -lnormaliz -lbcrypt -lsecur32 ^
-o ./test/test_windows.exe
