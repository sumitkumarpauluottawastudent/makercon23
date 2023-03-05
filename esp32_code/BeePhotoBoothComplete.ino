#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "esp32-hal-ledc.h"


#define COUNT_LOW 0
#define COUNT_HIGH 3500
#define TIMER_WIDTH 16

const char *SSID = "sumitlaptop";
const char *PWD = "4321drowssaP";
 
// Web server running on port 80
WebServer server(80);
 
const int potPin = 34;
int detection_num = 0;
bool is_moving = true;
int STABLIZE_TIME = 1000;/* Time to stabilize in terms of ms */

// void detectMotion()
// {
//   int potValue = 0;
//   // Reading potentiometer value
//   potValue = analogRead(potPin);

//   //if previously it was moving and now it is not
//   //if((potValue == 0))
//   if((prev_state_moving == true) && (potValue == 0))
//   {
//     detection_num++;
//     Serial.printf("You can take the picture now, static-position detection event: %d\n", detection_num);
//     prev_state_moving = false;
//   }
//   else if (potValue != 0)
//   {
//     prev_state_moving = true;
//     //After the motion detection it stays high for 3s
//     //delay(2000);
//   }

//   delay(50);
// }

void detectStability()
{
  int potValue;
  is_moving = true;

  while (is_moving == true)
  {
    // Reading potentiometer value
    potValue = analogRead(potPin);

    if(potValue == 0)
    {
      /* For few second it does not send output */
      delay(STABLIZE_TIME);

      /* Again check the value */
      potValue = analogRead(potPin);

      /* If it still in static position */
      if(potValue == 0)
      {
        is_moving = false;
      }
    }
    else
    {
      delay(500);
    }
  }
}

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(SSID);
  
  WiFi.begin(SSID, PWD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    // we can even make the ESP32 to sleep
  }
 
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}
void setup_routing() {	 	 
  server.on("/stability", getStability);	 	 

  // start server	 	 
  server.begin();	 	 
}
 

void getStability() {
  Serial.println("Received the App's request");
  contract_chamber();
  Serial.println("Chamber contracted");
  Serial.println("Waiting for the Bee to be stabilized..");
  detectStability();
  Serial.println("Stability detected");

  if (is_moving == true)
  {
    server.send(200, "application/json", "Moving");
  }
  else
  {
    server.send(200, "application/json", "Static");
  }
}
void contract_chamber()
{
  for (int i=COUNT_LOW ; i < COUNT_HIGH ; i=i+100)
  {
      ledcWrite(1, i);       // sweep servo 1
      delay(50);
  }
}

void expand_chamber()
{
  for (int i=COUNT_LOW ; i < COUNT_HIGH ; i=i+100)
  {
      ledcWrite(1, (COUNT_HIGH - i));       // sweep servo 1
      delay(100);
  }
}

void setup_motor()
{
  ledcSetup(1, 30, TIMER_WIDTH); // channel 1, 50 Hz, 16-bit width
  ledcAttachPin(22, 1);   // GPIO 22 assigned to channel 1
}

void setup() {	 	 
  Serial.begin(115200);

  setup_motor();

  connectToWiFi();	 	 
  //setup_task();	 	 
  setup_routing(); 	 	 
 }	 	 
  	 	 
void loop() {	 	 
  server.handleClient();
  //delay(5000);
  /* Change the motor to be in the initial position */
  //expand_chamber();
}