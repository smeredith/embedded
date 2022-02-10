#pragma once

#include <oneshottimer.h>
#include <statemachine.h>

namespace embedded
{
  namespace eventbutton
  {
    void enterCallback(void *);
    void enterDebouncing(void *);

    void debounceTimerHandler(void *);

    enum class State
    {
      start,
      callback,
      debouncing
    };

    enum class Event
    {
      low,
      high,
      timerExpired,
      nextState
    };

    using FSM = StateMachine<State, Event>;

    const FSM::Transition g_transitions[] = {
        {State::start, Event::high, State::callback},
        {State::callback, Event::nextState, State::debouncing},
        {State::debouncing, Event::low, State::debouncing},
        {State::debouncing, Event::timerExpired, State::start}};

    const FSM::Behavior g_behaviors[] = {
        {State::callback, enterCallback},
        {State::debouncing, enterDebouncing}};
  }

  class EventButton
  {
  public:
    EventButton(unsigned long intervalMs, void (*callback)(), unsigned long (*timeFunc)())
        : m_callback(callback), m_stateMachine(eventbutton::State::start), m_timer(intervalMs, eventbutton::debounceTimerHandler, timeFunc)
    {
    }

    void update(int buttonState)
    {
      m_timer.tick(this);
      if (buttonState != m_lastState)
      {
        m_stateMachine.tick(buttonState ? eventbutton::Event::high : eventbutton::Event::low, eventbutton::g_transitions, eventbutton::g_behaviors, this);
        m_lastState = buttonState;
      }
    }

    void enterCallback()
    {
      m_callback();
      m_stateMachine.tick(eventbutton::Event::nextState, eventbutton::g_transitions, eventbutton::g_behaviors, this);
    }

    void enterDebouncing()
    {
      m_timer.start();
    }

    void handleTimerExpired()
    {
      m_stateMachine.tick(eventbutton::Event::timerExpired, eventbutton::g_transitions, eventbutton::g_behaviors, this);
    }

  private:
    void (*const m_callback)();
    eventbutton::FSM m_stateMachine;
    OneshotTimer m_timer;
    int m_lastState = 0;
  };

  namespace eventbutton
  {
    void enterCallback(void *eventButton)
    {
      //Serial.println("+enterCallback");
      static_cast<EventButton *>(eventButton)->enterCallback();
    }

    void enterDebouncing(void *eventButton)
    {
      //Serial.println("+enterDebouncing()");
      static_cast<EventButton *>(eventButton)->enterDebouncing();
    }

    void debounceTimerHandler(void *eventButton)
    {
      //Serial.println("debouncTimerHandler()");
      static_cast<EventButton *>(eventButton)->handleTimerExpired();
    }
  }
}