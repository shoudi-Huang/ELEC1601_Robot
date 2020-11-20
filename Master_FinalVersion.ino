#include <SoftwareSerial.h>
SoftwareSerial blueToothSerial(10,11);

int JoyStick_X = A0; //x
int JoyStick_Y = A2; //y
int JoyStick_Z = 2; //z

void setup()
{
pinMode(JoyStick_Z, INPUT);
blueToothSerial.begin(9600);   
Serial.begin(9600);   
Serial.println("Master Board");
delay(5000);
}

// Memory for the movement of robot
String move_instruction[50];
int move_count_ls[50];
String detect_move_instruction[50];
int detect_move_count_ls[50];
String recent_move = "";

String suitable_data_ls[6] = {"F", "B", "L", "R", "D"};

int move_index = 0;
int move_count = 0;
int detect_move_index = 0;

boolean start_detection = false;

// changeble
int auto_L_turn_delay_times = 20;
int auto_R_turn_delay_times = 20;
int auto_reverse_delay_times = 40;


void loop()
{
    char local_recvChar;
    char BT_recvChar;
    String recvString = "";
    String instruction = "";

    while (true)
    {  
    while (blueToothSerial.available()>0)
    {
        boolean suitable_data = false;
        BT_recvChar = blueToothSerial.read();
        recvString = "";
        recvString += BT_recvChar;
        Serial.println(recvString);

        if (recvString.equals("D")){
                
                
                start_detection = false;
                detect_move_instruction[detect_move_index] = recent_move;
                detect_move_count_ls[detect_move_index] = move_count;
                detect_move_index++;
                recent_move = "";
                move_count = 0;
                
                Serial.println();
                Serial.println("AUTO DETECTION DONE, BALL FOUND!!!");
                delay(1000);
                start_detection = false;

                break;
            }
            
        for (int i = 0; i <= 4; i++){
          if (recvString == suitable_data_ls[i]){
            suitable_data = true;
            break;
          }
        }

        if (!suitable_data){
          break;
        }
        // if recvString is not the same as direction before, add it into the list, if same, add move
        
        if (recvString.equals("S")){
            break;
        }
        if (!start_detection){
            if (!recvString.equals(recent_move)){
                if (recent_move.equals("")){
                    recent_move = recvString;
                    if (!recent_move.equals("S")){
                        move_count = 1;
                    }
                    break;
                }
                else if (recvString.equals("S")){
                    break;
                }
                else {
                    move_instruction[move_index] = recent_move;
                    move_count_ls[move_index] = move_count;
                    move_index++;
                    move_count = 1;
                    recent_move = recvString;
                    break;
                }
            }
            else {
                move_count++;
                break;
            }
        }
        else {
            if (!recvString.equals(recent_move)){
                if (recent_move.equals("")){
                    recent_move = recvString;
                    move_count = 1;
                    break;
                }
                else if (recvString.equals("S")){
                    break;
                }
                else {
                    detect_move_instruction[detect_move_index] = recent_move;
                    detect_move_count_ls[detect_move_index] = move_count;
                    detect_move_index++;
                    move_count = 1;
                    recent_move = recvString;
                    break;
                }
            }
            else {
                move_count++;
                break;
            }
        }
    }

    if (!start_detection)
    {
      break;
    }
    delay(50);
    }

recvString = "";
while (Serial.available()>0)
{
local_recvChar = Serial.read();
recvString += local_recvChar;

if (recvString == "S")
{
Serial.println();
Serial.println("RESTART PROCESS ....");
blueToothSerial.print("R");

repeat_process(move_instruction, move_count_ls, move_index, "manully");
repeat_process(detect_move_instruction, detect_move_count_ls, detect_move_index, "auto");
Serial.println();
blueToothSerial.print("D");
Serial.println("RESTART PROCESS DONE!!!");
delay(3000);
break;
}

else if (recvString == "B")
{
Serial.println();
Serial.println("START RETURN PROCESS ...");
blueToothSerial.print("R");

return_starting_point(detect_move_instruction, detect_move_count_ls, detect_move_index, "auto");
Serial.println();
return_starting_point(move_instruction, move_count_ls, move_index, "manully");
Serial.println();

blueToothSerial.print("D");
Serial.println("RETURN PROCESS DONE ...");
Serial.println();
delay(3000);
break;
}

else if (recvString == "E")
{
Serial.println();
Serial.println("EMPTING MENMORY ...");
Serial.println();
 
memset(move_instruction, 0, sizeof(move_instruction));
memset(move_count_ls, 0, sizeof(move_count_ls));
move_index = 0;
memset(detect_move_instruction, 0, sizeof(detect_move_instruction));
memset(detect_move_count_ls, 0, sizeof(detect_move_count_ls));
detect_move_index = 0;
delay(3000);
Serial.println("MENMORY EMPTY PROCESS DONE!!!");
delay(3000);
}

else if (recvString == "P")
{
Serial.println();
Serial.println("START PICK UP ....");
blueToothSerial.print("P");
delay(9000);
Serial.println("PICK UP DONE!!!");
delay(1000);
break;
}

else if (recvString == "A")
{
move_instruction[move_index] = recent_move;
move_count_ls[move_index] = move_count;
move_index++;
move_count = 0;
recent_move = "";

Serial.println();
blueToothSerial.print("A");
Serial.println("START AUTO DETECTION ...");
start_detection = true;
break;
}

else
{
blueToothSerial.print(local_recvChar);
Serial.print(local_recvChar);
}

}

if (!start_detection)
{
int x,y,z;
x=analogRead(JoyStick_X);
y=analogRead(JoyStick_Y);
z=digitalRead(JoyStick_Z);
Serial.print("x:");
Serial.print(x ,DEC);
Serial.print(" ");
Serial.print("y:");
Serial.print(y ,DEC);
Serial.print(" ");
Serial.print("z:");
Serial.println(z ,DEC);
      
//Sent to Robot
sent_robot_instruction(x, y);
}
delay(150);

}

