#ifndef INTERACTION_LOOP_HPP
#define INTERACTION_LOOP_HPP

#include <functional>
#include <string>

void main_loop(std::function<std::string(std::string)> callback);
std::string invert(std::string input);

#endif // INTERACTION_LOOP_HPP
