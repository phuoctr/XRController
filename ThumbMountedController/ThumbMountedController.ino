#include <bluefruit.h>
#include "ThumbMountedController.h"
#include <Adafruit_LSM6DS33.h>  // Accel + gyro

//Sensors definition
Adafruit_LSM6DS33 lsm6ds33; // Gyro and Accel

// BLE Service
BLEService        service                    (HID_UUID(0x0000));
BLECharacteristic deviceInfoCharacteristic  (HID_UUID(0x0001));
BLECharacteristic hidCharacteristic (HID_UUID(0x0002));

int state = IDLING;
int timer0 = TOUCH_RELEASE_DURATION; // timer for touch releasing
int timer1 = SWIPING_UPDATE_INTERVAL; // time slot for angle calc
int timer2 = MINIMUM_PRESS_DOWN_DURATION; // timer for press sensing

// timer references
uint32_t timerRef0; // timestamp for touch releasing
uint32_t timerRef1; // timestamp for angle calc
uint32_t timerRef2; // timestamp for pressing

// touch release
bool firstPointRelease = false;
bool secondPointRelease = false;

//z Axis rotation
float zAxisRotation;

void setupSensors(void)
{
  
  lsm6ds33.begin_I2C(); // begin accel + gyro
  // set lowest range
  lsm6ds33.setGyroRange(LSM6DS_GYRO_RANGE_500_DPS);
  // Increase I2C speed to 400 Khz
  Wire.setClock(400000);
}

void setupBLE(void){
  const char deviceName[13] = "XRController";
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName(deviceName);
  service.begin();
  
  Bluefruit.Periph.setConnInterval(16, 24);
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  hidCharacteristic.setProperties(CHR_PROPS_NOTIFY);
  hidCharacteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  hidCharacteristic.setFixedLen(1 * sizeof(byte)); //  data length for state, 1 byte
  hidCharacteristic.begin();
   
  }
  
void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(service);
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}




