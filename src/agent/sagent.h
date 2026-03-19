#ifndef SAGENT
#define SAGENT

#include <string>
#include <unordered_map>
#include <deque>
#include <thread>
#include <mutex>
#include "vector"
#include "../api/syagent_interface.h"
#include "../data/sydata.h"
#include "../json/json.hpp"
#define sleep_2 usleep(350 * 1000); 
#define CURRENT this->profile
 
//"data:image/jpeg;base64,<BASE64_string>"

namespace sagtlib{
class Agent : public sagent
{
public:
    Agent(const std::string &home, const std::string &room);
    ~Agent();
    //main Logic
    void load();
    void register_tool(const std::string definition_[][5], size_t, std::function<std::string(const std::string *)>) override;
    nlohmann::json get_skills();
    void handle_tool_request(nlohmann::json *);
    nlohmann::json run_tool(size_t, nlohmann::json *);
    std::string send();
    void start_thread();
    void stop_thread();
    void listen_input_thread();
    void push_input(int web_or_cli,const std::string& text ) override;
    void save(const std::string &choice);
    //config and settings
    std::string help();
    std::string help_provider();
    std::string help_model();
    std::string help_open_route_model();
    std::string config(const std::string &);
    //Interface functions
    void direct_chat_session() override;
    void cout_to_web(const std::string&);
    void attach_file(const std::string&);
    //Network Socket


private:
    Model_setup profile;
    std::deque<nlohmann::json> message_pool;
    inputs_ input_pool[5];
    int push_in;
    int push_out;
    int queued_input;
    bool image_attached;
    std::vector<SKILL> SKILLs;
    std::unordered_map<std::string, size_t> SKILL_map;
    std::string home;
    std::string room;
    std::mutex input_mutex;
    std::thread monitoring;
    bool on;
};
}

#endif