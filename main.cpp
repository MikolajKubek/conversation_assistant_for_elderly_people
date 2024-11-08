#include "interaction_loop/interaction_loop.hpp"
#include "prompt_processor/prompt_processor.hpp"

int main() {
    prompt_processor::render_template();
    main_loop(invert);

    return 0;
}
