#pragma once

#include <Arduino.h>
#include <Queue.h>
#include <TeensyThreads.h>
#include <TimeLib.h>

namespace Button {
// Event types
const uint8_t PULL_DOWN = 0;
const uint8_t PULL_UP = 1;

class Event {
public:
  Event();
  Event(uint8_t pin, uint8_t type, uint32_t time = 0);
  uint8_t pin;
  uint8_t type;
  uint32_t time;
};

extern Queue<Event> events;
extern Threads::Mutex queueLock;

// Monitoring thread function
void monitor(int pin);
// Start monitoring thread
int startMonitoring(uint8_t pin);
// Stop monitoring thread
void stopMonitoring(int id);
// Is monitoring thread active
bool isMonitoring(int id);
// Has a button event been registered
bool eventWaiting();
// A dummy event is returned if no event is registered
Event getNextEvent();
} // namespace Button
