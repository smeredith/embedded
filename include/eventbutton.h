#pragma once

#include <oneshottimer.h>
#include <statemachine.h>

namespace embedded
{
  namespace eventbutton
  {
    void enterPressing(void *);
    void enterDebouncingPress(void *);

    void debounceTimerHandler(void *);

    enum class State
    {
      pressing,
      debouncingPress,
      pressed,
      released
    };

    enum class Event
    {
      low,
      high,
      timerExpired
    };

    using FSM = StateMachine<State, Event>;

    const FSM::Transition g_transitions[] = {
        {State::released, Event::high, State::pressing},
        {State::pressing, Event::high, State::debouncingPress},
        {State::debouncingPress, Event::high, State::debouncingPress},
        {State::debouncingPress, Event::timerExpired, State::pressed},
        {State::pressed, Event::low, State::released}};

    const FSM::Behavior g_behaviors[] = {
        {State::pressing, enterPressing},
        {State::debouncingPress, enterDebouncingPress}};
  }

  class EventButton
  {
  public:
    EventButton(unsigned long intervalMs, void (*callback)(), unsigned long (*timeFunc)())
        : m_intervalMs(intervalMs), m_callback(callback), m_stateMachine(eventbutton::State::released), m_timer(eventbutton::debounceTimerHandler, timeFunc)
    {
    }

    void update(int buttonState)
    {
      m_stateMachine.enqueue(buttonState ? eventbutton::Event::high : eventbutton::Event::low);
      m_timer.tick(this);
      m_stateMachine.tick(eventbutton::g_transitions, eventbutton::g_behaviors, this);
    }

    void enterPressing()
    {
      m_callback();
    }

    void enterDebouncingPress()
    {
      m_timer.setTimeMs(m_intervalMs);
    }

    void handleTimerExpired()
    {
      m_stateMachine.enqueue(eventbutton::Event::timerExpired);
    }

  private:
    unsigned long m_intervalMs;
    void (*const m_callback)();
    eventbutton::FSM m_stateMachine;
    OneshotTimer m_timer;
  };

  namespace eventbutton
  {
    void enterPressing(void *eventButton)
    {
      //Serial.println("+Pressing");
      static_cast<EventButton *>(eventButton)->enterPressing();
    }

    void enterDebouncingPress(void *eventButton)
    {
      //Serial.println("+DebouncingPress()");
      static_cast<EventButton *>(eventButton)->enterDebouncingPress();
    }

    void debounceTimerHandler(void *eventButton)
    {
      //Serial.println("debouncTimerHandler()");
      static_cast<EventButton *>(eventButton)->handleTimerExpired();
    }
  }
}