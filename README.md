# cli-Assistant
a ai tool Designed to help Improve Efficiency working on command line Interface

# Main Feature:
0:purely done in C++, Support windows and linux, nearly none cpu costs, totally 3Mb memory cost on Average

1:extension system Available , create a c++ function or a External script , and Register as a tool. 

2:work as both Server and client application

3:send Multiple types of file via termial 

# Prerequisite: nlohmann::json & curl (all Available in the lib-l/lib-w folder)
# Extension:
you can register your own tools in form of :
as Cpp function:
which is hardcoded ,low Efficiency and not Flexible.
you can use macro defined in the api header file to define your tool and bind it to a Cpp function Implemented on your own.

as External apps:
if you have a CLI tool, you can create a json file to Describe how to use and what it does,then place them in 
Proper Structure so than the project can Recognise them and register as Agent tool.

Currently you can find python Samples in ./assets folder.

https://github.com/pinchtab/pinchtab is Already emerged as a external app.

# build on linux
clone Repository
```
git clone https://github.com/xu-sa/cli-Assistant.git
```
change directory
```
cd ./cli-Assistant
```
build the static lib
```
./build-lib.sh
```
at this moment the lib is built Successfully,if you want to use the test app:
```
./build-test.sh
```


install as user level service (optional):
```
./install.sh
```
#build on windows

the same as linux, its just i didnt get the 'install.bat' ready yet

# How to use:
you can build the Library using ./build-lib.sh , result(.a and .h) would Appear in ./test/lib/

Or you can download the Release Library Compressed file to get the Archive file and the header file which tells you how to use.

once you have both files , here is a easy sample of how to use (you can build it via script ./build-test.sh): ./test/main.cpp

if you Decide to run it as a server or backgroudservice ,then './test/syagent_chat.py' would help you communicating with the server socket(you have to start server on socket 9995)
# Terminal Interface:
you can use and find Instruction about commands and configs once you have the sagent::interface() function running


