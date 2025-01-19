#include <api_manager/api_manager.hpp>
#include <memory>
#include <task_scheduler/task_scheduler.hpp>
#include <string>

class ReminderApi : public AssistantApi {
public:
  ReminderApi(std::shared_ptr<TaskScheduler> task_scheduler) {
    this->m_task_scheduler = task_scheduler;
  };
  ~ReminderApi() {};
  std::string command = "setReminder(\"MINUTES\", \"TEXT\")";
  void set_params(std::vector<std::string> params) {
    if (params.size() != 2) {
      std::cout << "expected two params but received: " << params.size() << std::endl;
    }
    this->m_minutes = std::stoi(params[0]);
    this->m_notification_text = params[1];
  }
  std::string call() {
    m_task_scheduler->schedule_notification(this->m_minutes, this->m_notification_text);

    json response;

    response["message"] = "reminder set properly";

    return response.dump(2);
  }

  std::string get_command() { return command; }
private:
  int m_minutes;
  std::string m_notification_text;
  std::shared_ptr<TaskScheduler> m_task_scheduler;
};

