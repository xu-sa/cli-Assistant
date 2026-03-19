#ifndef SMANNAGER
#define SMANNAGER
#include <deque>
#include <string>
#include "../json/json.hpp"
#include "../api/syagent_interface.h"
namespace sagtlib{
class manager: public sagent_mannager{
    public:
        manager();
        void add_agent() override;
        void remove_agent() override;
        void new_session() override;
    private:
        std::deque<manager*> members;
    };
}
#endif