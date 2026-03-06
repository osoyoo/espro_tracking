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

// IR Tracking sensors Pins (Left to Right)
#define sensors_LEFT_2   33    // Far Left
#define sensors_LEFT_1   35    // Left
#define sensors_CENTER   34    // Center
#define sensors_RIGHT_1  39    // Right
#define sensors_RIGHT_2  36    // Far Right

// Speed settings for tracking
// Reduced speeds for low-capacity 9V battery to prevent brownout
#define TRACK_SPEED     180    // Normal tracking speed (reduced from 80)
#define TURN_SPEED     120    // Turn speed (reduced from 100)
#define SLOW_SPEED     90    // Slow speed for gentle corrections (reduced from 60)

void  Left(int L_SPEED=SPEED)  //left motor
{
  if (L_SPEED<0){
    digitalWrite(LeftMotorDirPin1,LOW);
    digitalWrite(LeftMotorDirPin2,HIGH);
    analogWrite(speedPinL,-L_SPEED);
  } else {
    digitalWrite(LeftMotorDirPin1,HIGH);
    digitalWrite(LeftMotorDirPin2,LOW);   
    analogWrite(speedPinL,L_SPEED);
  }
  
}
void Right(int R_SPEED=SPEED)  //right motor
{
    if (R_SPEED<0){
      digitalWrite(RightMotorDirPin1, LOW);
      digitalWrite(RightMotorDirPin2,HIGH);
       analogWrite(speedPinR,-R_SPEED);
    }
    else {
      digitalWrite(RightMotorDirPin1, HIGH);
      digitalWrite(RightMotorDirPin2, LOW);     
       analogWrite(speedPinR,R_SPEED);
    }
 
}
void go_Left(int L_SPEED=SPEED,int R_SPEED=SPEED) {
  Right(L_SPEED);
  Left(-R_SPEED);
}
void go_Right(int L_SPEED=SPEED,int R_SPEED=SPEED)  //Turn right
{
  Right(-L_SPEED);
  Left(R_SPEED);
}
void go_Advance(int R_SPEED=SPEED,int L_SPEED=SPEED)  //Reverse
{
  Right(-R_SPEED);
  Left(-L_SPEED);
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

  // Initialize IR sensors pins as INPUT
  pinMode(sensors_LEFT_2, INPUT);
  pinMode(sensors_LEFT_1, INPUT);
  pinMode(sensors_CENTER, INPUT);
  pinMode(sensors_RIGHT_1, INPUT);
  pinMode(sensors_RIGHT_2, INPUT);

	stop_Stop();
}

// Read all 5 IR sensors and return as a string
float readsensors()
{
  int s1 = !digitalRead(sensors_LEFT_2);   // Far Left
  int s2 = !digitalRead(sensors_LEFT_1);   // Left
  int s3 = !digitalRead(sensors_CENTER);   // Center
  int s4 = !digitalRead(sensors_RIGHT_1);  // Right
  int s5 = !digitalRead(sensors_RIGHT_2);  // Far Right
  int count=s1+s2+s3+s4+s5;
  if (count==0) return 999;
  // Create 5-digit string
  float sensorsvalue = (s1*10.0+s2*5.0-s4*5.0-s5*10.0)/count;
  return sensorsvalue;
}

// Line tracking function
void lineTracking()
{
  float sensors = readsensors();
  Serial.print("sensors: ");
  Serial.println(sensors);
  // Track based on sensors pattern string
  // 0 = Black line detected, 1 = White ground detected
 if (sensors >900) {
    // All sensors on white: No line detected, continue forward slowly
    go_Advance(SLOW_SPEED, SLOW_SPEED);
  }
  else if (abs(sensors)<2  ) {
    // Center sensors on black line: Go straight
    go_Advance(TRACK_SPEED, TRACK_SPEED);
  }
  else if (sensors>=2 && sensors <5 ) {
    // Left and center on black: Line is on left, turn LEFT
    go_Advance(SLOW_SPEED, TRACK_SPEED);
  }
  else if (sensors >=5 && sensors<7.5) {
    // Left sensors on black: Line is more to left, turn LEFT stronger
    go_Advance(SLOW_SPEED, TURN_SPEED);
  }
  else if (sensors >=7.5) {
    // Far left sensors on black: Line is far left, sharp LEFT turn
    go_Left(TURN_SPEED, TURN_SPEED);
  }
  else if (sensors<= -2 && sensors>-5) {
    // Center and right on black: Line is on right, turn RIGHT
    go_Advance(TRACK_SPEED, SLOW_SPEED);
  }
  else if (sensors <=-5 && sensors>-7.5) {
    // Right sensors on black: Line is more to right, turn RIGHT stronger
    go_Advance(TURN_SPEED, SLOW_SPEED);
  }
  else if (sensors<=-7.5) {
    // Far right sensors on black: Line is far right, sharp RIGHT turn
    go_Right(TURN_SPEED, TURN_SPEED);
  }

}

void setup()
{
  Serial.begin(115200);  // Initialize Serial for debugging
	init_GPIO();
  //  go_Right(TURN_SPEED, TURN_SPEED);
  //  delay(2000);
    stop_Stop();
}

void loop()
{
  // Continuous line tracking
lineTracking();
  delay(10);  // Small delay for stability
}
