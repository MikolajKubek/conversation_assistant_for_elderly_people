#ifndef PROMPT_PROCESSOR_HPP
#define PROMPT_PROCESSOR_HPP
#include <string>
#include <vector>

namespace prompt_processor {
std::string render_template(std::string user_prompt,
                            std::vector<std::string> available_apis);
std::string render_followup_template(std::string user_prompt,
                                     std::string followup_data,
                                     std::vector<std::string> available_apis);
} // namespace prompt_processor

#endif // PROMPT_PROCESSOR_HPP
