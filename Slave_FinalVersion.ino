#include <SoftwareSerial.h>
SoftwareSerial blueToothSerial(8,9);

#include <Servo.h>                      // Include servo library
Servo servoPickup;
Servo servoRight;
Servo servoLeft;

const int echoFrontPin = 5;
const int trigFrontPin = 4;
const int echoLeftPin = 6;
const int trigLeftPin = 7;
const int echoRightPin = 3;
const int trigRightPin = 2;

int distance;
long temp;

//changeble
int pickup_UpAngle = 130;
int pickup_DownAngle =0;

void setup()
{
blueToothSerial.begin(9600);   
Serial.begin(9600);   
Serial.println("Salve Board");
delay(5000);
servoPickup.attach(10);
servoPickup.write(pickup_UpAngle);
delay(250);
servoPickup.detach();

pinMode(trigRightPin, OUTPUT);
pinMode(echoRightPin, INPUT);
pinMode(trigLeftPin, OUTPUT);
pinMode(echoLeftPin, INPUT);
pinMode(trigFrontPin, OUTPUT);
pinMode(echoFrontPin, INPUT);

pinMode(A2, INPUT);
pinMode(A3, INPUT);
}

boolean repeat_process = false;
boolean start_detection = false;

// changeble
int Front_detect_range = 12;
int Left_detect_range = 18;
int Right_detect_range = 18;

int right_delay_time = 800;
int left_delay_time = 800;

void loop()
{
boolean found_x_y = false;
char recvChar;
String recvalue;
int x_axis_value = -1;
int y_axis_value = -1;
String direct;

while(!found_x_y && !start_detection)
    {
      if(blueToothSerial.available())   // Check if there's any data sent from the remote Bluetooth shield
      {
        recvChar = blueToothSerial.read();

        String single_recvString = "";
        single_recvString += recvChar;
        if (isDigit(recvChar))
        {
          recvalue += recvChar;
        }   
        else if (single_recvString == "x")
        {
          // Get value in x axis
          x_axis_value=recvalue.toInt(); 
              
          // clear recvalue for another operation
          recvalue = "";
        }
        else if (single_recvString == "y")
        {
          // Get value in y axis
          y_axis_value=recvalue.toInt(); 
              
          // clear recvalue for another operation
          recvalue = "";
        }
        else if (single_recvString == "A")
        {
          Serial.println("Start AUTO DETECTION");
          start_detection = true;
          y_axis_value = -1;
          x_axis_value = -1;
          delay(1500);
          break;
        }
        else if (single_recvString == "R")
        {
          repeat_process = true;
        }
        else if (single_recvString == "D")
        {
          repeat_process = false;
        }
        else if (single_recvString == "P")
        {
          Serial.println("Start PICK UP");
          pick_up();
          Serial.println("PICK UP Done");
        }
            
     }
     // Break out of loop if x and y value are collected
     if (x_axis_value != -1 && y_axis_value != -1)
     {
       // Print x, y axis value to serial monitor
       Serial.print("x:");
       Serial.print(x_axis_value);
       Serial.print(" ");
       Serial.print("y:");
       Serial.println(y_axis_value);
              
       found_x_y = true;
     }
    }

// Process for maze navigation
if (start_detection)
{
// Check is ball in front of robot
boolean ball_infront = ball_exist_detection("position");
if (ball_infront == true)
{
Serial.println("Ball Found");
direct = "S";
start_detection = false;
blueToothSerial.print("D");
delay(10);
}
else
{
// Check is ball exist but far from robot
boolean ball_exist = ball_exist_detection("exist");

if (ball_exist == true)
{
Serial.println("Ball Exist");
// Get close to the Ball
direct = "F";
Serial.println(direct);
}
else
{
// ramdonlly chose a direction from avaliable direction
direct = generate_direction();
}

}

}

// Move Backward
if (y_axis_value > 1000 || direct == "B"){
direct = "B";
servoRight.attach(12);
servoLeft.attach(13);
delay(20);
servoRight.writeMicroseconds(1700);
servoLeft.writeMicroseconds(1300);
delay(100);
servoRight.detach();
servoLeft.detach();
}
// Move Forward
else if (y_axis_value == 0 || direct == "F"){
direct = "F";
servoRight.attach(12);
servoLeft.attach(13);
delay(20);
servoRight.writeMicroseconds(1300);
servoLeft.writeMicroseconds(1700);
delay(100);
servoRight.detach();
servoLeft.detach();

}
// Turn Left
else if (x_axis_value == 0 || direct == "L"){
direct = "L";
servoRight.attach(12);
servoLeft.attach(13);
delay(20);
servoRight.writeMicroseconds(1300);
servoLeft.writeMicroseconds(1300);
if (start_detection)
{
// time 90 degrees turn
delay(left_delay_time);  
}
else
{
delay(100);
}

servoRight.detach();
servoLeft.detach();
}
// Turn Right
else if (x_axis_value > 1000 || direct == "R"){
direct = "R";
servoRight.attach(12);
servoLeft.attach(13);
delay(20);
servoRight.writeMicroseconds(1700);
servoLeft.writeMicroseconds(1700);
if (start_detection)
{
// Time for 90 degree turn
delay(right_delay_time);  
}
else
{
delay(100);
}

servoRight.detach();
servoLeft.detach();
}
// Reverse robot (180 degrees turn)
else if (direct == "reverse"){
servoRight.attach(12);
servoLeft.attach(13);
delay(20);
servoRight.writeMicroseconds(1700);
servoLeft.writeMicroseconds(1700);
delay(right_delay_time);
servoRight.detach();
servoLeft.detach();

// Second time turn right in 90 degrees
servoRight.attach(12);
servoLeft.attach(13);
delay(20);
servoRight.writeMicroseconds(1700);
servoLeft.writeMicroseconds(1700);
delay(right_delay_time);
servoRight.detach();
servoLeft.detach();

}
else
{
// Robot Stop
direct = "S";
}
// Send finished move back to master board for memory of movement
if (!repeat_process)
{
if (direct == "reverse"){
blueToothSerial.print("R");
delay(10);
blueToothSerial.print("R");
}
else
{
blueToothSerial.print(direct);
}

}

}

