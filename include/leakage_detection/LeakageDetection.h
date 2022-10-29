#ifndef LEAKAGEDETECTION_H
#define LEAKAGEDETECTION_H

#include <Arduino.h>

class LeakageDetection{
    private: float initial_flowrate;
    private: float normal_flowrate;
    private: float leak_flowrate;
    private: float normal_velocity;
    private: float leak_velocity;
    private: float acceleration;
    private: float leak_location;

    void Calculate_normal_velocity();
    void Calculate_leak_velocity();
    void Calculate_acceleration();
    void Calculate_location_of_leak();
};

#endif