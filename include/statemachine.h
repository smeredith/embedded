#pragma once

#include "optional.h"

namespace embedded
{
  template <class T, class S, class E>
  Optional<S> getNextState(const T &transitions, S currentState, E event)
  {
    for (const auto &transition : transitions)
    {
      if (transition.startState == currentState && transition.event == event)
      {
        return transition.nextState;
      }
    }
    return Optional<S>();
  }

  template <class B, class S>
  void performEntryBehaviorForState(const B &behaviors, S state, void *callbackPayload)
  {
    for (const auto &behavior : behaviors)
    {
      if (behavior.state == state)
      {
        behavior.onEntryCallback(callbackPayload);
        return;
      }
    }
  }

  template <class T, class S, class B, class E>
  Optional<S> enterNextState(const T &transitions, const B &behaviors, S currentState, E event, void *callbackPayload)
  {
    Optional<S> newState = getNextState(transitions, currentState, event);

    if (newState)
    {
      performEntryBehaviorForState(behaviors, *newState, callbackPayload);
    }

    return newState;
  }

  template <class StateT, class EventT>
  class StateMachine
  {
  public:
    struct Transition
    {
      StateT startState;
      EventT event;
      StateT nextState;
    };

    struct Behavior
    {
      StateT state;
      void (*onEntryCallback)(void *);
    };

    StateMachine(StateT initialState)
        : m_state(initialState)
    {
    }

    template <class T, class B>
    void tick(EventT event, const T &transitions, const B &behaviors, void *callbackPayload)
    {
      Optional<StateT> nextState = enterNextState(transitions, behaviors, m_state, event, callbackPayload);
      if (nextState)
      {
        m_state = *nextState;
      }
    }

    template <class T, class B>
    void tick(EventT event, const T &transitions, const B &behaviors)
    {
      tick(event, transitions, behaviors, nullptr);
    }

  private:
    StateT m_state;
  };
}