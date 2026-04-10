#ifndef SAGENT
#define SAGENT

#include <unordered_map>
#include <deque>
#include <thread>
#include <chrono>
#include <mutex>
#include "vector"
#include "../api/syagent_interface.h"
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
    nlohmann::json definition;
    std::function<std::string(const std::string*)> Actual_tool;
    bool state;
    std::vector<std::string> parameter_format;
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
};

namespace sagtlib{
    class Agent : public sagent
    {
    public:
        //stage 1
        Agent(const std::string &home, const std::string &room);//home/room would be used as workspace Folder,where room is the name of agent instance
        ~Agent();
        void register_tool(const std::string definition_[][5], size_t, std::function<std::string(const std::string *)>) override;
        nlohmann::json run_tool(size_t, nlohmann::json *);//run tool
        nlohmann::json get_skills();//get tool discriptions from Assigned tool array
        void handle_tool_request(nlohmann::json *);//Parse what tool and parameter need to use
        std::string send();//send message 
        std::string load_cfg(const std::string& );//load config from home/room/ , would not reload chat history
        std::string save(const std::string &choice);//to save whether config or chat history to home/room/
        std::string load_cht(const std::string&);//to locd cached chat from workspace
        //stage 2
        void push_input(int socket,const std::string& client_id,const std::string& text,const std::string& image) override;//to add a new message to prepare for handling
        void start_main_thread();//start instance thread
        std::string start_server_thread(const std::string& port)override;//start listening to server
        std::string stop_server_thread(const std::string& none)override;
        void stop_all_thread();//stop instance thread
        void listen_input();//loop to check whether to send message
        //stage 3
        void terminalsession(bool)override;
        void handle_input();
        std::string attach_file(const std::string&);//to attach a file in the next message
        std::string config(const std::string&);//set config 
        //Network Socket
        void respond_socket(const std::string&,int,int);
        // void respond_socket(const std::string&);
        //void cout_to_web(int socket,const std::string&);
        void start_server();//start server and bind on a specified port
        void stop_server();//stop server
        void listen_server();//loop to wait and handle port Requests
        inputs_ input_pool[INPUT_POOL_SIZE];
        int push_in;
        int push_out;
        bool on;
        std::deque<nlohmann::json> message_pool;//all the mssages
        int chat_state;
        
    private:
        Model_setup profile;//a Structure Containing all the configs of LLM
        //inputs buffer and indications
        int queued_input;
        //bool image_attached;
        int working_count;
        int fail_count;
        //server socket number disturbed by kernel 
        int socket_num;
        //to load the built in skills and user customised tools
        std::vector<SKILL> SKILLs;        
        std::unordered_map<std::string, size_t> SKILL_map;
        std::string home;
        std::string room;
        //the Multi thread setting 
        std::mutex input_mutex;  
        std::thread main_thread; 
        std::thread server_thread;
        //whether the thread function should be force to stop to end the whole Process
    };
}
#endif