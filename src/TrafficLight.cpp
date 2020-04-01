#include <iostream>
#include <random>
#include "TrafficLight.h"



/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _condition.wait(uLock, [this]{return !_queue.empty();});

    T msg = std::move(_queue.front());
    std::cout<<"queue length:"<<_queue.size()<<std::endl;
    _queue.pop_front();
    return std::move(msg);
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.emplace_back(msg);
    _condition.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    // std::cout<<"Created Traffic Light #"<<getID()<<std::endl;
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while(true){
        TrafficLightPhase signlaChange = _messages.receive();
        if(signlaChange==TrafficLightPhase::green) return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
        threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    std::random_device rd; 
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> dist(4, 6);
    double cycleDuration = dist(eng);
    std::chrono::system_clock::time_point lastUpdate;

    //init lastUpdate
    lastUpdate = std::chrono::system_clock::now();
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        long duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if(duration > cycleDuration){
            //Toggle current phase
            TrafficLightPhase currentPhase = (_currentPhase==TrafficLightPhase::red? TrafficLightPhase::green:TrafficLightPhase::red);
            _currentPhase=currentPhase;
            std::cout<<"Traffic Light #"<<getID() <<"turning to "<<(_currentPhase==TrafficLightPhase::red?"red":"green")<<":"<<(currentPhase==TrafficLightPhase::red?"red":"green")<<std::endl;
            _messages.send(std::move(currentPhase));
            cycleDuration = dist(eng);
            lastUpdate = std::chrono::system_clock::now();    
        }        
    }
}
