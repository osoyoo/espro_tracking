/*  ___   ___  ___  _   _  ___   ___   ____ ___  ____  
 * / _ \ /___)/ _ \| | | |/ _ \ / _ \ / ___) _ \|    \ 
 *| |_| |___ | |_| | |_| | |_| | |_| ( (__| |_| | | | |
 * \___/(___/ \___/ \__  |\___/ \___(_)____)___/|_|_|_|
 *                  (____/ 
 * OSOYOO Espro All-In-One Board   Line Tracking Project
 * Tutorial URL https://osoyoo.com/?p=61226
 * CopyRight www.osoyoo.com

 * After running the code, smart car will go forward 2 seconds, then go backward 2
 * seconds, then left turn for 2 seconds then right turn for 2 seconds then stop. 
 * 
 */
#define speedPinR 16    //  RIGHT PWM pin connect MODEL-X ENA
#define RightMotorDirPin1  23    //Right Motor direction pin 1 to MODEL-X IN1
#define RightMotorDirPin2  25    //Right Motor direction pin 2 to MODEL-X IN2
#define speedPinL 17    // Left PWM pin connect MODEL-X ENB
#define LeftMotorDirPin1  26    //Left Motor direction pin 1 to MODEL-X IN3
#define LeftMotorDirPin2  27   //Left Motor direction pin 1 to MODEL-X IN4
#define SPEED 100

// IR Tracking Sensor Pins (Left to Right)
#define SENSOR_LEFT_2   33    // Far Left
#define SENSOR_LEFT_1   35    // Left
#define SENSOR_CENTER   34    // Center
#define SENSOR_RIGHT_1  39    // Right
#define SENSOR_RIGHT_2  36    // Far Right

// Speed settings for tracking
// Reduced speeds for low-capacity 9V battery to prevent brownout
#define TRACK_SPEED     100    // Normal tracking speed (reduced from 80)
#define TURN_SPEED     90    // Turn speed (reduced from 100)
#define SLOW_SPEED      70    // Slow speed for gentle corrections (reduced from 60)



/*motor control*/
void go_Back(int L_SPEED=SPEED,int R_SPEED=SPEED)  //Forward
{
  digitalWrite(RightMotorDirPin1, HIGH);
  digitalWrite(RightMotorDirPin2,LOW);
  digitalWrite(LeftMotorDirPin1,HIGH);
  digitalWrite(LeftMotorDirPin2,LOW);
  analogWrite(speedPinL,L_SPEED);
  analogWrite(speedPinR,R_SPEED);
}
void go_Right(int L_SPEED=SPEED,int R_SPEED=SPEED)  //Turn left
{
  digitalWrite(RightMotorDirPin1, HIGH);
  digitalWrite(RightMotorDirPin2,LOW);
  digitalWrite(LeftMotorDirPin1,LOW);
  digitalWrite(LeftMotorDirPin2,HIGH);
  analogWrite(speedPinL,L_SPEED);
  analogWrite(speedPinR,R_SPEED);
}
void go_Left(int L_SPEED=SPEED,int R_SPEED=SPEED)  //Turn right
{
  digitalWrite(RightMotorDirPin1, LOW);
  digitalWrite(RightMotorDirPin2,HIGH);
  digitalWrite(LeftMotorDirPin1,HIGH);
  digitalWrite(LeftMotorDirPin2,LOW);
  analogWrite(speedPinL,L_SPEED);
  analogWrite(speedPinR,R_SPEED);
}
void go_Advance(int L_SPEED=SPEED,int R_SPEED=SPEED)  //Reverse
{
  digitalWrite(RightMotorDirPin1, LOW);
  digitalWrite(RightMotorDirPin2,HIGH);
  digitalWrite(LeftMotorDirPin1,LOW);
  digitalWrite(LeftMotorDirPin2,HIGH);
  analogWrite(speedPinL,L_SPEED);
  analogWrite(speedPinR,R_SPEED);
}
void stop_Stop()    //Stop
{
  digitalWrite(RightMotorDirPin1, LOW);
  digitalWrite(RightMotorDirPin2,LOW);
  digitalWrite(LeftMotorDirPin1,LOW);
  digitalWrite(LeftMotorDirPin2,LOW);
  analogWrite(speedPinL,0);
  analogWrite(speedPinR,0);
}

