#include "api_manager/api_manager.hpp"
#include "task_scheduler/task_scheduler.hpp"
#include <cstdlib>

class RoutineApi : public AssistantApi {
public:
  RoutineApi(std::shared_ptr<TaskScheduler> task_scheduler) {
    this->m_task_scheduler = task_scheduler;
  };
  ~RoutineApi() {};
  std::string command = "setUpRoutine(\"CRON\", \"PROMPT\")";
  void set_params(std::vector<std::string> params) {
    if (params.size() != 2) {
      std::cout << "expected two params but received: " << params.size()
                << std::endl;
    }
    this->m_cron_schedule = convert_cron(params[0]);
    this->m_prompt = params[1];
  }

  std::string call() {
    json response;

    if (m_task_scheduler->schedule_routine(this->m_cron_schedule, 0,
                                           this->m_prompt) == EXIT_FAILURE) {
      response["error"] = "provided cron schedule is invalid, it needs to "
                          "follow Unix cron format";
    } else {
      response["message"] = "routine set up properly, it will trigger once the "
                            "requirements are met";
    }

    return response.dump(2);
  }

  std::string get_command() { return command; }

private:
  std::string m_cron_schedule;
  std::string m_prompt;
  std::shared_ptr<TaskScheduler> m_task_scheduler;
  std::string convert_cron(std::string unixCron) {
    std::istringstream stream(unixCron);
    std::vector<std::string> parts;
    std::string part;

    while (stream >> part) {
      parts.push_back(part);
    }

    if (parts.size() != 5) {
      throw std::invalid_argument(
          "Invalid UNIX cron expression: must have 5 fields");
    }

    std::string seconds = "0";
    std::string minutes = parts[0];
    std::string hours = parts[1];
    std::string dayOfMonth = parts[2];
    std::string month = parts[3];
    std::string dayOfWeek = "?";

    std::ostringstream result;
    result << seconds << " " << minutes << " " << hours << " " << dayOfMonth
           << " " << month << " " << dayOfWeek;

    return result.str();
  }
};
