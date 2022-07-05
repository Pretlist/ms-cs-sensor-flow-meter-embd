/*
   Copyright (C) 2022 Pretlist
 */
#include "timer.hpp"
#include <sys/time.h>
#include <csignal>
#include <cassert>

namespace timer {
void Timer::mSigHandler(int sig_no, siginfo_t *sig_info, void *context) {
  auto *ptr = static_cast<Timer *>(sig_info->_sifields._rt.si_sigval.sival_ptr);
  if (ptr != nullptr) {
    ptr->mHandleCallback();
  }
}

Timer::Timer() {
  assert(mInit());
  Stop();
}

Timer::~Timer() {
  Stop();
  timer_delete(m_timer_id);
}
  
uint64_t Timer::Seconds(uint64_t sec) {
  return Timer::MilliSeconds(sec * kValue1000);
}

uint64_t Timer::MilliSeconds(uint64_t milli_sec) {
  return Timer::MicroSeconds(milli_sec * kValue1000);
}

uint64_t Timer::MicroSeconds(uint64_t micro_sec) {
  return micro_sec * kValue1000;
}

void Timer::Register(const std::function<void(void *)> &callback, void *userdata) {
  m_callback = callback;
  m_userdata = userdata;
}
  
void Timer::SetSingleShot(bool single_shot) {
  m_single_shot = single_shot;
}
  
void Timer::SetTimeout(uint64_t timeout) {
  m_its.it_value.tv_sec = timeout / kOneNanoSecond;
  m_its.it_value.tv_nsec = timeout % kOneNanoSecond;
  if (m_single_shot) {
    m_its.it_interval.tv_sec = 0;
    m_its.it_interval.tv_nsec = 0;
  } else {
    m_its.it_interval.tv_sec = m_its.it_value.tv_sec;
    m_its.it_interval.tv_nsec = m_its.it_value.tv_nsec;
  }
}

bool Timer::Start() {
  if (timer_settime(&m_timer_id, 0, &m_its, nullptr) == -1) {
    return false;
  }
  
  return true;
}

bool Timer::Start(uint64_t timeout) {
  SetTimeout(timeout);
  return Start();
}

bool Timer::Stop() {
  SetTimeout(0);
  if (timer_settime(&m_timer_id, 0, &m_its, nullptr) == -1) {
    return false;
  }
  
  return true;
}

void Timer::mHandleCallback() {
  if (m_callback != nullptr) {
    m_callback(m_userdata);
  }
}

bool Timer::mInit() {
  // Set up signal handler
  struct sigaction sa {};
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = timer::Timer::mSigHandler; 
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGALRM, &sa, nullptr) == -1) {
    return false;
  }

  // Set and enable alarm
  struct sigevent sev {};
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGALRM;
  sev.sigev_value.sival_ptr = static_cast<void *>(this);
  if (timer_create(CLOCK_REALTIME, &sev, &m_timer_id) == -1) {
    return false;
  }

  return true;
}
}  // namespace timer
