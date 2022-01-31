#pragma once

namespace embedded
{
  // Capacity N-1. Adding more than this is undefined behavior.

  template <class E, int N>
  class EventQueue
  {
  public:
    EventQueue()
        : m_head(0), m_tail(0)
    {
    }

    void add(E event)
    {
      m_events[m_head++] = event;
      if (m_head == N)
        m_head = 0;
    }

    E get()
    {
      E result = m_events[m_tail++];
      if (m_tail == N)
        m_tail = 0;

      return result;
    }

    bool empty()
    {
      return m_head == m_tail;
    }

  private:
    E m_events[N];
    int m_head;
    int m_tail;
  };
}