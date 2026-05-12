# cli-Assistant

a ai PC agent, Designed to Fully Guide you into command line interface. 

# what it does?

Encountering Enormous commands with tons of parameters or wanting to use some System Configuration files but having Trouble finding a proper Tutorial and not knowing how to Describe your PC Setup to ai? this Tool Handles it all.

fix your typing , give notifications about commands , write codes, check system Available Resources/Apps.
All done within Command line interface, you need no GUI apps for help.(Extremely helpful for linux starter)

# Main Feature

1:purely done in C/CPP, Support windows and linux,run as a BG service, run Senselessly, nearly none cpu/memory costs

2:extension system Available , create a c++ function or a External script , and Register as a tool.

3:work as both Server and client application, Can be attached to IM apps to manage your PC Remotely

# Extension

#as Cpp function

low Development Efficiency and not Flexible, but has the best Performance.

you can use macro defined in the api header file to define your tool and bind it to a Cpp function Implemented on your own.

#as External apps

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
build the static lib and test app(you can find it at ./test/bin)
```
./build.sh
```
install as user level service (optional):
```
./install.sh
```
#build on windows

the same as linux, only  the 'install.bat' isnt ready yet

# How to use:
you can build the Library using ./build.sh(./build.bat), and complie your only app by Referring to ./test/main.cpp

Or you can download the Release Compressed file to get the test app Directly.

#Run as a Server
if you Decide to run it as a server or backgroudservice ,then the provided python scripts would help you communicating with the server socket.

#Terminal Direct chat
you can use and find Instruction about commands and configs once you have the sagent::interface() function running


