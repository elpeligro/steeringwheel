// Program used to test the USB Joystick library when used as 
// a Flight Controller on the Arduino Leonardo or Arduino 
// Micro.
//
// Matthew Heironimus
// 2016-05-29 - Original Version
//------------------------------------------------------------

#include "Joystick.h"


#define joyX A0
#define joyY A1
#define joyZ A2
#define joyThrottle A3

#define steering A1
#define accel A3
#define brake A4
#define wheelSwitch A5




#define joyRangeGap 150
#define steerRangeGap 100
#define xRangeGap 100
#define accelRangeGap 400
#define brakeRangeGap 400
#define yRangeGap 200


//  Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
//  JOYSTICK_TYPE_JOYSTICK, 14, 0,
//  true, true, true, false, false, false,
//  false, false, true, true, true);

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 15, 0,
  true, true, true, true, true, true,
  true, true, true, true, true);


// Set to true to test "Auto Send" mode or false to test "Manual Send" mode.
//const bool testAutoSendMode = true;
const bool testAutoSendMode = false;

const unsigned long gcCycleDelta = 1000;
const unsigned long gcAnalogDelta = 25;
const unsigned long gcButtonDelta = 500;
unsigned long gNextTime = 0;
unsigned int gCurrentStep = 0;  
unsigned int lastHatStatus = 0 ;
int lastHatButton = -1 ;

int lastButtonState[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  };
int mode = 0   ;

#define dpadUpPin 3
#define dpadDownPin 5
#define dpadLeftPin 2
#define dpadRightPin 4
#define gearUpPin 0
#define gearDownPin 1
#define AbuttonPin 11
#define BbuttonPin 9
#define RbuttonPin 10
#define IbuttonPin 7
#define IIbuttonPin 6
#define LbuttonPin 12
#define selectPin 13
#define startPin 8
#define wheelSwitchPin A5

#define dpadUpButton 3
#define dpadDownButton 5
#define dpadLeftButton 6
#define dpadRightButton 4
#define gearUpButton 1
#define gearDownButton 2
#define AbuttonButton 7
#define BbuttonButton 9
#define RbuttonButton 11
#define IbuttonButton 8
#define IIbuttonButton 10
#define LbuttonButton 12
#define selectButton 13
#define startButton 14
#define wheelSwitchButton 15

#define selectButtonIndex 12
#define AButtonIndex 6
#define BButtonIndex 8
#define RButtonIndex 11







void checkAndSetButton( unsigned int pin,  unsigned int joyButton)
{
  int currentButtonState = !digitalRead(pin);
  int index = joyButton -1 ;
  if  (currentButtonState != lastButtonState[index])   
  {
      Joystick.setButton(index, currentButtonState);
      lastButtonState[index] = currentButtonState;
  }

}

void checkWheelSwitch( unsigned int pin,  unsigned int joyButton)
{
  int currentButtonState = !digitalRead(pin);
  int index = joyButton -1 ;
  if  (currentButtonState != lastButtonState[index])   
  {
      Joystick.setButton(index, currentButtonState);
      lastButtonState[index] = currentButtonState;
  }

}



void check_and_set_mode() {
  unsigned int selIndex = selectButton -1;
   
  if (!digitalRead(selectPin))
  {
    Joystick.setXAxis(0);
    if (!digitalRead(AbuttonPin))
    {
      mode = 0;
    }
    else if (!digitalRead(BbuttonPin))
    {
      mode = 1;
    }
    else if (!digitalRead(RbuttonPin))
    {
      mode = 2;
    }
  }
}


void setup() {


    // Initialize Button Pins
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);  
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  //pinMode(16, INPUT_PULLUP);
  //pinMode(14, INPUT_PULLUP);
  //pinMode(15, INPUT_PULLUP);

  pinMode(A0, INPUT);  // Nothing
  pinMode(A1, INPUT);  // Steering
  pinMode(A2, INPUT);  // Nothing
  pinMode(A3, INPUT);  // Throttle analogue
  pinMode(A4, INPUT);  // brake
  pinMode(A5, INPUT);  // switch on wheel

  Joystick.setXAxisRange(0, 1023 - steerRangeGap - steerRangeGap);
  //Joystick.setYAxisRange(0, 1023 - accelRangeGap - accelRangeGap);
  Joystick.setYAxisRange(0, 1023 - yRangeGap - yRangeGap);
  Joystick.setThrottleRange( 0, 1023 - accelRangeGap - accelRangeGap);
  Joystick.setRzAxisRange(0, 1023 - brakeRangeGap - brakeRangeGap);

  Joystick.setSteeringRange(0, 1023 - steerRangeGap - steerRangeGap);
  Joystick.setAcceleratorRange(0, 1023 - accelRangeGap - accelRangeGap);
  Joystick.setBrakeRange(0, 1023 - brakeRangeGap - brakeRangeGap);
  
  if (testAutoSendMode)
  {
    Joystick.begin();
  }
  else
  {
    Joystick.begin(false);
  }
      
}


