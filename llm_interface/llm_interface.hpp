#ifndef LLM_INTERFACE_HPP
#define LLM_INTERFACE_HPP
#include <string>

class LlmInterface {
    public:
        LlmInterface(std::string api_key){}
        virtual ~LlmInterface(){}
        virtual std::string send_request() = 0;
};

#endif //LLM_INTERFACE_HPP
