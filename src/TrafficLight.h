#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
    void send(T &&msg);
    T receive();

private:
    std::condition_variable _condition;
    std::mutex _mutex;
    std::deque<T> _queue;
};

enum TrafficLightPhase{
    red, 
    green
};

class TrafficLight: public TrafficObject, public std::enable_shared_from_this<TrafficLight>
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _messages;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif