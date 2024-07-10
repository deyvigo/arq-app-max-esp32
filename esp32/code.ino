const char* ssid = "TP-Link_ED40";
const char* password = "72277232";

#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include <WiFi.h>

const char* websockets_connection_string = "192.168.18.11";

#include "MAX30105.h"
#include "heartRate.h"

#define BUTTON_PIN 13
#define FALL_THRESHOULD 15.0
#define INACTIVITY_DURATION 10000 // 10 seconds
#define INACTIVITY_THRESHOLD 1.0
#define BPM_THRESHOLD 50

MAX30105 particleSensor;

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
Adafruit_MPU6050 mpu;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

//Estado del sistema
bool systemActivated = false;

//
float gyroX, gyroY, gyroZ;
float accX, accY, accZ;
float temperature;

//Gyroscope sensor deviation
float gyroXerror = 0.09;
float gyroYerror = 0.03;
float gyroZerror = 0.01;

//Aceleracion sensor error
float accXerror = 0;
float accYerror = 0;
float accZerror = 0;

//Alarma de emergencia
bool emergencyBPM = false;
bool emergencyFall = false;
bool emergencyInactive = false;

long lastMovementTime = 0;
bool isInactive = false;

unsigned long lastResetTime = 0;

using namespace websockets;

// Calibrar el acelerometro
void calibrateAccelerometer() {
  const int numReadings = 100;
  float sumAccX = 0;
  float sumAccY = 0;
  float sumAccZ = 0;

  for (int i = 0; i < numReadings; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    sumAccX += a.acceleration.x;
    sumAccY += a.acceleration.y;
    sumAccZ += a.acceleration.z;
    delay(10);
  }

  accXerror = sumAccX / numReadings;
  accYerror = sumAccY / numReadings;
  accZerror = (sumAccZ / numReadings) - 9.81; // Ajustar para la gravedad

  Serial.println("Calibración completada:");
}

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

void resetVariables() {
  beatsPerMinute = 0;
  beatAvg = 0;
  rateSpot = 0;
  lastBeat = 0;

  gyroX = 0;
  gyroY = 0;
  gyroZ = 0;
  accX = 0;
  accY = 0;
  accZ = 0;
  temperature = 0;

  emergencyBPM = false;
  emergencyInactive = false;
  emergencyFall = false;

  lastMovementTime = 0;
  isInactive = false;

  for (byte i = 0; i < RATE_SIZE; i++) {
    rates[i] = 0;
  }

  Serial.println("Variables reseteadas");
}

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

  //Configrura el boton
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Inicializa el sensor MPU6050
  if (!mpu.begin()) {
    Serial.println("No se pudo encontrar el MPU6050, revisa tu conexion!");
    while (1) delay(10);
  }
  Serial.println("MPU6050 encontrado!");
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  calibrateAccelerometer();
}

void loop() {
  // Leer el estado del botón
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50);  // Debounce del botón
    if (digitalRead(BUTTON_PIN) == LOW) {
      systemActivated = !systemActivated;  // Alternar el estado del sistema
      Serial.print("Sistema ");
      Serial.println(systemActivated ? "activado" : "desactivado");

      resetVariables();

      delay(500);  // Pequeña pausa para evitar múltiples alternancias
    }
  }

  long irValue = particleSensor.getIR();
  bool finger = false;
 
  StaticJsonDocument<200> jsonDoc;

  if(systemActivated){
    
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
    
    if (irValue > 50000) {
      finger = true;
    }

    // Log values for debugging
    Serial.print("IR: ");
    Serial.print(irValue);
    Serial.print(" BPM: ");
    Serial.print(beatsPerMinute);
    Serial.print(" Avg BPM: ");
    Serial.println(beatAvg);

    // Leer datos del MPU6050
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    float gyroX_temp = g.gyro.x;
    if(abs(gyroX_temp) > gyroXerror)  {
      gyroX += gyroX_temp/50.00;
    }

    float gyroY_temp = g.gyro.y;
    if(abs(gyroY_temp) > gyroYerror) {
      gyroY += gyroY_temp/70.00;
    }

    float gyroZ_temp = g.gyro.z;
    if(abs(gyroZ_temp) > gyroZerror) {
      gyroZ += gyroZ_temp/90.00;
    }

    // Obtener los valores de la aceleracion
    accX = a.acceleration.x - accXerror;
    accY = a.acceleration.y - accYerror;
    accZ = a.acceleration.z - accZerror;

    temperature = temp.temperature;

    // Calular la magnitud de la aceleracion
    float accMagnitude = sqrt(accX * accX + accY * accY + accZ * accZ);

    // Detectar caidas del usuario
    if (accX > FALL_THRESHOULD || accY > FALL_THRESHOULD || accZ > FALL_THRESHOULD){
      emergencyFall = true;
      Serial.println("Caída detectada!");
    }

    // Verificar BPM alto
    if (beatsPerMinute > BPM_THRESHOLD){
      emergencyBPM = true;
      Serial.println("BPM alto detectado!");
    }

    // Verificar Inactividad
    if (abs(accMagnitude - 9.81) < INACTIVITY_THRESHOLD) { // Ajuste para gravedad
      if (!isInactive) {
        lastMovementTime = millis(); // Iniciar el temporizador de inactividad
        isInactive = true;
      } else if (millis() - lastMovementTime > INACTIVITY_DURATION) {
        emergencyInactive = true;
        Serial.println("Inactividad detectada!");
      }
    } else {
      isInactive = false;
    }

    // Imprimir los valores del acelerómetro y giroscopio
    Serial.print("Aceleración X: "); Serial.print(accX); Serial.print(" m/s^2");
    Serial.print("\tY: "); Serial.print(accY); Serial.print(" m/s^2");
    Serial.print("\tZ: "); Serial.print(accZ); Serial.println(" m/s^2");

    Serial.print("Giroscopio X: "); Serial.print(gyroX); Serial.print(" rad/s");
    Serial.print("\tY: "); Serial.print(gyroY); Serial.print(" rad/s");
    Serial.print("\tZ: "); Serial.print(gyroZ); Serial.println(" rad/s");

    jsonDoc["ir"] = irValue;
    jsonDoc["bpm"] = beatsPerMinute;
    jsonDoc["avg_bpm"] = beatAvg;
    jsonDoc["finger"] = finger;
    jsonDoc["acceleration_x"] = accX;
    jsonDoc["acceleration_y"] = accY;
    jsonDoc["acceleration_z"] = accZ;
    jsonDoc["gyro_x"] = gyroX;
    jsonDoc["gyro_y"] = gyroY;
    jsonDoc["gyro_z"] = gyroZ;
    jsonDoc["temperature"] = temperature;
    jsonDoc["emergencyBPM"] = emergencyBPM;
    jsonDoc["emergencyFall"] = emergencyFall;
    jsonDoc["emergencyInactive"] = emergencyInactive;
  }
  
  
  jsonDoc["systemActive"] = systemActivated;
  
  // Serializar el JSON a una cadena
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Enviar el JSON al servidor
  client.send(jsonString);

  // let the websockets client check for incoming messages
  if(client.available()) {
    client.poll();
  }

  if (millis() - lastResetTime > 1000) {
    emergencyBPM = false;
    emergencyFall = false;
    emergencyInactive = false;
    lastResetTime = millis();
    Serial.println("Estados de emergencia reseteados");
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
