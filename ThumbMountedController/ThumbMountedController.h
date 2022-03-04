//Debug
//#define HID_DEBUG_OUTPUT
//#define FORCE_SENSORS_DEBUG


#define HID_UUID(val)  \
  (const uint8_t[]) {  \
    0xe8, 0x74, 0x2c, 0x65, 0xf0, 0x01, 0x38, 0x95,  \
    0x7a, 0x46, (uint8_t) (val & 0xff), (uint8_t) (val >> 8), 0x02, 0x00, 0x5a, 0x55   \
  }

enum TouchState
{
  IDLING, 
  TOUCHING, 
  FIRST_ZONE_TOUCHING, 
  FIRST_ZONE_LEFT, 
  FIRST_ZONE_RIGHT, 
  FIRST_ZONE_START_PRESS,
  FIRST_ZONE_QUICK_PRESS,
  FIRST_ZONE_PRESS_DOWN, 
  SECOND_ZONE_TOUCHING,
  SECOND_ZONE_LEFT, 
  SECOND_ZONE_RIGHT, 
  SECOND_ZONE_START_PRESS, 
  SECOND_ZONE_QUICK_PRESS, 
  SECOND_ZONE_PRESS_DOWN, 
  TOUCH_RELEASE, 
  UP, 
  DOWN, 
};

// Pressure 
#define NONE_CONTACT 10 // <10: none contact, just ground noise
#define NORMAL_TOUCH 50 // 50-599: normal touch
#define HARD_PRESS 600 // >= 600: hard press


// State 
#define SWIPING_UPDATE_INTERVAL 2000
#define TOUCH_RELEASE_DURATION 100
#define SWIPING_ANGLE 15 // degrees
#define MINIMUM_PRESS_DOWN_DURATION 250 // miliseconds
