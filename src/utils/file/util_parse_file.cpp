#include <string>
#include <fstream>
//#include "../../data/sydata.h"
#include "util_parse_file.h"
#include <vector>
//file handler
using namespace std;
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 11";

//C Style

// std::string base64_encode(char* data,size_t size_) {
//     std::string ret;
//     int i = 0;
//     int j = 0;
//     unsigned char char_array_3[3];
//     unsigned char char_array_4[4];
//     for (size_t n = 0; n < size_; ++n) {
//         char_array_3[i] = data[n];
//         i++;
//         if (i == 3) {
//             encode_triplet(char_array_3,char_array_4);
//             for (i = 0; i < 4; i++) ret += base64_chars[char_array_4[i]];
//             i = 0;
//         }
//     }
//     for(size_t k=i;k<3;k++)char_array_3[k]=0b0;//C++ 14
//     encode_triplet(char_array_3,char_array_4);
//     for(size_t k=0;k<4;k++)ret +=(k<i+1?base64_chars[char_array_4[k]]:'=');//remain n -> use n+1 to contain, index 0 to n
//     return "base64,"+ret;
// }

// std::string detect_image_format(char* data) {
//     if (data[0] == (char)0xFF && data[1] == (char)0xD8) return "data:image/jpeg;";
//     if (data[0] == (char)0x89 && data[1] == (char)0x50 && 
//         data[2] == (char)0x4E && data[3] == (char)0x47) return "data:image/png;";
//     if (data[0] == 'G' && data[1] == 'I' && data[2] == 'F') return "data:image/gif;";
//     if (data[0] == 'R' && data[1] == 'I' && 
//         data[2] == 'F' && data[3] == 'F') return "data:image/webp;";
//     return "";  
// }

static void encode_triplet(unsigned char char_array_3[3], unsigned char char_array_4[4]) {
    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;
}
 
static std::string base64_encode(const std::vector<unsigned char>& data) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3]={0};
    unsigned char char_array_4[4]={0};
    for (size_t n = 0; n < data.size(); ++n) {
        char_array_3[i] = data[n];
        i++;
        if (i == 3) {
            encode_triplet(char_array_3,char_array_4);
            for (i = 0; i < 4; i++) ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }
    for(size_t k=i;k<3;k++)char_array_3[k]=0b0;//C++ 14
    encode_triplet(char_array_3,char_array_4);
    for(size_t k=0;k<4;k++)ret +=(k<i+1?base64_chars[char_array_4[k]]:'=');//remain n -> use n+1 to contain, index 0 to n
    return "base64,"+ret;
}

std::vector<unsigned char> compress_image(const std::vector<unsigned char> data ){
    std::vector<unsigned char> data_;
    size_t index=0;
    size_t size_data=sizeof(data)/sizeof(unsigned char);
    while (index<size_data)
    {
        

    }
    return data_;
}

static std::string detect_image_format(std::vector<unsigned char>& data) {
    if ((char)data[0] == (char)0xFF && (char)data[1] == (char)0xD8) return "data:image/jpeg;";
    if ((char)data[0] == (char)0x89 && (char)data[1] == (char)0x50 && 
        (char)data[2] == (char)0x4E && (char)data[3] == (char)0x47) return "data:image/png;";
    if ((char)data[0] == 'G' && (char)data[1] == 'I' && (char)data[2] == 'F') return "data:image/gif;";
    if ((char)data[0] == 'R' && (char)data[1] == 'I' && 
        (char)data[2] == 'F' && (char)data[3] == 'F') return "data:image/webp;";
    return "";
}

FileCategory check_file_type(const string& path){
    size_t dotPos = path.find_last_of('.');
    if (dotPos == std::string::npos || dotPos == path.size()-1)return NOT_SUPPORTED;
    string I=path.substr(dotPos);
    auto it = file_type_map.find(I);
    if(it!=file_type_map.end()){
        return it->second;
    }else return NO_MARCH;
}

std::string decode_image(const std::string& filepath) {
    string i="";
    if(filepath=="")return i;
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return "Cant open this picture: " + filepath + "\n";
    }
    file.seekg(0,ios::end);//set position to the end 
    std::streamsize size = file.tellg();//return the stream position in the Form of offset from begin (size)
    std::vector<unsigned char> buffer(size);
    file.seekg(0,ios::beg);
    if (file.read((char*)buffer.data(), size)){
        i+=detect_image_format(buffer);
        if(i!="")i+=base64_encode(buffer);
    }
    file.close();
    return i;
}

std::string decode_txt(const std::string& path) {    
    std::ifstream file(path);
    if (!file.is_open()) {
        return "**cant open file: " + path + "**";
    }
    
    std::string content;// data stored on the heap memory
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    file.close();

    const size_t MAX_LENGTH = 1024*10;
    if (content.length() > MAX_LENGTH) {
        content = content.substr(0, MAX_LENGTH) + "\n...(file is too long to view here)";
    }
    
    return "**user send a text file**:\n```\n" + content + "\n```";
}

std::string decode_audio(const string& path){
    return "**user send a audio file but currently cant parse this audio file**";
}

std::string decode_video(const string& path){
    return "**user send a video file but currently cant parse this video file**";
}
