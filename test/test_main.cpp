#include <unity.h>
#include "eventbutton.h"
#include "eventqueue.h"
#include "oneshottimer.h"
#include "optional.h"
#include "statemachine.h"

using namespace embedded;

enum class Event
{
    init,
    event1,
    event2,
    event3,
    event4,
    event5
};

enum class State
{
    init,
    s0,
    s1,
    s2
};

int g_enters0 = 0;
int g_enters1 = 0;
int g_enters2 = 0;

void resetMonitors()
{
    g_enters0 = 0;
    g_enters1 = 0;
    g_enters2 = 0;
}

void enters0(void *) { g_enters0++; }
void enters1(void *) { g_enters1++; }
void enters2(void *) { g_enters2++; }

using MyStateMachine = StateMachine<State, Event>;

const MyStateMachine::Behavior g_behaviors[] = {
    {State::s0, enters0},
    {State::s1, enters1},
    {State::s2, enters2}};

const MyStateMachine::Transition g_transitions[] = {
    {State::init, Event::init, State::s0},
    {State::s0, Event::event1, State::s0},
    {State::s0, Event::event2, State::s1},
    {State::s0, Event::event3, State::s2},
    {State::s1, Event::event4, State::s0},
    {State::s1, Event::event5, State::s1},
    {State::s1, Event::event1, State::s2},
    {State::s2, Event::event2, State::s0}};

void test_EventQueue_Empty()
{
    EventQueue<Event, 4> eventQueue;
    TEST_ASSERT_TRUE(eventQueue.empty());

    eventQueue.add(Event::event1);
    TEST_ASSERT_FALSE(eventQueue.empty());
}

void test_EventQueue_Add_Get()
{
    EventQueue<Event, 4> eventQueue;

    eventQueue.add(Event::event1);
    TEST_ASSERT_EQUAL(Event::event1, eventQueue.get());
}

void test_EventQueue_Wrap()
{
    EventQueue<Event, 6> eventQueue;

    eventQueue.add(Event::event1);
    eventQueue.add(Event::event2);
    TEST_ASSERT_EQUAL(Event::event1, eventQueue.get());
    eventQueue.add(Event::event3);
    eventQueue.add(Event::event4);
    TEST_ASSERT_EQUAL(Event::event2, eventQueue.get());
    TEST_ASSERT_EQUAL(Event::event3, eventQueue.get());
    TEST_ASSERT_EQUAL(Event::event4, eventQueue.get());
    eventQueue.add(Event::event5);
    TEST_ASSERT_EQUAL(Event::event5, eventQueue.get());
}

void test_TransitionFromInit()
{
    resetMonitors();

    MyStateMachine stateMachine(State::init);
    stateMachine.enqueue(Event::init);
    stateMachine.tick(g_transitions, g_behaviors, nullptr);

    TEST_ASSERT_EQUAL(1, g_enters0);
    TEST_ASSERT_EQUAL(0, g_enters1);
    TEST_ASSERT_EQUAL(0, g_enters2);
}

void test_TransitionToSelf()
{
    resetMonitors();

    MyStateMachine stateMachine(State::init);
    stateMachine.enqueue(Event::init);
    stateMachine.tick(g_transitions, g_behaviors, nullptr);
    stateMachine.enqueue(Event::event1);
    stateMachine.tick(g_transitions, g_behaviors, nullptr);

    TEST_ASSERT_EQUAL(2, g_enters0);
    TEST_ASSERT_EQUAL(0, g_enters1);
    TEST_ASSERT_EQUAL(0, g_enters2);
}

void test_NoTransitionsNoBehaviors()
{
    resetMonitors();

    MyStateMachine stateMachine(State::init);
    stateMachine.enqueue(Event::event1);
    stateMachine.tick(g_transitions, g_behaviors, nullptr);

    TEST_ASSERT_EQUAL(0, g_enters0);
    TEST_ASSERT_EQUAL(0, g_enters1);
    TEST_ASSERT_EQUAL(0, g_enters2);
}