void return_starting_point(String input_move_instruction[], int input_move_count_ls[], int input_move_index, String key)
{
  String return_move_instruction[50];
  int return_move_count_ls[50];
  int i = 0;
  
    for (int x = (input_move_index - 1); x >= 0; x--)
    {
      if (input_move_instruction[x] == "B")
      {
        return_move_instruction[i] = "B";
      }
      else if (input_move_instruction[x] == "F")
      {
        return_move_instruction[i] = "F";
      }
      else if (input_move_instruction[x] == "R")
      {
        return_move_instruction[i] = "L";
      }
      else if (input_move_instruction[x] == "L")
      {
        return_move_instruction[i] = "R";
      }

      return_move_count_ls[i] = input_move_count_ls[x];
      i++;
    }
  
  if (key == "auto")
  {
    Serial.print("Reverse  ");
    for (int z = 0; z <= auto_reverse_delay_times; z++)
    {
      sent_robot_instruction(1023, 512); // Reverse 
      delay(150);
    }
  }
  repeat_process(return_move_instruction, return_move_count_ls, input_move_index, key);
}

void repeat_process(String input_move_instruction[], int input_move_count_ls[], int input_move_index, String key)
{
  int x, y;
  int move_count;
  for (int i = 0; i <= (input_move_index - 1); i++)
  {
    String direct;
    
    if (input_move_instruction[i] == "B")
    {
      x = 512;
      y = 1023 ;
      direct = "B";
    }
    else if (input_move_instruction[i] == "F")
    {
      x = 512;
      y = 0;
      direct = "F";
      
    }
    else if (input_move_instruction[i] == "R")
    {
      x = 1023;
      y = 512;
      direct = "R";

    }
    else if (input_move_instruction[i] == "L")
    {
      x = 0;
      y = 512;
      direct = "L";

    }
    move_count = input_move_count_ls[i];

    Serial.print(input_move_instruction[i]);
    Serial.print(": ");
    Serial.print(move_count);
    Serial.print("  ");
    
    for (int w = 1; w <= move_count; w++)
    {
      if (key == "manully")
      {
        sent_robot_instruction(x, y);
        delay(150);
      }
      else if (key == "auto")
      {
        if ( direct == "L")
        {
          for (int z = 0; z <= auto_L_turn_delay_times; z++)
          {
            sent_robot_instruction(x, y);
            delay(150);
          }
        }
        else if (direct == "R")
        {
          for (int z = 0; z <= auto_R_turn_delay_times; z++)
          {
            sent_robot_instruction(x, y);
            delay(150);
          }
        }
        else
        {
          sent_robot_instruction(x, y);
          delay(150);
        }
  
      }
    }
  }
  
}


void sent_robot_instruction(int x, int y)
{
  blueToothSerial.print(x, DEC);
  blueToothSerial.print("x");
  blueToothSerial.print(y, DEC);
  blueToothSerial.print("y");
}
