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
void Meter1ISR()
{
    // let our flow meter count the pulses
    Meter1->count();
}

// define an 'interrupt service handler' (ISR) for every interrupt pin you use
void Meter2ISR()
{
    // let our flow meter count the pulses
    Meter2->count();
}

// define an 'interrupt service handler' (ISR) for every interrupt pin you use
void Meter3ISR()
{
    // let our flow meter count the pulses
    Meter3->count();
}

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

void messageHandler(String &topic, String &payload)
{
    Serial.println("incoming: " + topic + " - " + payload);

    StaticJsonDocument<200> doc;
    deserializeJson(doc, payload);
    const char *message = doc["message"];
}

void connectAWS()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.println("Connecting to Wi-Fi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    // Configure WiFiClientSecure to use the AWS IoT device credentials
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);

    // Connect to the MQTT broker on the AWS endpoint we defined earlier
    client.begin(AWS_IOT_ENDPOINT, 8883, net);

    // Create a message handler
    client.onMessage(messageHandler);

    Serial.print("Connecting to AWS IOT");

    while (!client.connect(THINGNAME))
    {
        Serial.print(".");
        delay(100);
    }

    if (!client.connected())
    {
        Serial.println("AWS IoT Timeout!");
        return;
    }

    // Subscribe to a topic
    client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

    Serial.println("AWS IoT Connected!");
}

void publishMessage()
{
    StaticJsonDocument<200> doc;
    doc["time"] = millis();
    doc["inlet_flowrate"] = Meter1->getCurrentFlowrate();
    doc["kitchen_sink_flowrate"] = Meter2->getCurrentFlowrate();
    doc["shower_flowrate"] = Meter3->getCurrentFlowrate();
    doc["inlet_volume"] = Meter1->getTotalVolume();
    doc["kitchen_sink_volume"] = Meter2->getTotalVolume();
    doc["shower_volume"] = Meter3->getTotalVolume();
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer); // print to client

    client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void setup()
{
    // prepare serial communication
    Serial.begin(115200);
    connectAWS();

    // get a new FlowMeter instance for an uncalibrated flow sensor and let them attach their 'interrupt service handler' (ISR) on every rising edge
    Meter1 = new FlowMeter(digitalPinToInterrupt(2), UncalibratedSensor, Meter1ISR, RISING);

    // do this setup step for every  FlowMeter and ISR you have defined, depending on how many you need
    Meter2 = new FlowMeter(digitalPinToInterrupt(4), UncalibratedSensor, Meter2ISR, RISING);

    // do this setup step for every  FlowMeter and ISR you have defined, depending on how many you need
    Meter3 = new FlowMeter(digitalPinToInterrupt(5), UncalibratedSensor, Meter3ISR, RISING);
}

void loop()
{
    // wait between output updates
    delay(period);

    // process the (possibly) counted ticks
    Meter1->tick(period);
    Meter2->tick(period);
    Meter3->tick(period);

    // output some measurement result
    Serial.println("Meter 1 currently " + String(Meter1->getCurrentFlowrate()) + " l/min, " + String(Meter1->getTotalVolume()) + " l total.");
    Serial.println("Meter 2 currently " + String(Meter2->getCurrentFlowrate()) + " l/min, " + String(Meter2->getTotalVolume()) + " l total.");
    Serial.println("Meter 3 currently " + String(Meter3->getCurrentFlowrate()) + " l/min, " + String(Meter3->getTotalVolume()) + " l total.");

    publishMessage();
    client.loop();
}