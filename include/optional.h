#pragma once

namespace embedded
{
    template <class T>
    class Optional
    {
    public:
        Optional()
            : m_hasValue(false)
        {
        }

        Optional(const T &value)
            : m_value(value), m_hasValue(true)
        {
        }

        Optional<T> &operator=(const T &value)
        {
            m_value = value;
            m_hasValue = true;
            return *this;
        }

        operator bool() const
        {
            return m_hasValue;
        }

        T &operator*()
        {
            return m_value;
        }

        T *operator->()
        {
            return &m_value;
        }

        bool has_value() const
        {
            return m_hasValue;
        }

        T &value()
        {
            return m_value;
        }

        const T &value() const
        {
            return m_value;
        }

    private:
        T m_value;
        bool m_hasValue;
    };

    template <class T>
    bool operator==(const Optional<T> &lhs, const Optional<T> &rhs)
    {
        if (!lhs.has_value() && !rhs.has_value())
            return true;

        return lhs.has_value() == rhs.has_value() && lhs.value() == rhs.value();
    }

    template <class T>
    bool operator!=(const Optional<T> &lhs, const Optional<T> &rhs)
    {
        return !(lhs == rhs);
    }

    template <class T>
    bool operator<(const Optional<T> &lhs, const Optional<T> &rhs)
    {
        if (!lhs.has_value() && !rhs.has_value())
            return false;

        if (!lhs.has_value() && rhs.has_value())
            return true;

        return lhs.value() < rhs.value();
    }
}