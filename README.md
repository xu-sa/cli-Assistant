# cli-Assistant
a ai tool Designed to help Improve Efficiency working on command line Interface

# Main Feature:
0:purely done in C++, Support windows and linux, nearly none cpu costs, totally 3Mb memory cost on Average

1:extension system Available , create a c++ function , and tell your AI agent to run it. 

2:work as both Server and client application

3:send Multiple types of file via termial 

# Prerequisite: nlohmann::json & curl

# How to use:
you can build the Library using ./build-lib.sh , result(.a and .h) would Appear in ./test/lib/

Or you can download the Release Library Compressed file to get the Archive file and the header file which tells you how to use.

once you have both files , here is a easy sample of how to use (you can build it via script ./build-test.sh): ./test/main.cpp

if you Decide to run it as a server or backgroudservice ,then './test/syagent_chat.py' would help you communicating with the server socket(you have to start server on socket 9995)
# Terminal Interface:
you can use and find Instruction about commands and configs once you have the sagent::interface() function running


