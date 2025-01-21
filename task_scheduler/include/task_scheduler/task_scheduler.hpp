#ifndef TASK_SCHEDULER_HPP
#define TASK_SCHEDULER_HPP

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <libcron/Cron.h>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>
#include <vector>

libcron::Cron cron;

enum TaskType { NOTIFICATION, ROUTINE };

class TaskScheduler {
public:
  TaskScheduler()
      : stop_flag(false), worker(&TaskScheduler::notification_worker, this),
        routine_worker_thread(&TaskScheduler::routine_worker, this) {}
  ~TaskScheduler() {
    stop_flag = true;
    cv.notify_all();
    if (worker.joinable()) {
      worker.join();
    }
  }

  int schedule_notification(int minutes, std::string notification_text) {
    const std::chrono::system_clock::time_point trigger_time =
        std::chrono::system_clock::now() + std::chrono::minutes(minutes);
    {
      std::lock_guard<std::mutex> lock(notification_queue_mutex);
      notification_queue.push({notification_text, trigger_time});
      std::cout << "scheduled notification " << std::endl;
    }
    cv.notify_all();

    return EXIT_SUCCESS;
  };

  bool has_ready_interactions() {
    std::lock_guard<std::mutex> lock(notification_queue_mutex);
    std::cout << "result_queue size: " << result_queue.size() << std::endl;
    return !result_queue.empty();
  }

  int pop_ready_notification(TaskType &task_type, std::string &notification) {
    std::lock_guard<std::mutex> lock(notification_queue_mutex);
    if (!result_queue.empty()) {
      task_type = result_queue.front().first;
      notification = result_queue.front().second;
      result_queue.pop();
      return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
  }

  int schedule_routine(std::string cron_schedule, int id,
                       std::string routine_text) {
    std::cout << "scheduling a routine " << routine_text << std::endl;
    bool cron_scheduled_properly =
        cron.add_schedule(routine_text, cron_schedule, [=](auto &) {
          std::cout << "routine called: " << routine_text << std::endl;
          std::lock_guard<std::mutex> lock(notification_queue_mutex);
          result_queue.push(std::make_pair(TaskType::ROUTINE, routine_text));
        });

    if (!cron_scheduled_properly) {
      return EXIT_FAILURE;
    }

    std::cout << "amount of scheduled routines: " << cron.count() << std::endl;

    return EXIT_SUCCESS;
  }
  int cancel_routine(int id);

private:
  struct Notification {
    std::string message;
    std::chrono::system_clock::time_point trigger_time;

    bool operator>(const Notification &other) const {
      return trigger_time > other.trigger_time;
    }
  };
  std::atomic<bool> stop_flag;
  std::thread worker;
  std::thread routine_worker_thread;
  std::condition_variable cv;
  std::mutex notification_queue_mutex;
  std::queue<std::pair<TaskType, std::string>> result_queue;
  std::priority_queue<Notification, std::vector<Notification>, std::greater<>>
      notification_queue;

  void notification_worker() {
    std::cout << "start worker" << std::endl;
    while (!stop_flag) {
      std::cout << "work" << std::endl;
      std::unique_lock<std::mutex> lock(notification_queue_mutex);
      if (notification_queue.empty()) {
        std::cout << "queue empty" << std::endl;
        cv.wait(lock,
                [this] { return stop_flag || !notification_queue.empty(); });
      } else {
        std::cout << "queue not empty" << std::endl;
        auto now = std::chrono::system_clock::now();
        auto next_notification = notification_queue.top();
        if (now >= next_notification.trigger_time) {
          std::cout << "Notification: " << next_notification.message
                    << std::endl;
          result_queue.push(std::make_pair(TaskType::NOTIFICATION,
                                           next_notification.message));
          notification_queue.pop();
        } else {
          std::cout << "waiting for notification" << std::endl;
          cv.wait_until(lock, next_notification.trigger_time);
        }
      }
    }
  }

  void routine_worker() {
    std::cout << "start routine worker" << std::endl;
    while (!stop_flag) {
      cron.tick();
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }
};
#endif // TASK_SCHEDULER_HPP
