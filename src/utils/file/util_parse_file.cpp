//#include <string>
#include <fstream>
#include "util_parse_file.h"
#include <vector>
#include <iostream>
//file handler
#define MAX_IMAGE_SIZE 2 * 1024 * 1024
using namespace std;
#define PRINT_ERROR std::cout<<"Unexpected Error Occurred : 11";

inline static void encode_triplet(unsigned char char_array_3[3], unsigned char char_array_4[4]) {
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
    if (i > 0) {
    encode_triplet(char_array_3, char_array_4);
        for (int k = 0; k < 4; k++) {
            if (k < i + 1) {
                ret += base64_chars[char_array_4[k]];
            } else {
                ret += '=';
            }
        }
    }
    // for(size_t k=i;k<3;k++)char_array_3[k]=0b0;//C++ 14
    // encode_triplet(char_array_3,char_array_4);
    // for(size_t k=0;k<4;k++)ret +=(k<i+1?base64_chars[char_array_4[k]]:'=');//remain n -> use n+1 to contain, index 0 to n
    return ret;
}

static std::string detect_image_format(std::vector<unsigned char>& data) {
    if ((char)data[0] == (char)0xFF && (char)data[1] == (char)0xD8) return "data:image/jpeg;";
    if ((char)data[0] == (char)0x89 && (char)data[1] == (char)0x50 && 
        (char)data[2] == (char)0x4E && (char)data[3] == (char)0x47) return "data:image/png;";
    if ((char)data[0] == 'G' && (char)data[1] == 'I' && (char)data[2] == 'F') return "data:image/gif;";
    if ((char)data[0] == 'R' && (char)data[1] == 'I' && 
        (char)data[2] == 'F' && (char)data[3] == 'F') return "data:image/webp;";
    return "2";
}


FileCategory check_file_type(const string& path){
    size_t dotPos = path.find_last_of('.');
    if (dotPos == std::string::npos || dotPos == path.size()-1)return NOT_FOUND;
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
        return "0";
    }
    file.seekg(0,ios::end);//set position to the end 
    std::streamsize size = file.tellg();//return the stream position in the Form of offset from begin (size)
    if (size > MAX_IMAGE_SIZE) {// 2 MB limit
        file.close();
        return "1";
    }
    std::vector<unsigned char> buffer(size);
    file.seekg(0,ios::beg);
    if (file.read((char*)buffer.data(), size)){
        i+=detect_image_format(buffer);
        if(i!="2")i+="base64,"+base64_encode(buffer);
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