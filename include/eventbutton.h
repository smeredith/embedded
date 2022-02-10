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
      low,
      callback,
      debouncing1,
      debouncing2,
      high,
      debouncing3,
      debouncing4
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
        {State::low, Event::high, State::callback},
        {State::callback, Event::nextState, State::debouncing1},
        {State::debouncing1, Event::low, State::debouncing2},
        {State::debouncing2, Event::high, State::debouncing1},
        {State::debouncing1, Event::timerExpired, State::high},
        {State::debouncing2, Event::timerExpired, State::high},
        {State::high, Event::low, State::debouncing3},
        {State::debouncing3, Event::high, State::debouncing4},
        {State::debouncing4, Event::low, State::debouncing3},
        {State::debouncing3, Event::timerExpired, State::low},
        {State::debouncing4, Event::timerExpired, State::low}};

    const FSM::Behavior g_behaviors[] = {
        {State::callback, enterCallback},
        {State::debouncing1, enterDebouncing},
        {State::debouncing3, enterDebouncing}};
  }

  class EventButton
  {
  public:
    EventButton(unsigned long intervalMs, void (*callback)(), unsigned long (*timeFunc)())
        : m_callback(callback), m_stateMachine(eventbutton::State::low), m_timer(intervalMs, eventbutton::debounceTimerHandler, timeFunc)
    {
    }

    void update(int buttonState)
    {
      m_timer.tick(this);
      m_stateMachine.tick(buttonState ? eventbutton::Event::high : eventbutton::Event::low, eventbutton::g_transitions, eventbutton::g_behaviors, this);
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