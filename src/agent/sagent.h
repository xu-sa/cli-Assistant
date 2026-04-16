#ifndef SAGENT
#define SAGENT

#include <unordered_map>
#include <deque>
#include <thread>
#include <chrono>
#include <mutex>
#include "vector"
#include "../api/syagent_interface.h"
#include "../data/sydata.h"
#ifdef _WIN32
#include "../../lib-w/json/json.hpp"
#else
#include "../../lib-l/json/json.hpp"
#endif
#define sleep_2(s) std::this_thread::sleep_for(std::chrono::milliseconds(100*s));
#define INPUT_POOL_SIZE 10
#define CURRENT this->profile

struct inputs_{
    std::string message;
    std::string image;
    int client_socket;
    std::string client_id;
};  

struct SKILL{
    nlohmann::json definition;//the standard json format definition for LLM function calling
    std::function<std::string(const std::string*)> fuc;//the c++ function that is Compiled and will be called Directly when tool is been called
    std::vector<std::string> parameter_format;//Unify the parameter format for different skill
    std::string type;
    bool state;//activate the function or not
};

struct Model_setup{
    std::string name;
    std::string api;
    int provider;//LLM provider
    int model;//LLM model,Varies from provider
    int openroute_model;
    int local_llm_socket;
    int max_tokens;//token limites the costs
    bool stream;//stream output/NOT AVAILABLE YET
    float temperature;//temperature to make LLM speak within a Concentrated or Deviated mind 
    float top_p;//Percentage limitation for the Sum of top Possible output characters
    size_t max_message;//how many meesages will be stored
    std::string tool_choice;//Manage single tool Validity
    std::string extension_env;
};

namespace sagtlib{
    class Agent : public sagent
    {
    public:
        //stage 0
        Agent(const std::string &home, const std::string &room);//home/room would be used as workspace Folder,where room is the name of agent instance
        ~Agent();
        void register_tool(const std::string definition_[][5], size_t, std::function<std::string(const std::string *)>) override;
        std::string get_skills(const std::string*);
        std::string activate_tool(const std::string*);
        nlohmann::json run_tool(SKILL*, nlohmann::json *);//run tool
        std::string send();//send message 
        //stage 1
        std::string load_cf();//load config from home/room/ , would not reload chat history
        std::string load_ch(const std::string&);//to load cached chat from workspace
        std::string load_ex();//to load python skill from extension folder
        std::string save(const std::string &choice);//to save whether config or chat history to home/room/
        //stage 2
        void push_input(int socket,const std::string& client_id,const std::string& text,const std::string& image) override;//to add a new message to prepare for handling
        void listen_input();//loop to check whether to send message
        void start_main_thread();//start instance thread
        void stop_all_thread();//stop instance thread
        std::string start_server_thread(const std::string& port)override;//start listening to server
        std::string stop_server_thread(const std::string& none)override;
        //stage 3
        void terminalsession(bool)override;
        void handle_input();
        std::string attach_file(const std::string&);//to attach a file in the next message
        std::string config(const std::string&);//set config 
        //stage 4
        void respond_socket(const std::string&,int,int);
        void start_server();//start server and bind on a specified port
        void stop_server();//stop server
        void listen_server();//loop to wait and handle port Requests
        inputs_ input_pool[INPUT_POOL_SIZE];
        int push_in;
        int push_out;
        std::deque<nlohmann::json> message_pool;//all the mssages
        int chat_state;
        std::vector<SKILL> SKILLs;        
    //  std::unordered_map<std::string, size_t> SKILL_map;
        //whether the threads should be force to stop to end the whole Process
        bool on;
    private:
        Model_setup profile;//a Structure Containing all the configs of LLM
        //inputs buffer and indications
        int queued_input;
        //bool image_attached;
        int working_count;
        int fail_count;
        //server socket number disturbed by kernel 
        int socket_num;
        std::string home;
        std::string room;
        //the Multi thread setting 
        std::mutex input_mutex;  
        std::thread main_thread; 
        std::thread server_thread;
    };
}
#endif