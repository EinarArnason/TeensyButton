#include "TeensyButton.h"

TeensyButton::Event::Event() {
  pin = -1;
  type = -1;
  time = 0;
}

TeensyButton::Event::Event(uint8_t pin, uint8_t type, uint32_t time) {
  this->pin = pin;
  this->type = type;
  this->time = time;
}

namespace TeensyButton {
Queue<TeensyButton::Event> events;
Threads::Mutex queueLock;
} // namespace TeensyButton

void TeensyButton::monitor(int pin) {
  bool up = true;
  bool down = false;
  pinMode(pin, INPUT);
  while (true) {
    if (digitalRead(pin)) {
      if (!up) {
        Event event(pin, PULL_UP, millis());
        queueLock.lock();
        events.enqueue(event);
        queueLock.unlock();
        down = false;
        up = true;
      } else {
        threads.yield();
      }
    } else {
      if (!down) {
        Event event(pin, PULL_DOWN, millis());
        queueLock.lock();
        events.enqueue(event);
        queueLock.unlock();
        down = true;
        up = false;
      } else {
        threads.yield();
      }
    }
  }
}

int TeensyButton::startMonitoring(uint8_t pin) {
  return threads.addThread(monitor, pin);
}

void TeensyButton::stopMonitoring(int id) { threads.kill(id); }

bool TeensyButton::isMonitoring(int id) {
  return threads.getState(id) == Threads::RUNNING;
}

bool TeensyButton::eventWaiting() { return !TeensyButton::events.isEmpty(); }

TeensyButton::Event TeensyButton::getNextEvent() {
  if (TeensyButton::eventWaiting()) {
    TeensyButton::queueLock.lock();
    TeensyButton::Event event = TeensyButton::events.dequeue();
    TeensyButton::queueLock.unlock();

    return event;
  }
  // If event queue is empty, return dummy event
  return TeensyButton::Event();
}