void loop() {


  int index = 0;
  int currentButtonState = 0;

  int steerValue = analogRead(steering);
  int accelValue = analogRead(accel);
  int brakeValue = analogRead(brake);
  int wheelValue = analogRead(wheelSwitch);
  int brakeVsAccel;
  int yAxisValue;
  
  steerValue = 1024- steerValue;
  accelValue = 1024- accelValue;
  brakeValue = 1024- brakeValue;
    //yAxisValue = 512;
  
  
  steerValue =  steerValue - steerRangeGap;
  
  accelValue =  accelValue - accelRangeGap;
  brakeValue =  brakeValue - brakeRangeGap;

  yAxisValue = accelValue ;

  

  

  if (wheelValue <400 )    // if the switch is on then disable the external pedals i.e. pedals plugged in elsewhere
  {
    accelValue = 0;
    brakeValue = 0;
    int index = wheelSwitchButton - 1;
    if  (0 == lastButtonState[index])   
    {
      Joystick.setButton(index, 1);
      lastButtonState[index] = 1;
    }
  }
  else
  {
    int index = wheelSwitchButton - 1;
    if  (1 == lastButtonState[index])   
    {
      Joystick.setButton(index, 0);
      lastButtonState[index] = 0;
    }
  }
  
  if (mode == 0 )  // X for steering   Throttle for Accel RZ for Brake
  {   
    yAxisValue = 0;
    Joystick.setXAxis(steerValue);
    Joystick.setYAxis(512 - yRangeGap);
    Joystick.setThrottle(accelValue);
    Joystick.setRzAxis(brakeValue);
    Joystick.setSteering(512);
    Joystick.setAccelerator(0);
    Joystick.setBrake(0);
  }
  
  if (mode == 1) // X for steering Y for accel + brake
  {
    brakeVsAccel = (brakeValue - accelValue) / 2;
    yAxisValue = 512 + brakeVsAccel;
    yAxisValue = yAxisValue - yRangeGap;
    Joystick.setXAxis(steerValue);
    Joystick.setYAxis(yAxisValue);
    Joystick.setThrottle(0);
    Joystick.setRzAxis(0);
    Joystick.setSteering(512  - steerRangeGap);
    Joystick.setAccelerator(0);
    Joystick.setBrake(0);
 
  }

  if (mode == 2 )  // pure steering wheel steering for steering   accel for Accel brake for Brake
  {
    Joystick.setXAxis(512 - steerRangeGap);
    Joystick.setYAxis(512 - yRangeGap);
    Joystick.setThrottle(0);
    Joystick.setRzAxis(0);
    Joystick.setSteering(steerValue);
    Joystick.setAccelerator(accelValue);
    Joystick.setBrake(brakeValue);
  }
  
  

  checkAndSetButton( dpadUpPin, dpadUpButton);
  checkAndSetButton( dpadDownPin, dpadDownButton);
  checkAndSetButton( dpadLeftPin, dpadLeftButton);
  checkAndSetButton( dpadRightPin, dpadRightButton);
  checkAndSetButton( gearUpPin, gearUpButton);
  checkAndSetButton( gearDownPin, gearDownButton);
  checkAndSetButton( AbuttonPin, AbuttonButton);      
  checkAndSetButton( BbuttonPin, BbuttonButton);
  checkAndSetButton( RbuttonPin, RbuttonButton);
  checkAndSetButton( IbuttonPin, IbuttonButton);
  checkAndSetButton( IIbuttonPin, IIbuttonButton);
    checkAndSetButton( LbuttonPin, LbuttonButton);   
  checkAndSetButton( selectPin, selectButton);   
  checkAndSetButton( startPin, startButton);   
  // checkAndSetButton( wheelSwitchPin, wheelSwitchButton);   
  
  check_and_set_mode();  // check select A/B/R  buttons to set the mode
  
  
    if (testAutoSendMode == false)
    {
      Joystick.sendState();
    }
    

  delay(50);
  
}
