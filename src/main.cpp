#include <Arduino.h>
#include <FlowMeter.h>
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

// connect a flow meter to an interrupt pin (see notes on your Arduino model for pin numbers)
FlowMeter *Meter1;
FlowMeter *Meter2;
FlowMeter *Meter3;

// set the measurement update period to 1s (1000 ms)
const unsigned long period = 1000;

// define an 'interrupt service handler' (ISR) for every interrupt pin you use
void Meter1ISR() {
    // let our flow meter count the pulses
    Meter1->count();
}

// define an 'interrupt service handler' (ISR) for every interrupt pin you use
void Meter2ISR() {
    // let our flow meter count the pulses
    Meter2->count();
}

// define an 'interrupt service handler' (ISR) for every interrupt pin you use
void Meter3ISR() {
    // let our flow meter count the pulses
    Meter3->count();
}

//The MQTT topics that this device should publish/subscribe

void setup() {
  // prepare serial communication
    Serial.begin(115200);

    // get a new FlowMeter instance for an uncalibrated flow sensor and let them attach their 'interrupt service handler' (ISR) on every rising edge
    Meter1 = new FlowMeter(digitalPinToInterrupt(2), UncalibratedSensor, Meter1ISR, RISING);
    
    // do this setup step for every  FlowMeter and ISR you have defined, depending on how many you need
    Meter2 = new FlowMeter(digitalPinToInterrupt(4), UncalibratedSensor, Meter2ISR, RISING);

    // do this setup step for every  FlowMeter and ISR you have defined, depending on how many you need
    Meter3 = new FlowMeter(digitalPinToInterrupt(5), UncalibratedSensor, Meter3ISR, RISING);
}

void loop() {
      // wait between output updates
    delay(period);

    // process the (possibly) counted ticks
    Meter1->tick(period);
    Meter2->tick(period);
    Meter3->tick(period);

    // output some measurement result
    Serial.println("Meter 1 currently " + String(Meter1->getCurrentFlowrate()) + " l/min, " + String(Meter1->getTotalVolume())+ " l total.");
    Serial.println("Meter 2 currently " + String(Meter2->getCurrentFlowrate()) + " l/min, " + String(Meter2->getTotalVolume())+ " l total.");
    Serial.println("Meter 3 currently " + String(Meter3->getCurrentFlowrate()) + " l/min, " + String(Meter3->getTotalVolume())+ " l total.");



}