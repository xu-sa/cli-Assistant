#ifndef CLI_TOOL
#define CLI_TOOL
#include "../api/syagent_interface.h"
DEFINE_TOOL(
    terminal,
    "To execute a shell command on the local linux/windows machine , Always use '&&' for Multiple commands Execution in one time",
    {"1","command","string","The shell command to be Executed", ""}
)
DEFINE_TOOL(
    get_image,
    "To get the image from a local path in base64 encode format,Support file type: jpeg,png,gif and webp",
    {"1","path","string","the path to the image file you want to check",""}
)
std::string skill_1(const std::string* data);
std::string skill_2(const std::string* data);
#endif