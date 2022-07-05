/*
 * @copyright (C) 2022 Pretlist
 */

#ifndef WORKSPACE_SRC_TIMER_HPP
#define WORKSPACE_SRC_TIMER_HPP

#include <folly/FBVector.h>
#include <folly/executors/CPUThreadPoolExecutor.h>
#include <semaphore.h>

#include <atomic>
#include <csignal>
#include <cstdint>
#include <functional>
#include <log/logger.hpp>

namespace timer {
constexpr uint32_t kValue1000 = 1000;
constexpr uint32_t kOneNanoSecond = 1000000000;

class Timer {
 public:
  /**
    @brief  Constructor of Timer object
   */
  Timer();

  /**
    @brief  Destructor of Timer object
   */
  ~Timer();

  /**
    @brief  Marking copy constructors, assignment operators as NA
   */
  Timer operator=(const Timer &test) = delete;
  Timer(const Timer &test) = delete;
  Timer(Timer &&) = delete;
  Timer &operator=(Timer &&) = delete;

  static uint64_t Seconds(uint64_t sec);
  static uint64_t MilliSeconds(uint64_t milli_sec);
  static uint64_t MicroSeconds(uint64_t micro_sec);

  void Register(const std::function<void(void *)> &callback, void *userdata);
  void SetSingleShot(bool single_shot);
  void SetTimeout(uint64_t timeout);
  bool Start();
  bool Start(uint64_t timeout);
  bool Stop();

 protected:
  void mHandleCallback();

 private:
  static void mSigHandler(int signo, siginfo_t *info, void *context);

  bool mInit();

  bool m_single_shot = false;
  timer_t m_timer_id = nullptr;
  struct itimerspec m_its{};
  std::function<void(void *)> m_callback = nullptr;
  void *m_userdata = nullptr;
};
}  // namespace timer
#endif
