#ifndef UTILS_SW_TASK_H
#define UTILS_SW_TASK_H

#include <string>
#include <Arduino.h>

namespace abckoth
{

template<uint32_t STACK_DEPTH = 1024, uint32_t PRIORITY = tskIDLE_PRIORITY>
class Task {
public:
  explicit Task(const std::string& module_name)
  : c_task_name(module_name), m_task_handle(nullptr) { }

  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;
  Task(Task&& moved) = delete;
  Task& operator=(Task&&) = delete;

  virtual ~Task() { destroy(); }

  [[nodiscard]] std::string getName() const { return c_task_name; }

  virtual void taskFunc() = 0;

  virtual void create() {
    if(!m_task_handle) {
      xTaskCreate(
        [](void* obj) constexpr {
          uint32_t last_tick = xTaskGetTickCount();
          for (;;) {
            static_cast<decltype(this)>(obj)->taskFunc();
            taskYIELD(); // yield instead of delay for 100% cpu utilization
          }
        },
        c_task_name.c_str(),
        STACK_DEPTH,
        this,
        PRIORITY,
        &m_task_handle
      );
    }
  }

  virtual void suspend() {
    if (m_task_handle) vTaskSuspend(m_task_handle);
  }

  virtual void resume() {
    if (m_task_handle) vTaskResume(m_task_handle);
  }

  virtual void destroy() {
    if (m_task_handle) {
      vTaskDelete(m_task_handle); // `delete` is reserved & destroy is a better
      m_task_handle = nullptr;    // pair for "create"
    }
  }

private:
  const std::string c_task_name;
  TaskHandle_t m_task_handle;
}; // class Task
} // namespace abckoth


#endif // MODULES_MODULE_BASE_H