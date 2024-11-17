#ifndef PROMPT_PROCESSOR_HPP
#define PROMPT_PROCESSOR_HPP
#include <iostream>

namespace prompt_processor {
    std::string render_template(std::string user_prompt);
    std::string render_followup_template(std::string user_prompt, std::string followup_data);
}

#endif //PROMPT_PROCESSOR_HPP
