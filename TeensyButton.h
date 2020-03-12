#pragma once

#include <Arduino.h>
#include <Queue.h>
#include <TeensyThreads.h>
#include <TimeLib.h>

namespace TeensyButton {
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

// Main monitoring thread
void monitor(int pin);
// Start monitoring thread and return its ID
int startMonitoring(uint8_t pin);
void stopMonitoring(int id);
bool isMonitoring(int id);
bool eventWaiting();
Event getNextEvent();

} // namespace TeensyButton