uint8_t updateState(void){
  // read the input on analog pin 0:
  int AIN0 = analogRead(A0); // first force sensor
  int AIN1 = analogRead(A1); // second force sensor
  
  sensors_event_t event; // gyro event
  lsm6ds33.getGyroSensor()->getEvent(&event);
  
  switch (state) {

    case IDLING: // Default state, listen for change on first and second force sensors
      if (AIN0 >= NORMAL_TOUCH || AIN1 >= NORMAL_TOUCH) state = TOUCHING;
      break;

    case TOUCHING: // When contact is detected on at least one force sensor
      if (AIN0 >= NORMAL_TOUCH && AIN1 >= NORMAL_TOUCH) break; // Not yet built
      // first point sensing
      if (AIN0 >= NORMAL_TOUCH && AIN0 < HARD_PRESS) { // Light-medium pressure on 1s force sensor
        state = FIRST_ZONE_TOUCHING; // Set state to swiping
        timerRef1 = millis(); // Start time reference for calculating z Axis rotation
      }
      else if (AIN0 >= HARD_PRESS){ 
        state = FIRST_ZONE_START_PRESS; // Hard press on first force sensor
        timerRef2 = millis();
      }
      // second point sensing
      if (AIN1 >= NORMAL_TOUCH && AIN1 < HARD_PRESS) { // Light-medium pressure on 2 force sensor
        state = SECOND_ZONE_TOUCHING; // Set state to swiping
        timerRef1 = millis();
      }
      else if (AIN1 >= HARD_PRESS) {
        state = SECOND_ZONE_START_PRESS; // Hard press on first force sensor
        timerRef2 = millis();
      }
      break;

    case FIRST_ZONE_TOUCHING:
      if (AIN0 <= NONE_CONTACT) { 
        // pressure down below 10 => touch release
        state = TOUCH_RELEASE;
        firstPointRelease = true;
        timerRef0 = millis(); // start timer
      }
      else if (AIN0 >= HARD_PRESS) { // pressure > 600 => pressing
        state = FIRST_ZONE_START_PRESS;
        timerRef2 = millis(); // timer ref for pressing
      }
      else {
        if (timer1 > 0) {
          zAxisRotation += event.gyro.z * SENSORS_RADS_TO_DPS * (millis() - timerRef1) / 1000;          // Integration from angular velocity to rotation
          timer1 -= millis() - timerRef1;
          timerRef1 = millis();
          if (zAxisRotation >= SWIPING_ANGLE) {
            state = FIRST_ZONE_LEFT;
            zAxisRotation = 0;
          }
          else if (zAxisRotation <= -SWIPING_ANGLE) {
            state = FIRST_ZONE_RIGHT;
            zAxisRotation = 0;
          }
        }
        else {
          timer1 = SWIPING_UPDATE_INTERVAL;
          zAxisRotation = 0;
        }
      }
      break;

    case FIRST_ZONE_LEFT:
      state = TOUCHING;
      break;

    case FIRST_ZONE_RIGHT:
      state = TOUCHING;
      break;

    case FIRST_ZONE_START_PRESS:
//      // If pressure down to 10 and below, just switch to switch release
      if (AIN0 <= NONE_CONTACT){ 
        state = IDLING;
        break;
      }
      // The timer count down from 500
      if (timer2 > 0) {
        timer2 -= millis() - timerRef2; //On first time, the timerRef2 was initialized on TOUCHING
        timerRef2 = millis(); //Re-initialized for next loop
        if (AIN0 < HARD_PRESS) state = FIRST_ZONE_QUICK_PRESS; //if pressing shorter than 500ms => press once
      }
      else { 
          timer2 = 0;
          state = FIRST_ZONE_PRESS_DOWN;
      }
      break;

    case FIRST_ZONE_QUICK_PRESS:
      // Do something with single tap
      timer2 = MINIMUM_PRESS_DOWN_DURATION; // reset timer 2
      state = TOUCHING;
      break;

    case FIRST_ZONE_PRESS_DOWN:
      // keep state until force below 600
      if(AIN0 < HARD_PRESS) {
        state = TOUCHING;
        timer2 = MINIMUM_PRESS_DOWN_DURATION; // reset timer 2
        }
      break;

    case SECOND_ZONE_TOUCHING:
      if (AIN1 <= NONE_CONTACT) {
        state = TOUCH_RELEASE;
        secondPointRelease = true;
        timerRef0 = millis();
      }
      else if (AIN1 >= HARD_PRESS) {
        state = SECOND_ZONE_START_PRESS;
        timerRef2 = millis();
      }
      else {
        if (timer1 > 0) {
          zAxisRotation += event.gyro.z * SENSORS_RADS_TO_DPS * (millis() - timerRef1) / 1000;
          timer1 -= millis() - timerRef1;
          timerRef1 = millis();
          if (zAxisRotation >= SWIPING_ANGLE/2) {
            state = SECOND_ZONE_LEFT;
            zAxisRotation = 0;
            Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
          }
          else if (zAxisRotation <= -SWIPING_ANGLE/3) {
            state = SECOND_ZONE_RIGHT;
            zAxisRotation = 0;
            Serial.println("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
          }
        }
        else {
          timer1 = SWIPING_UPDATE_INTERVAL;
          zAxisRotation = 0;
        }
      }
      break;

    case SECOND_ZONE_LEFT:
      //Do sth
      state = TOUCHING;
      break;

    case SECOND_ZONE_RIGHT:
      //Do sth
      state = TOUCHING;
      break;

    case SECOND_ZONE_START_PRESS:
//      // If pressure down to 10 and below, just switch to switch release
      if (AIN1 <= NONE_CONTACT){ 
        state = IDLING;
        break;
      }
//      // The timer count down for the duration of defined minimum press down duration
      if (timer2 > 0) {
        timer2 -= millis() - timerRef2; //On first time, the timerRef2 was initialized on TOUCHING
        timerRef2 = millis(); //Re-initialized for next loop
        if (AIN1 < HARD_PRESS) state = SECOND_ZONE_QUICK_PRESS; //if pressing shorter than 100ms => press once
      }
      else { 
        timer2 = 0;
        state = SECOND_ZONE_PRESS_DOWN;
      }
      break;

    case SECOND_ZONE_QUICK_PRESS:
      // Do something with single tap
      timer2 = MINIMUM_PRESS_DOWN_DURATION; // reset timer 2
      state = TOUCHING;
      break;

    case SECOND_ZONE_PRESS_DOWN:
      // keep state until force below 600
      if(AIN1 < HARD_PRESS) {
        state = TOUCHING;
        timer2 = MINIMUM_PRESS_DOWN_DURATION; // reset timer 2
        }
      break;
      
    case TOUCH_RELEASE:
      timer0 -= (millis() - timerRef0);
      timerRef0 = millis();
      if (timer0 <= 0){
          timer0 = TOUCH_RELEASE_DURATION;
          state = IDLING;
          // After touch release interval without any touch, reset first/secondTouchRelease bool
          firstPointRelease = false; 
          secondPointRelease = false;
        }
      else{
        if(firstPointRelease && AIN1 > NONE_CONTACT){
          firstPointRelease = false;
          timer0 = TOUCH_RELEASE_DURATION;
          state = UP;
          break;
          }
        if(secondPointRelease && AIN0 >= NONE_CONTACT){
          secondPointRelease = false;
          timer0 = TOUCH_RELEASE_DURATION;
          state = DOWN;
          break;
          }
        }
      break;

    case UP:
        state = IDLING;
        break;

    case DOWN:
        state = IDLING;
        break;
  }
  
  //Plot
  #if defined(HID_DEBUG_OUTPUT)
  Serial.print(AIN0);
  Serial.print(",");
  Serial.print(AIN1);
  Serial.print(",");
  //Serial.print(timer0);
  //Serial.print(",");
  Serial.println(state * 100);
  #endif

  #if defined(FORCE_SENSORS_DEBUG)
  Serial.print(AIN0);
  Serial.print(",");
  Serial.print(AIN1);
  Serial.print(",");
  Serial.println(timer2);
  #endif


  return state;
  }
  
void updateSubscribedCharacteristics(void)
{
  if(hidCharacteristic.notifyEnabled()) {
    byte data[1];
    data[0] = updateState();
    hidCharacteristic.notify(data, hidCharacteristic.getMaxLen()); //notify state of the periph
  }
}
void setup() {
  setupSensors();
  setupBLE();
  startAdv();
}

void loop() {  
  if (Bluefruit.connected())
  {
    updateSubscribedCharacteristics();
  }
  
#if defined(HID_DEBUG_OUTPUT) || defined(FORCE_SENSORS_DEBUG)
   updateState();
   delay(10);
#endif
}

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}
