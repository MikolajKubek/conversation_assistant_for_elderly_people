#include "interaction_loop.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>

//void main_loop(std::function<std::string(std::string)> callback) {
//    std::cout << "Start interacting with the assistant:" << std::endl;
//
//    std::string prompt;
//    while (true) {
//        std::getline(std::cin, prompt);
//
//        if (prompt == "exit()") {
//            break;
//        }
//        std::cout << callback(prompt) << std::endl;
//    }
//    return;
//}

std::string invert(std::string input) {
    std::string output(input);
    std::reverse(output.begin(), output.end());
    return output;
}