//Pins initialize
void init_GPIO()
{
	pinMode(RightMotorDirPin1, OUTPUT);
	pinMode(RightMotorDirPin2, OUTPUT);
	pinMode(speedPinL, OUTPUT);

	pinMode(LeftMotorDirPin1, OUTPUT);
  pinMode(LeftMotorDirPin2, OUTPUT);
  pinMode(speedPinR, OUTPUT);

  // Initialize IR sensor pins as INPUT
  pinMode(SENSOR_LEFT_2, INPUT);
  pinMode(SENSOR_LEFT_1, INPUT);
  pinMode(SENSOR_CENTER, INPUT);
  pinMode(SENSOR_RIGHT_1, INPUT);
  pinMode(SENSOR_RIGHT_2, INPUT);

	stop_Stop();
}

// Read all 5 IR sensors and return as a string
String readSensors()
{
  int s1 = digitalRead(SENSOR_LEFT_2);   // Far Left
  int s2 = digitalRead(SENSOR_LEFT_1);   // Left
  int s3 = digitalRead(SENSOR_CENTER);   // Center
  int s4 = digitalRead(SENSOR_RIGHT_1);  // Right
  int s5 = digitalRead(SENSOR_RIGHT_2);  // Far Right

  // Create 5-digit string
  String sensorString = String(s1) + String(s2) + String(s3) + String(s4) + String(s5);

  // Print sensor values for debugging
  Serial.print("Sensors: ");
  Serial.println(sensorString);

  return sensorString;
}

// Line tracking function
void lineTracking()
{
  String sensors = readSensors();

  // Track based on sensor pattern string
  // 0 = Black line detected, 1 = White ground detected

  if (sensors == "11011") {
    // Center sensor on black line: Go straight
    go_Advance(TRACK_SPEED, TRACK_SPEED);
  }
  else if (sensors == "10011") {
    // Left and center on black: Line is on left, turn LEFT
    go_Advance(SLOW_SPEED, TRACK_SPEED);
  }
  else if (sensors == "10111") {
    // Left sensor on black: Line is more to left, turn LEFT stronger
    go_Advance(SLOW_SPEED, TURN_SPEED);
  }
  else if (sensors == "00111" || sensors == "01111") {
    // Far left sensors on black: Line is far left, sharp LEFT turn
    go_Left(TURN_SPEED, TURN_SPEED);
  }
  else if (sensors == "11001") {
    // Center and right on black: Line is on right, turn RIGHT
    go_Advance(TRACK_SPEED, SLOW_SPEED);
  }
  else if (sensors == "11101") {
    // Right sensor on black: Line is more to right, turn RIGHT stronger
    go_Advance(TURN_SPEED, SLOW_SPEED);
  }
  else if (sensors == "11100" || sensors == "11110") {
    // Far right sensors on black: Line is far right, sharp RIGHT turn
    go_Right(TURN_SPEED, TURN_SPEED);
  }
  else if (sensors == "11111") {
    // All sensors on white: No line detected, continue forward slowly
    go_Advance(SLOW_SPEED, SLOW_SPEED);
  }
  else if (sensors == "00000") {
    // All sensors on black: Might be intersection or end, stop
    stop_Stop();
  }
  else {
    // Other patterns: Continue forward
    go_Advance(TRACK_SPEED, TRACK_SPEED);
  }
}

void setup()
{
  Serial.begin(115200);  // Initialize Serial for debugging

  // OPTIONAL: Disable brownout detector for weak 9V battery
  // WARNING: This is a workaround, not a real fix!
  // Uncomment the line below if you still experience resets
  // #include "soc/soc.h"
  // #include "soc/rtc_cntl_reg.h"
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

	init_GPIO();
  delay(500);  // Short delay to stabilize

 // Serial.println("ESP32 Line Tracking Car Started");
 // Serial.println("Sensor format: [S1][S2][S3][S4][S5]");
 // Serial.println("(Far Left)(Left)(Center)(Right)(Far Right)");
 // Serial.println("0 = Black line, 1 = White ground");

  // Optional: Test motor demo (comment out if not needed)
  /*
  go_Advance();//Forward
  delay(1000);
  go_Back();//Reverse
  delay(1000);
  go_Left();//Turn left
  delay(1000);
  go_Right();//Turn right
  delay(1000);
  stop_Stop();//Stop
  delay(1000);
  */
}

void loop()
{
  // Continuous line tracking
  lineTracking();
  delay(10);  // Small delay for stability
}
