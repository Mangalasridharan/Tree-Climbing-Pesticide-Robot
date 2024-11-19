#include <RH_ASK.h>
#include <SPI.h>
#include "SoftServo.h" 

RH_ASK rf_driver;

String lastCommand = "";  

int motor1_a = 4;
int motor1_b = 5;
int motor2_a = 2;
int motor2_b = 3;

SoftServo servoX; 
SoftServo servoY;

void setup() 
{
  Serial.begin(9600);       
  rf_driver.init();         
  

  pinMode(motor1_a, OUTPUT);       
  pinMode(motor1_b, OUTPUT);       
  pinMode(motor2_a, OUTPUT);       
  pinMode(motor2_b, OUTPUT);       

  digitalWrite(motor1_a, LOW);
  digitalWrite(motor1_b, LOW);
  digitalWrite(motor2_a, LOW);
  digitalWrite(motor2_b, LOW);

  
  servoX.attach(9);  
  servoY.attach(10); 

  
  servoX.delayMode();  
  servoY.delayMode();
}

void stopMotor() 
{ 
  digitalWrite(motor1_a, LOW);
  digitalWrite(motor1_b, LOW);
  digitalWrite(motor2_a, LOW);
  digitalWrite(motor2_b, LOW);
  Serial.println("Motor: Stopped");
}

void runMotorClockwise() 
{
  digitalWrite(motor1_a, HIGH);
  digitalWrite(motor1_b, LOW);
  digitalWrite(motor2_a, HIGH);
  digitalWrite(motor2_b, LOW);
  Serial.println("Motor: Clockwise (ASCEND)");
}

void runMotorCounterclockwise() 
{
  digitalWrite(motor1_a, LOW);
  digitalWrite(motor1_b, HIGH);
  digitalWrite(motor2_a, LOW);
  digitalWrite(motor2_b, HIGH);
  Serial.println("Motor: Counterclockwise (DESCEND)");
}


void moveServos(int xVal, int yVal) 
{
  int servoXPos = map(xVal, 0, 1023, 0, 180); // Map x value to 0-180 degrees
  int servoYPos = map(yVal, 0, 1023, 0, 180); // Map y value to 0-180 degrees
  
  servoX.write(servoXPos);  // Move servo X
  servoY.write(servoYPos);  // Move servo Y

  // Call tick() frequently to update servo positions
  servoX.tick();
  servoY.tick();
}

void loop() 
{
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];  
  uint8_t buflen = sizeof(buf);

  if (rf_driver.recv(buf, &buflen)) 
  {
    buf[buflen] = '\0'; // Null-terminate the received message

    String command = String((char*)buf); // Store received message as a string

    Serial.print("Received: ");
    Serial.println(command);

    // Parse command and x, y values
    int commaIndex1 = command.indexOf(',');  // First comma
    int commaIndex2 = command.lastIndexOf(',');  // Last comma

    int xVal = 0, yVal = 0; // Initialize xVal and yVal

    if (commaIndex1 > 0 && commaIndex2 > commaIndex1) 
    {
      // Extract the command part before the first comma
      String cmd = command.substring(0, commaIndex1);
      
      // Extract the x and y values from the message
      xVal = command.substring(commaIndex1 + 1, commaIndex2).toInt();  // x value
      yVal = command.substring(commaIndex2 + 1).toInt();  // y value
      
      Serial.print("X value: ");
      Serial.println(xVal);
      Serial.print("Y value: ");
      Serial.println(yVal);

      // Move the servos based on x and y values
      moveServos(xVal, yVal);
    }

    // Only process the command if it's different from the last one
    if (command != lastCommand) 
    {
      lastCommand = command;  // Update the last command received

      // Stop the motor before changing direction
      stopMotor();
      delay(300);  // Increased delay to allow motor to fully stop

      // Check if the message is "ASCEND" or "DESCEND"
      if (command.startsWith("ASCEND")) 
      {
        runMotorClockwise();  // Run motor clockwise
      } 
      else if (command.startsWith("DESCEND")) 
      {
        stopMotor();
        runMotorCounterclockwise();  // Run motor counterclockwise
      } 
      else 
      {
        // If the message is not recognized, keep the motor stopped
        stopMotor();
      }
    }
  }

  // Continuously update servo positions
  servoX.tick();
  servoY.tick();
}