void test_NoSelfTransisionIfNotInTransitionTable()
{
    resetMonitors();

    MyStateMachine stateMachine(State::s0);
    stateMachine.enqueue(Event::init);
    stateMachine.tick(g_transitions, g_behaviors, nullptr);

    TEST_ASSERT_EQUAL(0, g_enters0);
    TEST_ASSERT_EQUAL(0, g_enters1);
    TEST_ASSERT_EQUAL(0, g_enters2);
}

void test_NoEventsNoBehaviors()
{
    resetMonitors();

    MyStateMachine stateMachine(State::init);
    stateMachine.tick(g_transitions, g_behaviors, nullptr);

    TEST_ASSERT_EQUAL(0, g_enters0);
    TEST_ASSERT_EQUAL(0, g_enters1);
    TEST_ASSERT_EQUAL(0, g_enters2);
}

void test_OptionalDefaultHasNoValue()
{
    Optional<int> optional;
    TEST_ASSERT_FALSE(optional);
}

void test_OptionalOperatorBool()
{
    Optional<int> optional = 9;
    TEST_ASSERT_TRUE(optional);
}

void test_OptionalInitWithValue()
{
    Optional<int> optional(10);
    TEST_ASSERT_EQUAL(10, *optional);
}

void test_OptionalCopyAssignment()
{
    Optional<int> optional(11);
    Optional<int> optional2 = optional;
    TEST_ASSERT_EQUAL(11, *optional2);
}

void test_OptionalAssignmentViaRef()
{
    Optional<int> optional(12);
    *optional = 13;
    TEST_ASSERT_EQUAL(13, *optional);
}

void test_OptionalAssignment()
{
    Optional<int> optional;
    optional = 1;
    TEST_ASSERT_EQUAL(1, *optional);
}

void test_OptionalArrow()
{
    Optional<int> optional(14);
    TEST_ASSERT_EQUAL(14, *(optional.operator->()));
}

void test_OptionalHasValue()
{
    Optional<int> optional;
    TEST_ASSERT_FALSE(optional.has_value());
    Optional<int> optional2 = 15;
    TEST_ASSERT_TRUE(optional2.has_value());
}

void test_OptionalValue()
{
    Optional<int> optional(16);
    TEST_ASSERT_EQUAL(16, optional.value());
}

void test_OptionalEquality()
{
    Optional<int> o1;
    Optional<int> o2;

    TEST_ASSERT_TRUE(o1 == o2);

    Optional<int> o3(1);
    Optional<int> o4(1);
    TEST_ASSERT_TRUE(o3 == o4);

    Optional<int> o5(2);
    TEST_ASSERT_FALSE(o3 == o5);
}

void test_OptionalNotEqual()
{
    Optional<int> o1;
    Optional<int> o2(0);

    TEST_ASSERT_TRUE(o1 != o2);

    Optional<int> o3(1);
    TEST_ASSERT_TRUE(o2 != o3);
}

void test_OptionalLess()
{
    TEST_ASSERT_FALSE(Optional<int>() < Optional<int>());
    TEST_ASSERT_TRUE(Optional<int>() < Optional<int>(0));
    TEST_ASSERT_TRUE(Optional<int>(0) < Optional<int>(1))
    TEST_ASSERT_FALSE(Optional<int>(1) < Optional<int>(0));
}

int g_timerCallbackCalled = 0;
void *g_timerCallbackPayload = nullptr;
void timerCallback(void *payload)
{
    g_timerCallbackCalled++;
    g_timerCallbackPayload = payload;
}

unsigned long g_timerValue = 0;
unsigned long timeFunction()
{
    return g_timerValue;
}

void test_TimerCallbackGetsCalled()
{
    g_timerCallbackCalled = 0;
    OneshotTimer timer(timerCallback, timeFunction);
    timer.setTimeMs(0);
    g_timerValue = 1;
    timer.tick(nullptr);

    TEST_ASSERT_EQUAL(1, g_timerCallbackCalled);
}

