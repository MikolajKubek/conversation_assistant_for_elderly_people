#include "api_manager/api_manager.hpp"
#include "api_manager/news_api.hpp"
#include "api_manager/reminder_api.hpp"
#include "api_manager/routine_api.hpp"
#include "api_manager/wikipedia_api.hpp"
#include "interaction_db/interaction_db.hpp"
#include "llm_interface/llm_interface.hpp"
#include "prompt_processor/prompt_processor.hpp"
#include "speech_components/whisper_cpp_client.hpp"
#include "task_scheduler/task_scheduler.hpp"
#include "utils/sdl_client.hpp"
#include <SDL_stdinc.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#define MAX_INTERACTOINS 4

std::string loop_callback(std::string user_prompt,
                          std::shared_ptr<GptInterface> llm_interface,
                          std::shared_ptr<ApiManager> api_manager,
                          std::vector<std::string> user_data) {
  std::cout << "user data size: " << user_data.size() << std::endl;
  std::vector<std::string> available_apis = api_manager->get_apis();
  std::string user_prompt_wrapped =
      prompt_processor::render_template(user_prompt, available_apis);
  std::string model_response = llm_interface->send_request(user_prompt_wrapped);

  int interaction_counter = 0;
  auto processed_response = api_manager->handle_response(model_response);
  bool is_final = processed_response.is_final;
  std::vector<std::pair<std::string, std::string>> followup_data;
  while (!is_final && interaction_counter < MAX_INTERACTOINS) {
    followup_data.push_back(std::make_pair(processed_response.api_called,
                                           processed_response.api_response));
    interaction_counter++;
    user_prompt_wrapped = prompt_processor::render_followup_template(
        user_prompt, followup_data, available_apis);
    model_response = llm_interface->send_request(user_prompt_wrapped);
    processed_response = api_manager->handle_response(model_response);
    is_final = processed_response.is_final;
  }

  return model_response;
}

std::string get_env(std::string env_name) {
  const char *env_raw = std::getenv(env_name.c_str());
  if (!env_raw) {
    std::cout << "Please provide " << env_name << " variable" << std::endl;
    ;
    throw 345;
  }

  return std::string(env_raw);
}

void parseArguments(int argc, char *argv[], bool &text_interface) {
  text_interface = false;

  std::vector<std::string> args(argv, argv + argc);

  for (size_t i = 0; i < args.size(); ++i) {
    if (args[i] == "--interface" && i + 1 < args.size() &&
        args[i + 1] == "text") {
      text_interface = true;
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  bool text_interface = false;
  parseArguments(argc, argv, text_interface);

  if (text_interface) {
    std::cout << "Text interface is enabled." << std::endl;
  } else {
    std::cout << "Text interface is disabled." << std::endl;
  }

  std::vector<std::string> user_data = {
      "user's primary language is polish", "user is based in Warsaw",
      "user prefers short and concise responses"};
  std::string openai_key = get_env("OPENAI_KEY");
  std::string openweather_key = get_env("OPENWEATHER_KEY");
  std::string news_api_key = get_env("NEWS_API_KEY");

  // Initialize gpt interface
  GptInterface gptInterface(openai_key);
  std::shared_ptr<GptInterface> interface_ptr =
      std::make_shared<GptInterface>(gptInterface);

  // Initialize db context
  std::shared_ptr<InteractionDb> interaction_db =
      std::make_shared<InteractionDb>();

  std::shared_ptr<TaskScheduler> task_scheduler =
      std::make_shared<TaskScheduler>();
  task_scheduler->schedule_notification(1, "halo halo test");
  task_scheduler->schedule_routine("0 15 22 * * ?", 1, "test routine");

  // Initialize API manager
  auto api_manager = std::make_shared<ApiManager>();
  std::unique_ptr<AssistantApi> respond_api =
      std::make_unique<RespondApi>(interaction_db);
  std::unique_ptr<AssistantApi> time_api = std::make_unique<TimeApi>();
  std::unique_ptr<AssistantApi> date_api = std::make_unique<DateApi>();
  std::unique_ptr<AssistantApi> weather_api =
      std::make_unique<WeatherApi>(openweather_key);
  std::unique_ptr<AssistantApi> news_api =
      std::make_unique<NewsApi>(news_api_key);
  std::unique_ptr<AssistantApi> previous_context_api =
      std::make_unique<PreviousContextApi>(interaction_db);
  std::unique_ptr<AssistantApi> wikipedia_api =
      std::make_unique<WikipediaApi>();
  std::unique_ptr<AssistantApi> reminder_api =
      std::make_unique<ReminderApi>(task_scheduler);
  std::unique_ptr<AssistantApi> routine_api =
      std::make_unique<RoutineApi>(task_scheduler);
  api_manager->register_api("respond", std::move(respond_api));
  api_manager->register_api("getTime", std::move(time_api));
  api_manager->register_api("getDate", std::move(date_api));
  api_manager->register_api("getWeather", std::move(weather_api));
  api_manager->register_api("getNews", std::move(news_api));
  api_manager->register_api("getPreviousContext",
                            std::move(previous_context_api));
  api_manager->register_api("getWikipediaArticle", std::move(wikipedia_api));
  api_manager->register_api("setReminder", std::move(reminder_api));
  api_manager->register_api("setUpRoutine", std::move(routine_api));

  if (text_interface) {
    // Initialize interaction loop
    std::cout << "Start interacting with the assistant:" << std::endl;

    std::string prompt;
    while (true) {
      std::getline(std::cin, prompt);

      if (prompt == "exit()") {
        break;
      }
      interaction_db->insert("user", prompt);

      if (task_scheduler->has_ready_interactions()) {
        TaskType task_type;
        std::string notification;
        task_scheduler->pop_ready_notification(task_type, notification);
        if (task_type == TaskType::NOTIFICATION) {
          std::cout << "notification" << std::endl;
          std::cout << notification << std::endl;
        } else {
          std::cout << "routine" << std::endl;
          std::cout << notification << std::endl;
        }
      }
      std::cout << loop_callback(prompt, interface_ptr, api_manager, user_data)
                << std::endl;
    }
  } else {
    std::cout << "voice interface" << std::endl;
    char key;
    SdlClient sdl_client = SdlClient();
    // Uint8* recording_buffer = new Uint8[sdl_client.get_buffer_size()];
    // int buffer_len;
    // sdl_client.record(recording_buffer, buffer_len);
    // sdl_client.save_wav_file("test.wav", recording_buffer, buffer_len);

    WhisperClient whisper_client = WhisperClient();
    // std::string result = whisper_client.transcribe(recording_buffer,
    // buffer_len); std::cout << result << std::endl;

    while (true) {
      Uint8 *recording_buffer = new Uint8[sdl_client.get_buffer_size()];
      int buffer_len;
      sdl_client.record(recording_buffer, buffer_len);
      // sdl_client.save_wav_file("test.wav", recording_buffer, buffer_len);

      std::string prompt =
          whisper_client.transcribe(recording_buffer, buffer_len);
      std::cout << prompt << std::endl;

      if (prompt.size() > 0) {
        std::cout << loop_callback(prompt, interface_ptr, api_manager,
                                   user_data)
                  << std::endl;
      } else {
        std::cout << "in audio was empty" << std::endl;
      }

      std::cout << "Press any key to start recording, press ESC to exit"
                << std::endl;
      std::cin >> key;
      if (key == 27) {
        break;
      }
    }
  }

  return 0;
}
