const char* ssid = "TP-Link_ED40";
const char* password = "72277232";

#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include <WiFi.h>

const char* websockets_connection_string = "192.168.0.110";

#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

using namespace websockets;

void onMessageCallback(WebsocketsMessage message) {
  Serial.print("Got Message: ");
  Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) {
  if(event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connnection Opened");
  } else if(event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connnection Closed");
  } else if(event == WebsocketsEvent::GotPing) {
    Serial.println("Got a Ping!");
  } else if(event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  }
}

WebsocketsClient client;
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Wait some time to connect to wifi
  for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
    Serial.print(".");
    delay(1000);
  }

  // Check if connected to wifi
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("No Wifi!");
    return;
  }

  Serial.println("Connected to Wifi, Connecting to server.");
  // try to connect to Websockets server
  bool connected = client.connect(websockets_connection_string, 3000, "/");
  if(connected) {
    Serial.println("Connected!");
  } else {
    Serial.println("Not Connected!");
  }
  
  // run callback when messages are received
  client.onMessage([&](WebsocketsMessage message){
    Serial.print("Got Message: ");
    Serial.println(message.data());
  });

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

void loop() {
  long irValue = particleSensor.getIR();
  bool finger = true;

  if (checkForBeat(irValue) == true) {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  if (irValue < 50000) {
    finger = false;
  }

  // Log values for debugging
  Serial.print("IR: ");
  Serial.print(irValue);
  Serial.print(" BPM: ");
  Serial.print(beatsPerMinute);
  Serial.print(" Avg BPM: ");
  Serial.println(beatAvg);


  StaticJsonDocument<200> jsonDoc;
  jsonDoc["ir"] = irValue;
  jsonDoc["bpm"] = beatsPerMinute;
  jsonDoc["avg_bpm"] = beatAvg;
  jsonDoc["finger"] = finger;

  // Serializar el JSON a una cadena
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Enviar el JSON al servidor
  client.send(jsonString);
  // let the websockets client check for incoming messages
  if(client.available()) {
    client.poll();
  }
}


// Test code

// const char* ssid = "TP-Link_ED40";
// const char* password = "72277232";

// #include <ArduinoWebsockets.h>
// #include <ArduinoJson.h>
// #include <WiFi.h>

// const char* websockets_connection_string = "192.168.0.110";

// #include "MAX30105.h"
// #include "heartRate.h"
// #include "spo2_algorithm.h"

// MAX30105 particleSensor;

// uint32_t irBuffer[100]; //infrared LED sensor data
// uint32_t redBuffer[100];  //red LED sensor data
// int32_t bufferLength; //data length
// int32_t spo2; //SPO2 value
// int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
// int32_t heartRate; //heart rate value
// int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

// using namespace websockets;

// void onMessageCallback(WebsocketsMessage message) {
//   Serial.print("Got Message: ");
//   Serial.println(message.data());
// }

// void onEventsCallback(WebsocketsEvent event, String data) {
//   if(event == WebsocketsEvent::ConnectionOpened) {
//     Serial.println("Connnection Opened");
//   } else if(event == WebsocketsEvent::ConnectionClosed) {
//     Serial.println("Connnection Closed");
//   } else if(event == WebsocketsEvent::GotPing) {
//     Serial.println("Got a Ping!");
//   } else if(event == WebsocketsEvent::GotPong) {
//     Serial.println("Got a Pong!");
//   }
// }

// WebsocketsClient client;
// void setup() {
//   Serial.begin(115200);
//   WiFi.begin(ssid, password);

//   // Wait some time to connect to wifi
//   for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
//     Serial.print(".");
//     delay(1000);
//   }

//   // Check if connected to wifi
//   if(WiFi.status() != WL_CONNECTED) {
//     Serial.println("No Wifi!");
//     return;
//   }

//   Serial.println("Connected to Wifi, Connecting to server.");
//   // try to connect to Websockets server
//   bool connected = client.connect(websockets_connection_string, 3000, "/");
//   if(connected) {
//     Serial.println("Connected!");
//   } else {
//     Serial.println("Not Connected!");
//   }
  
//   // run callback when messages are received
//   client.onMessage([&](WebsocketsMessage message){
//     Serial.print("Got Message: ");
//     Serial.println(message.data());
//   });

//   // Initialize sensor
//   if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
//   {
//     Serial.println("MAX30105 was not found. Please check wiring/power. ");
//     while (1);
//   }
//   Serial.println("Place your index finger on the sensor with steady pressure.");

//   particleSensor.setup(); //Configure sensor with default settings
//   particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
//   particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
// }

// void loop() {
//   bufferLength = 100; //buffer length of 100 stores 4 seconds of samples running at 25sps

//   //read the first 100 samples, and determine the signal range
//   for (byte i = 0 ; i < bufferLength ; i++) {
//     while (particleSensor.available() == false) {
//       particleSensor.check(); //Check the sensor for new data
//     }
//     redBuffer[i] = particleSensor.getRed();
//     irBuffer[i] = particleSensor.getIR();
//     particleSensor.nextSample(); //We're finished with this sample so move to next sample
//   }

//   maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

//   StaticJsonDocument<200> jsonDoc;
//   jsonDoc["ir"] = irBuffer[bufferLength - 1];
//   jsonDoc["bpm"] = heartRate;
//   jsonDoc["avg_bpm"] = validHeartRate;
//   jsonDoc["spo"] = spo2;

//   // Serializar el JSON a una cadena
//   String jsonString;
//   serializeJson(jsonDoc, jsonString);
//   client.send(jsonString);

//   // // Log values for debugging
//   // Serial.print("IR: ");
//   // Serial.print(irBuffer[bufferLength - 1]);
//   // Serial.print(" BPM: ");
//   // Serial.print(heartRate);
//   // Serial.print(" Avg BPM: ");
//   // Serial.print(heartRate);
//   // Serial.print( " SpO2: ");
//   // Serial.println(spo2);

//   // StaticJsonDocument<200> jsonDoc;
//   // jsonDoc["ir"] = irBuffer[bufferLength - 1];
//   // jsonDoc["bpm"] = heartRate;
//   // jsonDoc["avg_bpm"] = heartRate;
//   // jsonDoc["spo"] = spo2;

//   // // Serializar el JSON a una cadena
//   // String jsonString;
//   // serializeJson(jsonDoc, jsonString);

//   // Enviar el JSON al servidor
//   // client.send(jsonString);
//   // let the websockets client check for incoming messages
//   if(client.available()) {
//     client.poll();
//   }
// }