void test_TimerCancel()
{
    g_timerCallbackCalled = 0;
    OneshotTimer timer(timerCallback, timeFunction);
    timer.setTimeMs(0);
    g_timerValue = 1;
    timer.cancel();
    timer.tick(nullptr);

    TEST_ASSERT_EQUAL(0, g_timerCallbackCalled);
}

void test_TimerPayload()
{
    OneshotTimer timer(timerCallback, timeFunction);
    g_timerValue = 1;
    timer.setTimeMs(0);
    timer.tick(&timer);

    TEST_ASSERT_EQUAL(&timer, g_timerCallbackPayload);
}

void test_TimerIsActive()
{
    OneshotTimer timer(timerCallback, timeFunction);
    g_timerValue = 0;
    timer.setTimeMs(1);

    TEST_ASSERT_TRUE(timer.isActive());
    g_timerValue = 1;
    timer.tick(nullptr);
    TEST_ASSERT_FALSE(timer.isActive());
}

int g_eventButtonCallbackCalled = 0;
void eventButtonCallback()
{
    g_eventButtonCallbackCalled++;
}

void test_EventButtonCallbackGetsCalled()
{
    g_eventButtonCallbackCalled = 0;
    EventButton button(1, eventButtonCallback, timeFunction);
    g_timerValue = 0;
    button.update(1);
    TEST_ASSERT_EQUAL(1, g_eventButtonCallbackCalled);
}

void test_EventButtonDebounces()
{
    g_eventButtonCallbackCalled = 0;
    EventButton button(3, eventButtonCallback, timeFunction);
    g_timerValue = 0;
    button.update(1);
    g_timerValue = 1;
    button.update(0);
    g_timerValue = 2;
    button.update(1);

    TEST_ASSERT_EQUAL(1, g_eventButtonCallbackCalled);
}

void test_EventButtonResets()
{
    g_eventButtonCallbackCalled = 0;
    EventButton button(4, eventButtonCallback, timeFunction);
    g_timerValue = 0;
    button.update(1);
    button.update(1);
    button.update(0);
    g_timerValue = 4;
    button.update(0);
    button.update(0);
    button.update(1);

    TEST_ASSERT_EQUAL(2, g_eventButtonCallbackCalled);
}

void process()
{
    UNITY_BEGIN();
    RUN_TEST(test_EventQueue_Empty);
    RUN_TEST(test_EventQueue_Add_Get);
    RUN_TEST(test_EventQueue_Wrap);

    RUN_TEST(test_TransitionFromInit);
    RUN_TEST(test_TransitionToSelf);
    RUN_TEST(test_NoTransitionsNoBehaviors);
    RUN_TEST(test_NoSelfTransisionIfNotInTransitionTable);
    RUN_TEST(test_NoEventsNoBehaviors);

    RUN_TEST(test_OptionalDefaultHasNoValue);
    RUN_TEST(test_OptionalOperatorBool);
    RUN_TEST(test_OptionalInitWithValue);
    RUN_TEST(test_OptionalCopyAssignment);
    RUN_TEST(test_OptionalAssignmentViaRef);
    RUN_TEST(test_OptionalAssignment);
    RUN_TEST(test_OptionalArrow);
    RUN_TEST(test_OptionalHasValue);
    RUN_TEST(test_OptionalValue);
    RUN_TEST(test_OptionalEquality);
    RUN_TEST(test_OptionalNotEqual);
    RUN_TEST(test_OptionalLess);

    RUN_TEST(test_TimerCallbackGetsCalled);
    RUN_TEST(test_TimerCancel);
    RUN_TEST(test_TimerPayload);
    RUN_TEST(test_TimerIsActive);

    RUN_TEST(test_EventButtonCallbackGetsCalled);
    RUN_TEST(test_EventButtonDebounces);
    RUN_TEST(test_EventButtonResets);

    UNITY_END();
}

#ifdef ARDUINO

#include <Arduino.h>

void setup()
{
    delay(2000);
    pinMode(LED_BUILTIN, OUTPUT);

    process();
}

void loop()
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
}

#else

int main(int argc, char **argv)
{
    process();
    return 0;
}

#endif