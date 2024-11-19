#include <RH_ASK.h>
#include <SPI.h>

RH_ASK rf_driver;

int lastButtonState1 = LOW;  // To store the last state of button 1
int lastButtonState2 = LOW;  // To store the last state of button 2

unsigned long lastDebounceTime1 = 0;  // The last time button 1 state was changed
unsigned long lastDebounceTime2 = 0;  // The last time button 2 state was changed
unsigned long debounceDelay = 50;     // Debounce time in milliseconds

int x;
int y;

void setup() 
{
  Serial.begin(9600);
  rf_driver.init();
  pinMode(4, INPUT);  
  pinMode(3, INPUT);  

  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
}

void sendMessage(String command, int xVal, int yVal) {
  // Create a formatted string containing the command and x, y values
  String message = command + "," + String(xVal) + "," + String(yVal);
  
  // Convert the message to a character array
  const char *msg = message.c_str();
  
  // Send the message via RF
  rf_driver.send((uint8_t *)msg, strlen(msg));
  rf_driver.waitPacketSent();
  
  Serial.print("Sent message: ");
  Serial.println(message);
}

void loop() {
  
  int buttonState1 = digitalRead(4);
  int buttonState2 = digitalRead(3);

  // Debounce the button states
  if (buttonState1 != lastButtonState1) {
    lastDebounceTime1 = millis();  
  }
  if (buttonState2 != lastButtonState2) {
    lastDebounceTime2 = millis();  // Record the time of the change
  }

  // Check if enough time has passed since the last button change
  if ((millis() - lastDebounceTime1) > debounceDelay && (millis() - lastDebounceTime2) > debounceDelay) {
    
    // Read the analog values from A0 and A1
    x = analogRead(A0);
    y = analogRead(A1);

    if (buttonState1 == HIGH && buttonState2 == LOW) {
      sendMessage("ASCEND",512,512);  // Button 1 is pressed, button 2 is not
    } 
    else if (buttonState1 == LOW && buttonState2 == HIGH) {
      sendMessage("DESCEND",512,512);  // Button 2 is pressed, button 1 is not
    } 
    else if ((buttonState1 == HIGH && buttonState2 == HIGH) || (buttonState1 == LOW && buttonState2 == LOW)) {
      sendMessage("IDLE", x, y);  // Both buttons are either pressed or not pressed
    }
  }

  // Update the button states
  lastButtonState1 = buttonState1;
  lastButtonState2 = buttonState2;

  // Small delay to prevent flooding the transmitter
  delay(100);
}
