#include "api_manager/api_manager.hpp"
#include "task_scheduler/task_scheduler.hpp"

class RoutineApi: public AssistantApi {
public:
  RoutineApi(std::shared_ptr<TaskScheduler> task_scheduler) {
    this->m_task_scheduler = task_scheduler;
  };
  ~RoutineApi() {};
  std::string command = "setUpRoutine(\"CRON\", \"PROMPT\")";
  void set_params(std::vector<std::string> params) {
    if (params.size() != 2) {
      std::cout << "expected two params but received: " << params.size() << std::endl;
    }
    this->m_cron_schedule = params[0];
    this->m_prompt = params[1];
  }
  std::string call() {
    json response;

    if (!m_task_scheduler->schedule_routine(this->m_cron_schedule, 0, this->m_prompt)) {
      response["error"] = "provided cron schedule is invalid, it needs to follow Java cron format";
    } else {
      response["message"] = "routine set properly";
    }

    return response.dump(2);
  }

  std::string get_command() { return command; }
private:
  std::string m_cron_schedule;
  std::string m_prompt;
  std::shared_ptr<TaskScheduler> m_task_scheduler;

};
