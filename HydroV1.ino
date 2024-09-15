#include "arduino_secrets.h"

/*

*/
// CAN
#include "AA_MCP2515.h"

// Temperature
#include <OneWire.h>
#include <DallasTemperature.h>

#define DEBUG 

// capteur eau
#define EAU_PIN 4
// capteur ultrason
#define TRIG_PIN 5
#define ECHO_PIN 6
// capteur TDS
#define TdsSensorPin A1
#define VREF 5.0
#define SCOUNT  30
// Capteur temperature
#define ONE_WIRE_BUS 3


// CAN BUS
const CANBitrate::Config CAN_BITRATE = CANBitrate::Config_8MHz_500kbps;
const uint8_t CAN_PIN_CS = 10;
const int8_t CAN_PIN_INT = 2;
CANConfig config(CAN_BITRATE, CAN_PIN_CS, CAN_PIN_INT);
CANController CAN(config);

// Temperature
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


void setup() {
  // Initialize pin
  pinMode(EAU_PIN, INPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(TdsSensorPin, INPUT);
  // Initialize built in pin
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Initialize CAN
  while(CAN.begin(CANController::Mode::Normal) != CANController::OK) {
    Serial.println("CAN begin FAIL - delaying for 1 second");
    delay(1000);
  }

  // Initialize temperature
  sensors.begin();

  
  #ifdef DEBUG
    // Initialize serial and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
      ;  // wait for serial port to connect. Needed for native USB port only
    }
    // prints title with ending line break
    Serial.println("Setup done");
  #endif
}

void BlinkLED_BUILTIN(){
  if(digitalRead(LED_BUILTIN) == 0){
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

// lire la valeur du dÃ©tecteur de presence d'eau
bool GetCapteurEau(){
  bool val = digitalRead(EAU_PIN); // 0: pas d'eau dÃ©tectÃ©e, 1: eau dÃ©tectÃ©e
  return val;
}

// mesurer la distance avec le capteur ultrason
float GetCapteurSonar(){
  float duration, cm;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  cm = (duration / 2) *0.0343;
  return cm;
}

// mesurer la valeur TDS
float GetCapteurTDS(float temperature){
  int analogBuffer[SCOUNT];
  int analogBufferTemp[SCOUNT];
  int analogBufferIndex = 0, copyIndex = 0;
  float averageVoltage = 0, tdsValue = 0;

  for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++){
    delayMicroseconds(40);
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin); 
    analogBufferIndex++;
  }
  for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++){
    analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
  }
  averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0;   
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
  float compensationVolatge = averageVoltage / compensationCoefficient;
  tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge);
  return tdsValue;
}

// mesurer la temperature
float GetCapteurTemp (){
  float Temperature = 0;
  sensors.requestTemperatures();
  Temperature = sensors.getTempCByIndex(0);
  if(Temperature != DEVICE_DISCONNECTED_C) {
    return Temperature;
  } 
  else {
    #ifdef DEBUG
    Serial.println("Error: Could not read temperature data");
    #endif
    return 0;
  }  
}

int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}

void loop() {
  bool EauDetectee = GetCapteurEau();
  float Temperature = GetCapteurTemp();
  uint8_t data1[] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
  data1[0] = EauDetectee;
  data1[1] = ((uint8_t*)&Temperature)[0];
  data1[2] = ((uint8_t*)&Temperature)[1];
  data1[3] = ((uint8_t*)&Temperature)[2];
  data1[4] = ((uint8_t*)&Temperature)[3];
  CANFrame frame1(0x100, data1, sizeof(data1));
  CAN.write(frame1);


  float DistanceSonar = GetCapteurSonar();
  float TDS = GetCapteurTDS(Temperature);
  uint8_t data2[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  data2[0] = ((uint8_t*)&DistanceSonar)[0];
  data2[1] = ((uint8_t*)&DistanceSonar)[1];
  data2[2] = ((uint8_t*)&DistanceSonar)[2];
  data2[3] = ((uint8_t*)&DistanceSonar)[3];
  data2[4] =((uint8_t*)&TDS)[0];
  data2[5] =((uint8_t*)&TDS)[1];
  data2[6] =((uint8_t*)&TDS)[2];
  data2[7] =((uint8_t*)&TDS)[3];
  CANFrame frame2(0x200, data2, sizeof(data2));
  CAN.write(frame2);
  

  // Debug
  #ifdef DEBUG
    // blink built in led
    BlinkLED_BUILTIN();
    // serial output
    Serial.println((String)"Eau:" + EauDetectee + (String)" Temp:" + Temperature + (String)" Dist:" + DistanceSonar+ (String)"cm" + (String)" TDS:" + TDS + (String)"us");
    frame1.print("TX");
    frame2.print("TX");
    Serial.println();
  #endif

  // attendre une seconde
  delay(5000);
}

