#pragma once

namespace embedded
{
    class OneshotTimer
    {
    public:
        explicit OneshotTimer(unsigned long (*timeFunc)(), void (*callback)(void *))
            : m_duration(0), m_startTime(0), m_callback(callback), m_timeFunc(timeFunc), m_expired(true){};

        void tick(void *payload)
        {
            if (!m_expired && m_timeFunc() - m_startTime >= m_duration)
            {
                m_expired = true; // do first so callback can set it again
                m_callback(payload);
            }
        }

        void setTimeMs(unsigned long duration)
        {
            m_startTime = m_timeFunc();
            m_duration = duration;
            m_expired = false;
        }

        long timeRemaining()
        {
            if (m_expired)
                return 0L;

            return m_duration - (m_timeFunc() - m_startTime);
        }

        void cancel()
        {
            m_expired = true;
        }

    private:
        unsigned long m_duration;
        unsigned long m_startTime;
        void (*const m_callback)(void *);
        unsigned long (*m_timeFunc)();
        bool m_expired = false;
    };
}