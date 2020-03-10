#include "Button.h"

Button::Event::Event() {
  pin = -1;
  type = -1;
}

Button::Event::Event(uint8_t pin, uint8_t type, uint32_t time) {
  this->pin = pin;
  this->type = type;
  this->time = time;
}

namespace Button {
Queue<Button::Event> events;
Threads::Mutex queueLock;
} // namespace Button

void Button::monitor(int pin) {
  bool up = true;
  bool down = false;
  pinMode(pin, INPUT);
  while (true) {
    if (digitalRead(pin)) {
      if (!up) {
        Event event(pin, PULL_UP);
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
        Event event(pin, PULL_DOWN);
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

int Button::startMonitoring(uint8_t pin) {
  return threads.addThread(monitor, pin);
}

void Button::stopMonitoring(int id) { threads.kill(id); }

bool Button::isMonitoring(int id) {
  return threads.getState(id) == Threads::RUNNING;
}

bool Button::eventWaiting() { return !Button::events.isEmpty(); }

Button::Event Button::getNextEvent() {
  if (Button::eventWaiting()) {
    Button::queueLock.lock();
    Button::Event event = Button::events.dequeue();
    Button::queueLock.unlock();
    return event;
  }

  return Button::Event();
}