// Function for pick up the ball
void pick_up()
{
//  suitable angle for lay down collecter
  servoPickup.attach(10);
  servoPickup.write(pickup_DownAngle);
  delay(1000);
  servoPickup.detach();
  delay(3000);
  servoPickup.attach(10);
  servoPickup.write(pickup_UpAngle);
  delay(250);
  servoPickup.detach();
}

// Function for ball exist and position detection
boolean ball_exist_detection(String key)
{
  // Receive signal from Raspberry Pi
  int pin;
  if (key == "exist")
  {
    pin = A2;
  }
  else if (key == "position")
  {
    pin = A3;
  }

  int ball_detection = digitalRead(pin);
  boolean ball_status = false;
  // Confirm ball_status
  if (ball_detection == 1)
  {
    delay(250);
    ball_detection = digitalRead(pin);
    if (ball_detection == 1)
    {
      ball_status = true;
    }
  }

  return ball_status;
}

// Function for generating direction for maze navigation
String generate_direction()
{
String direct = "";
String avaliable_direct_ls[3];

// Check distance between wall and sensor
int front_distance = checkdist(echoFrontPin, trigFrontPin);
int right_distance = checkdist(echoRightPin, trigRightPin);
int left_distance = checkdist(echoLeftPin, trigLeftPin);

Serial.print("Right: ");
Serial.print(right_distance);
Serial.print("  Left: ");
Serial.print(left_distance);
Serial.print("  Front: ");
Serial.println(front_distance);


int i = 0;
if (front_distance > Front_detect_range)
{
avaliable_direct_ls[i] = "F";
i++;
}
if (right_distance > Right_detect_range)
{
avaliable_direct_ls[i] = "R";
i++;
}
if (left_distance > Left_detect_range)
{
avaliable_direct_ls[i] = "L";
i++;
}

// ramdonlly chose a direction
if (i>0)
{
int suitable_index_range = i;
direct = avaliable_direct_ls[random(0, suitable_index_range)];
}
else
{
direct = "reverse";
}

Serial.println(direct);
return direct;

}

// Function for detection distance between wall and sensor
int checkdist(int echoPin, int trigPin) {
  //低高低电平发送一个短时间脉冲
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  temp = pulseIn(echoPin, HIGH);
  // Formula for converting digital value to distance in cm
  distance = (temp * 17) / 1000;
  delay(10);
//  Serial.print("距离：");
//  Serial.println(distance);
//  Serial.println();
  return distance;
}
