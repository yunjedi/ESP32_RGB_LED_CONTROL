#include <dummy.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

hw_timer_t * timer = NULL;
const int LED = GPIO_NUM_2; // on board blue led. (pin24)
const char ledR = A4;
const char ledG = A5;
const char ledB = A18;
//const int LED = 24;

uint8_t ledArray[3] = {1, 2, 3};

bool deviceConnected = false; // connection-status flag

String receivedCommand = ""; // commands via bluetooth stored here

#define SERVICE_UUID           "0000ffe0-0000-1000-8000-00805f9b34fb" // UART service UUID
#define CHARACTERISTIC_UUID_RX "0000ffe1-0000-1000-8000-00805f9b34fb"
//#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

void IRAM_ATTR onTimer() {
  static byte state = LOW;
  state = ! state;
  digitalWrite(LED, state);
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("connnected to a device!");
    }
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("disconnected!");
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      if (rxValue.length() > 0) {
        Serial.println("Received a command");
        receivedCommand = rxValue.c_str();
      }
    }
};

void setup() {
  // Create the BLE Device
  BLEDevice::init("NHOM 8 JAVA"); // Give it a name

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE
  /*BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
    pCharacteristic->addDescriptor(new BLE2902());*/
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);

  Serial.begin(115200);
  Serial.println("Waiting a client connection to notify...");
  pinMode(LED, OUTPUT);

  ledcAttachPin(ledR, 1); // assign RGB led pins to channels
  ledcAttachPin(ledG, 2);
  ledcAttachPin(ledB, 3);

  ledcSetup(1, 12000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(2, 12000, 8);
  ledcSetup(3, 12000, 8);

}
void loop() {
  if (receivedCommand.length() > 0)
  {
    if (receivedCommand == "BL1")
    {
      blink_1();
    }

    else if (receivedCommand == "ONLED" || receivedCommand == "1")
    {
      digitalWrite(LED, HIGH);
    }

    else if (receivedCommand == "OFFLED" || receivedCommand == "0")
    {
      digitalWrite(LED, LOW);
    }

    else if (receivedCommand == "BLN")
    {
      timerAlarmEnable(timer);
    }

    else if (receivedCommand == "SBL")
    {
      timerAlarmDisable(timer);
    }

    else
    {
      change_color(receivedCommand);
    }

    receivedCommand = "";
  }
  delay(10);
}

void change_color(String cmd)
{

  if (cmd.substring(0, 1) == "R")
  {
    //version 5 convert decimal to hex
//     long long number = strtoll( &cmd[1], NULL, 16);
//     // Split them up into r, g, b values
//     long long R = number >> 16;
//     long long G = number >> 8 & 0xFF;
//     long long B = number & 0xFF;
//     apply_color(R, G, B);
    
    //version 6 convert decimal to char
     char  *char_cmd = &cmd[0]; // assign char dau tien cua string vao con tro char
  
  if (char_cmd[0] == '#')
  {
     int R = (int) char_cmd[1] *255/100;
     int G = (int) char_cmd[2] *255/100;
     int B = (int) char_cmd[3]  *255/100;

    apply_color(R, G, B);
  }
  else
    Serial.println("Invalid command!");

}

void apply_color(unsigned char R, unsigned char G, unsigned char B)
{
  if (R >= 0 && R <= 255)  ledcWrite(1, R);
  else ledcWrite(1, 255);

  if (G >= 0 && G <= 255) ledcWrite(2, G);
  else ledcWrite(2, 255);

  if (B >= 0 && B <= 255) ledcWrite(3, B);
  else ledcWrite(3, 255);

  Serial.println("LED's color changed!");

 Serial.print(R);
 Serial.print(G);
 Serial.print(B);
}

void blink_1()
{
  Serial.println("Received a blinking require command!");
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(500);
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(500);
}
