#ifndef UTILS_3
#define UTILS_3
#include <string>
#include <unordered_map>
enum FileCategory {
    NOT_FOUND,
    NO_MARCH,
    IMAGE,
    // DOCUMENT,
    // AUDIO,
    // VIDEO,
    // ZIP,
    // BIN,
    // CATEGORY_COUNT
};
 

    
static std::unordered_map<std::string, FileCategory> file_type_map = {
    // image file 12 types
    {".jpg", IMAGE}, {".jpeg", IMAGE}, {".jpe", IMAGE},
    {".jfif", IMAGE}, {".jif", IMAGE}, {".png", IMAGE},
    {".webp", IMAGE}, {".gif", IMAGE}, {".bmp", IMAGE},
    {".dib", IMAGE}, {".tiff", IMAGE}, {".tif", IMAGE},
    
    // text file 23 types
    // {"",DOCUMENT},{".txt", DOCUMENT}, {".text", DOCUMENT},
    // //{".pdf", DOCUMENT}, {".csv", DOCUMENT}, {".pptx", DOCUMENT}, {".ppt", DOCUMENT},{".docx", DOCUMENT},{".doc", DOCUMENT}, 
    // {".md", DOCUMENT},{".xlsx", DOCUMENT},{".sh",DOCUMENT},
    // {".bat",DOCUMENT},
    // {".xls", DOCUMENT}, {".html", DOCUMENT}, {".htm", DOCUMENT},
    // {".json", DOCUMENT}, {".h", DOCUMENT}, {".cpp", DOCUMENT},
    // {".py", DOCUMENT}, {".java", DOCUMENT}, {".js", DOCUMENT},
    // {".ts", DOCUMENT}, {".c", DOCUMENT}, {".cs", DOCUMENT},
    // {".php", DOCUMENT}, {".rb", DOCUMENT}, {".go", DOCUMENT},
    // {".rs", DOCUMENT}, {".swift", DOCUMENT}, {".kt", DOCUMENT},
    
    
    // // audio file 9 types
    // {".mp3", AUDIO}, {".mp4", AUDIO}, {".m4a", AUDIO},
    // {".wav", AUDIO}, {".flac", AUDIO}, {".aac", AUDIO},
    // {".ogg", AUDIO}, {".oga", AUDIO}, {".wma", AUDIO},
    
    // // video file 8 types
    // {".mp4", VIDEO}, {".m4v", VIDEO}, {".mov", VIDEO},
    // {".avi", VIDEO}, {".mkv", VIDEO}, {".webm", VIDEO},
    // {".flv", VIDEO}, {".wmv", VIDEO},
    
    // // zip files 
    // {".zip", ZIP}, {".rar", ZIP}, {".7z", ZIP},
    // {".tar", ZIP}, {".gz", ZIP},
    
    // // binary files
    // {".exe", BIN}, {".dll", BIN}, {".bin", BIN}
};
FileCategory check_file_type(const std::string& path);//return FileCategory 
std::string decode_image(const std::string& filepath);//decode image as base64 Sequence
// std::string decode_audio(const std::string& filepath);//decode mp3 file into string
// std::string decode_txt(const std::string& filepath);//copy the text into string
// std::string decode_video(const std::string& filepath);//decode file into...

#endif