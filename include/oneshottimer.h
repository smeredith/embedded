#pragma once

namespace embedded
{
    class OneshotTimer
    {
    public:
        explicit OneshotTimer(unsigned long duration, void (*callback)(void *), unsigned long (*timeFunc)())
            : m_duration(duration), m_startTime(0), m_callback(callback), m_timeFunc(timeFunc), m_expired(true){};

        void tick(void *payload)
        {
            if (!m_expired && m_timeFunc() - m_startTime >= m_duration)
            {
                m_expired = true; // do first so callback can set it again
                m_callback(payload);
            }
        }

        void start()
        {
            m_startTime = m_timeFunc();
            m_expired = false;
        }

        void setTimeMs(unsigned long duration)
        {
            m_duration = duration;
        }

        long timeRemaining() const
        {
            if (m_expired)
                return 0L;

            return m_duration - (m_timeFunc() - m_startTime);
        }

        void cancel()
        {
            m_expired = true;
        }

        bool isActive() const
        {
            return !m_expired;
        }

    private:
        unsigned long m_duration;
        unsigned long m_startTime;
        void (*const m_callback)(void *);
        unsigned long (*const m_timeFunc)();
        bool m_expired = false;
    };
}