//Libraries 
#include <Wire.h>

#include <Digital_Light_TSL2561.h>
#include "SmartInterface.h"
#include "wifi_mqtt.h"

// Use SAMD51's DMAC to read ADC1 on the Microphone input and alternately store results in two memory arrays
// Use DMAC channel0
#define NO_RESULTS 256
const int WINDOW_SIZE = 512;
#define LED_PIN D0                                                  // for debug
#define VolumeGainFactorBits 0
#define AmpMax 1600
volatile boolean results0Ready = false;
volatile boolean results1Ready = false;
uint16_t adcResults0[NO_RESULTS];                                  // ADC results array 0
uint16_t adcResults1[NO_RESULTS];                                  // ADC results array 1
uint32_t maxdB;
uint32_t dB;
typedef struct           // DMAC descriptor structure
{
  uint16_t btctrl;
  uint16_t btcnt;
  uint32_t srcaddr;
  uint32_t dstaddr;
  uint32_t descaddr;
} dmacdescriptor ;

volatile dmacdescriptor wrb[DMAC_CH_NUM] __attribute__ ((aligned (16)));          // Write-back DMAC descriptors
dmacdescriptor descriptor_section[DMAC_CH_NUM] __attribute__ ((aligned (16)));    // DMAC channel descriptors
dmacdescriptor descriptor __attribute__ ((aligned (16)));                         // Place holder descriptor


//WiFi definitions

WiFiClient wioClient;
PubSubClient client(wioClient);

void wifi_setup()
{
  WiFi.begin(ssid, password); // Connecting WiFi

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}


void reconnect() 
{
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "WioTerminal-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 1 seconds before retrying
      delay(200);
    }
  }
}


//Initializations

TFT_eSPI tft; //Initializing TFT LCD
TFT_eSprite spr = TFT_eSprite(&tft); //Initializing buffer


float ltx = 0;    // Saved x coord of bottom of needle
uint16_t osx = 160, osy = 160; // Saved x & y coords
uint32_t updateTime = 0;       // time for next update

int old_analog =  -999; // Value last displayed
int old_digital = -999; // Value last displayed

int average_index = 0;
int readings[WINDOW_SIZE] = {0};

bool first =false;

float t=20;
float concentration=0;
float h=0;
int light = 0;
bool pressed = false;

ProgramState current_state = show_data;
ClassAssistant classAssistant;
bool class_started = false;
unsigned long started_notification = 0;
bool pause_resume_notification = false;

NotificationAlert pauseRecomendation(5, 5*60, "A break from class is recommended");
NotificationAlert blockRecomendation(5, 10*60, "a block breakthrough of the class is recommended");

NotificationAlert lowTemperaturetAlert(5, 15*60, "Low temperature detected");
float min_t = 15;
NotificationAlert highTemperaturetAlert(5, 15*60, "High temperature detected");
float max_t = 35;
NotificationAlert lightAlert(5, 10*60, "Insuficient light detected");
int min_light = 300;
NotificationAlert humiditytAlert(5, 10*60, "Insuficient humidity detected");
float min_h = 300;
NotificationAlert airtAlert(5, 10*60, "Bad air quality detected");
float min_concentration = 0;

#define N_NOTIFICATIONS 7
NotificationAlert* notifications[N_NOTIFICATIONS] = {
  &pauseRecomendation,
  &blockRecomendation,
  &lowTemperaturetAlert,
  &highTemperaturetAlert,
  &lightAlert,
  &humiditytAlert,
  &airtAlert
};

Tracker tracker;

void setup()
{
  SERIAL.begin(9600); //Start SERIAL communication
  pinMode(WIO_MIC, INPUT);
  tft.begin(); //Start TFT LCD
  tft.setRotation(3); //Set TFT LCD rotation
  spr.createSprite(TFT_HEIGHT,TFT_WIDTH); //Create buffer
  analogReference(AR_DEFAULT);
  
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  pinMode(WIO_BUZZER, OUTPUT);
  Wire.begin();
  TSL2561.init();
  scd30.initialize();
    // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_PIN, OUTPUT);

  DMAC->BASEADDR.reg = (uint32_t)descriptor_section;                          // Specify the location of the descriptors
  DMAC->WRBADDR.reg = (uint32_t)wrb;                                          // Specify the location of the write back descriptors
  DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);                // Enable the DMAC peripheral

  DMAC->Channel[1].CHCTRLA.reg = DMAC_CHCTRLA_TRIGSRC(ADC1_DMAC_ID_RESRDY) |  // Set DMAC to trigger when ADC1 result is ready
                                 DMAC_CHCTRLA_TRIGACT_BURST;                  // DMAC burst transfer
  descriptor.descaddr = (uint32_t)&descriptor_section[1];                     // Set up a circular descriptor
  descriptor.srcaddr = (uint32_t)&ADC1->RESULT.reg;                           // Take the result from the ADC1 RESULT register
  descriptor.dstaddr = (uint32_t)adcResults0 + sizeof(uint16_t) * NO_RESULTS; // Place it in the adcResults0 array
  descriptor.btcnt = NO_RESULTS;                                              // Beat count
  descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD |                            // Beat size is HWORD (16-bits)
                      DMAC_BTCTRL_DSTINC |                                    // Increment the destination address
                      DMAC_BTCTRL_VALID |                                     // Descriptor is valid
                      DMAC_BTCTRL_BLOCKACT_SUSPEND;                           // Suspend DMAC channel 0 after block transfer
  memcpy(&descriptor_section[0], &descriptor, sizeof(descriptor));            // Copy the descriptor to the descriptor section
  descriptor.descaddr = (uint32_t)&descriptor_section[0];                     // Set up a circular descriptor
  descriptor.srcaddr = (uint32_t)&ADC1->RESULT.reg;                           // Take the result from the ADC1 RESULT register
  descriptor.dstaddr = (uint32_t)adcResults1 + sizeof(uint16_t) * NO_RESULTS; // Place it in the adcResults1 array
  descriptor.btcnt = NO_RESULTS;                                              // Beat count
  descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD |                            // Beat size is HWORD (16-bits)
                      DMAC_BTCTRL_DSTINC |                                    // Increment the destination address
                      DMAC_BTCTRL_VALID |                                     // Descriptor is valid
                      DMAC_BTCTRL_BLOCKACT_SUSPEND;                           // Suspend DMAC channel 0 after block transfer
  memcpy(&descriptor_section[1], &descriptor, sizeof(descriptor));            // Copy the descriptor to the descriptor section

  NVIC_SetPriority(DMAC_1_IRQn, 0);    // Set the Nested Vector Interrupt Controller (NVIC) priority for TCC1 OVF to 0 (highest)
  NVIC_EnableIRQ(DMAC_1_IRQn);         // Connect TCC1 to Nested Vector Interrupt Controller (NVIC)

  DMAC->Channel[1].CHINTENSET.reg = DMAC_CHINTENSET_SUSP;                     // Activate the suspend (SUSP) interrupt on DMAC channel 0

  ADC1->INPUTCTRL.bit.MUXPOS = ADC_INPUTCTRL_MUXPOS_AIN12_Val;                // Set the analog input to AIN12
  while (ADC1->SYNCBUSY.bit.INPUTCTRL);                                       // Wait for synchronization
  ADC1->SAMPCTRL.bit.SAMPLEN = 0x0a;                                          // Set max Sampling Time Length to half divided ADC clock pulse (2.66us) if set to 0x0
  while (ADC1->SYNCBUSY.bit.SAMPCTRL);                                        // Wait for synchronization
  ADC1->CTRLA.reg = ADC_CTRLA_PRESCALER_DIV256;                               // Divide Clock ADC GCLK by 256 (48MHz/256 = 187.5kHz)
  ADC1->CTRLB.reg = ADC_CTRLB_RESSEL_12BIT |                                  // Set ADC resolution to 12 bits
                    ADC_CTRLB_FREERUN;                                        // Set ADC to free run mode
  while (ADC1->SYNCBUSY.bit.CTRLB);                                           // Wait for synchronization
  ADC1->CTRLA.bit.ENABLE = 1;                                                 // Enable the ADC
  while (ADC1->SYNCBUSY.bit.ENABLE);                                          // Wait for synchronization
  ADC1->SWTRIG.bit.START = 1;                                                 // Initiate a software trigger to start an ADC conversion
  while (ADC1->SYNCBUSY.bit.SWTRIG);                                          // Wait for synchronization
  DMAC->Channel[1].CHCTRLA.bit.ENABLE = 1;                                    // Enable DMAC ADC on channel 1


  // Connect to Wifi
  wifi_setup();
  client.setServer(mqtt_server, 1883);
}

void loop() {

  // Read right button
  if (digitalRead(WIO_KEY_A) == LOW) {
    delay(500);
    SERIAL.println("button A pressed");
    SERIAL.println(pressed);
    if (current_state == noise)
      current_state = show_data;
    else
      current_state = noise;
    SERIAL.print("Current state noise ");
    SERIAL.println(current_state);
    if (pressed == true)
    {
      first=!first;
    }

    pressed = !pressed;
    SERIAL.println("first");
    SERIAL.println(first);
    
  }

  // Read middle button
  else if (digitalRead(WIO_KEY_B) == LOW)
  {
    delay(500);
    SERIAL.println(classAssistant.get_class_is_running());
    SERIAL.println("Button B pressed");
    // Show start - puase - stop menu
    if (!classAssistant.get_class_has_started())
    {
      current_state = start_class;
      SERIAL.print("Current state start class ");
      SERIAL.println(current_state);
    }
    else
    {
      if (digitalRead(WIO_KEY_B) == LOW)
      {
        delay(200);
        SERIAL.println("class ended");
        int rate = end_class_menu();
        SERIAL.println("Finish end class menu");
        if (rate > -1)
        {
          classAssistant.end_class();
          classAssistant.rate_class(rate);
          if (!client.connected())
            reconnect();
          classAssistant.pub_summary(client);
          SERIAL.println("Class rated");
        }
      }
      else
      {
        classAssistant.change_class_state();
        SERIAL.print("Changed class state to ");
        SERIAL.println(classAssistant.get_class_is_running());
        started_notification = millis();
        // Start notification break - resume done
        SERIAL.println("Start showing resume notification");
        pause_resume_notification = true;
      }
    }
  }

  else if (classAssistant.get_class_is_running() && digitalRead(WIO_KEY_C) == LOW)
  {
    delay(200);
    SERIAL.println("Button C is pressed");
    current_state = change_block;
    SERIAL.print("Current state change block ");
    SERIAL.println(current_state);
  }

  if (pause_resume_notification && millis() - started_notification > 5 * 1000)
  {
    pause_resume_notification = false;
    SERIAL.println("stop showing resume notification");
  }


  //Check normal class conditions
  // Check break recomendation
  // if (classAssistant.check_current_time_in_a_row())
  // {
  //   SERIAL.println("Break recommended");
  //   pauseRecomendation.activate();
  // }
  // Check block duration
  if (classAssistant.check_current_block_time() && notifications[1]->get_activation())
  {
    SERIAL.println("Change block recommended");
    notifications[1]->activate();
  }
  // Check temperature level
  if (t < min_t && !notifications[2]->get_activation())
  {
    SERIAL.println("Low temperature");
    notifications[2]->activate();
  }
  else if (t > max_t && !notifications[3]->get_activation())
  {
    SERIAL.println("High temperature");
    notifications[3]->activate();
  }
  // Check light level
  if (light < min_light && !notifications[4]->get_activation())
  {
    SERIAL.println("Low light");
    notifications[4]->activate();
  }
  // Check humidity level
  if (h > min_h && !notifications[5]->get_activation())
  {
    SERIAL.println("High humidity level");
    notifications[5]->activate();
  }
  // Check air quality level
  if (concentration > min_concentration && !notifications[6]->get_activation())
  {
    SERIAL.println("Bad air quality");
    notifications[6]->activate();
  }


  switch (current_state)
  {
  // Read sensor measures and show in screen
  case show_data:
    SERIAL.println("SHOW DATA");
    show_measured_data();
    break;
  case noise:
  SERIAL.println("NOISE");
    if (first == false){
      SERIAL.println("first noise");
      analogMeter(); // Draw analogue meter
      SERIAL.println("------solo entra una vez");
      first=true;
    }
    SERIAL.println("Plot needle");
    plotNeedle(); // Put meter needle at 0
    updateTime = millis(); // Next update time
    break;
  case start_class:
    start_class_menu();
    current_state = show_data;
    classAssistant.start_class();
    class_started = true;
    break;
  case change_block:
    if (change_block_menu(String(classAssistant.get_current_block())))
    {
      classAssistant.change_class_block();
    }
    current_state = show_data;
  default:
    break;
  }

}


void show_measured_data()
{
  float result[3] = {0};

  if(scd30.isAvailable())
  {
    scd30.getCarbonDioxideConcentration(result);
    // SERIAL.print("Carbon Dioxide Concentration is: ");
    concentration=result[0];
    // SERIAL.print(result[0]);
    // SERIAL.println(" ppm");
    // SERIAL.println(" ");
    // SERIAL.print("Temperature = ");
    t = result[1];
    // SERIAL.print(result[1]);
    // SERIAL.println(" ℃");
    // SERIAL.println(" ");
    // SERIAL.print("Humidity = ");
    h = result[2];
    // SERIAL.print(result[2]);
    // SERIAL.println(" %");
    // SERIAL.println(" ");
    // SERIAL.println(" ");
    // SERIAL.println(" ");
  }

  

  //int t = dht.readTemperature(); //Assign variable to store temperature 
  //int h = dht.readHumidity(); //Assign variable to store humidity 
  //int light = analogRead(LighsensorPin); //Assign variable to store light sensor values
  light = TSL2561.readVisibleLux(); //Assign variable to store light sensor values
  //SERIAL.println("--------------------------------");
  //SERIAL.println(TSL2561.readVisibleLux());

  //light = light * (3300/2048.0);
  //SERIAL.println(light);
  //Setting the title header 
  spr.fillSprite(TFT_BLACK); //Fill background with white color
  spr.fillRect(0,0,320,50,TFT_DARKCYAN); //Rectangle fill with dark green 
  spr.setTextColor(TFT_WHITE); //Setting text color
  spr.setTextSize(3); //Setting text size 
  spr.drawString("Smart Classroom",37,15); //Drawing a text String 

  if (pause_resume_notification)
  {
    SERIAL.println("Show pause resume notification");
    spr.drawRoundRect(tft.width() / 2 - 25 , tft.height() / 2 - 25, 50, 50, TFT_DARKGREY, TFT_WHITE);
    spr.setTextColor(TFT_WHITE);
    spr.setTextSize(2);
    if (classAssistant.get_class_is_running())
    {
      spr.drawString("Class is now in session", 10, tft.height() / 2); //Drawing a text String 
      SERIAL.println("Class is now in session");
    }
    else
    {
      spr.drawString("Class is now on break", 10, tft.height() / 2); //Drawing a text String 
      SERIAL.println("Class is now on break");
    }
    spr.pushSprite(0,0); //Push to LCD
    delay(50);
    return;
  }

  // Check if any notification should be displayed
  for (int i = 0; i < N_NOTIFICATIONS; i++)
  {
    notifications[i]->check_notification_time();
    if (notifications[i]->get_activation())
    {
      SERIAL.print("Notification ");
      SERIAL.print(i);
      SERIAL.println(" is showing");
      SERIAL.println(notifications[i]->get_text());
      spr.drawRoundRect(tft.width() / 2 - 25 , tft.height() / 2 - 25, 50, 50, TFT_DARKGREY, TFT_WHITE);
      spr.setTextColor(TFT_WHITE);
      spr.setTextSize(2);
      spr.drawString(notifications[i]->get_text(), 10, tft.height() / 2);
      analogWrite(WIO_BUZZER, 128);
      spr.pushSprite(0,0); //Push to LCD
      delay(50);
      return;
    }
    analogWrite(WIO_BUZZER, 0);
  }






  //spr.drawFastVLine(165,50,190,TFT_BLUE); //Drawing verticle line
  //spr.drawRoundRect(5, 55, (tft.width() / 2 - 5), (tft.height()/2 -35) , 10, TFT_WHITE); // L1
  //spr.drawRoundRect((tft.width() / 2 - 5), 55, (tft.width()), (tft.height()/2 -25) , 10, TFT_WHITE); // L1
  //spr.drawFastHLine(0,140,320,TFT_BLUE); //Drawing horizontal line
  // Temp rect


  spr.drawRoundRect(5 , 60, (tft.width() / 2)-7 , (tft.height() - 65) / 2 , 10, TFT_WHITE); // s1  //Temp
  spr.drawRoundRect((tft.width() / 2) +3 , 60, (tft.width() / 2)-10  , (tft.height()-65) / 2 , 10, TFT_WHITE); // s2  Air

  spr.drawRoundRect(5, (tft.height() / 2) + 30, (tft.width() / 2)-7, (tft.height() - 65) / 2 , 10, TFT_WHITE); // s3  Hum
  spr.drawRoundRect((tft.width() / 2) +3 , (tft.height() / 2) + 30, (tft.width() / 2)-10   , (tft.height() - 65) / 2 , 10, TFT_WHITE); // s4



  //Setting temperature
  spr.setTextColor(TFT_WHITE);
  spr.setTextSize(2);
  spr.drawString("Temperature",15,70);
  if((t>20)&&(t<25)){
    spr.setTextColor(TFT_GREEN);
  } else{
    spr.setTextColor(TFT_RED);
  }
  spr.setTextSize(3);
  spr.drawNumber(t,55,105); //Display temperature values 
  spr.drawString("C",95,105);

  //Setting humidity
  spr.setTextColor(TFT_WHITE);
  spr.setTextSize(2);
  spr.drawString("Humidity",35,165);
  if((h>30)&&(h<60)){
    spr.setTextColor(TFT_GREEN);
  } else{
    spr.setTextColor(TFT_RED);
  }

  spr.setTextSize(3);
  spr.drawNumber(h,40,200); //Display humidity values 
  spr.drawString("%RH",80,200);

  //Setting air quality
  //sensorValue = analogRead(sensorPin); //Store sensor values 

  //light = map(light,0,1023,0,100);
  //SERIAL.println(light);
  spr.setTextColor(TFT_WHITE);
  spr.setTextSize(2);
  spr.drawString("Air Quality",175,70);
  //light = map(light,0,1024,0,100); //Map sensor values 
  if(concentration<700){
    spr.setTextColor(TFT_GREEN);
  } else{
    spr.setTextColor(TFT_RED);
  }
    spr.setTextSize(3);
    spr.drawNumber(concentration,175,105); //Display sensor values as percentage  
    spr.drawString("ppm",250,105);

    spr.setTextColor(TFT_WHITE);
    spr.setTextSize(2);
    spr.drawString("Light",210,165);
    //Setting light 
  if(light<300){
    spr.setTextColor(TFT_RED);
    //spr.setTextSize(3);
    //light = map(light,0,512,0,100); //Map sensor values 
    //spr.drawString("Bad",215,200);
    //spr.drawNumber(light,205,190); //Display sensor values as percentage  
    //spr.drawString("%",245,190);
    //SERIAL.println(light);
  } else{  spr.setTextColor(TFT_GREEN);
    //spr.setTextSize(3);
    //light = map(light,0,512,0,100); //Map sensor values 
    //spr.drawNumber(light,205,190); //Display sensor values as percentage  
    //spr.drawString("%",245,190);
    //spr.drawString("Good",205,200);
    //SERIAL.println(light);
  }
  spr.setTextSize(3);
  spr.drawNumber(light,180,200); 
  spr.drawString("lux",245,200);


  /*
  //Condition for low soil moisture
  if(sensorValue < 50){
    spr.fillSprite(TFT_RED);
    spr.drawString("Time to water!",35,100);
    analogWrite(WIO_BUZZER, 150); //beep the buzzer
    delay(1000);
    analogWrite(WIO_BUZZER, 0); //Silence the buzzer
    delay(1000);
  }
  */

  //Gather tracker data
  tracker.gather_data(h, light, concentration, t);
  if (!client.connected())
    reconnect();
  tracker.publish_data(client, h, light, concentration, t);
  spr.pushSprite(0,0); //Push to LCD
  delay(50);
}


// #########################################################################
//  Draw the analogue meter on the screen
// #########################################################################
void analogMeter() {
  // Meter outline

  spr.fillRect(5, 3, 320, 158, TFT_WHITE);
  spr.drawRect(5, 3, 320, 158, TFT_BLACK);

  spr.setTextColor(TFT_BLACK);  // Text colour

  // Draw ticks every 5 degrees from -50 to +50 degrees (100 deg. FSD swing)
  for (int i = -50; i < 51; i += 5) {
      // Long scale tick length
      int tl = 20;

      // Coodinates of tick to draw
      float sx = cos((i - 90) * 0.0174532925);
      float sy = sin((i - 90) * 0.0174532925);
      uint16_t x0 = sx * (133 + tl) + 160;
      uint16_t y0 = sy * (133 + tl) + 186;
      uint16_t x1 = sx * 133 + 160;
      uint16_t y1 = sy * 133 + 186;

      // Coordinates of next tick for zone fill
      float sx2 = cos((i + 5 - 90) * 0.0174532925);
      float sy2 = sin((i + 5 - 90) * 0.0174532925);
      uint16_t x2 = sx2 * (133 + tl) + 160;
      uint16_t y2 = sy2 * (133 + tl) + 186;
      uint16_t x3 = sx2 * 133 + 160;
      uint16_t y3 = sy2 * 133 + 186;

      // Green zone limits
      if (i >= -50 && i < 0) {
        spr.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_GREEN);
        spr.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_GREEN);
      }

      // Yellow zone limits
      if (i >= 0 && i < 25) {
        spr.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_YELLOW);
        spr.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_YELLOW);
      }

      // Red zone limits
      if (i >= 25 && i < 50) {
        spr.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_RED);
        spr.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_RED);
      }

      // Short scale tick length
      if (i % 25 != 0) {
          tl = 8;
      }

      // Recalculate coords incase tick lenght changed
      x0 = sx * (133 + tl) + 160; 
      y0 = sy * (133 + tl) + 186;
      x1 = sx * 133 + 160;
      y1 = sy * 133 + 186;

      // Draw tick
      spr.drawLine(x0, y0, x1, y1, TFT_BLACK);

      // Check if labels should be drawn, with position tweaks
      if (i % 25 == 0) {
          // Calculate label positions
          x0 = sx * (133 + tl + 10) + 160;
          y0 = sy * (133 + tl + 10) + 186;
          switch (i / 25) {
              case -2: spr.drawCentreString("0", x0, y0 - 12, 2); break;
              case -1: spr.drawCentreString("25", x0, y0 - 9, 2); break;
              case 0: spr.drawCentreString("50", x0, y0 - 7, 2); break;
              case 1: spr.drawCentreString("75", x0, y0 - 9, 2); break;
              case 2: spr.drawCentreString("100", x0, y0 - 12, 2); break;
          }
      }

      // Now draw the arc of the scale
      sx = cos((i + 5 - 90) * 0.0174532925);
      sy = sin((i + 5 - 90) * 0.0174532925);
      x0 = sx * 133 + 160;
      y0 = sy * 133 + 186;
      // Draw scale arc, don't draw the last part
      if (i < 50) {
          spr.drawLine(x0, y0, x1, y1, TFT_BLACK);
      }
  }
}

// #########################################################################
// Update needle position
// This function is blocking while needle moves, time depends on ms_delay
// 10ms minimises needle flicker if text is drawn within needle sweep area
// Smaller values OK if text not in sweep area, zero for instant movement but
// does not look realistic... (note: 100 increments for full scale deflection)
// #########################################################################
void plotNeedle()
{
  SERIAL.println("--------COMIENZA");
  maxdB=0;
  SERIAL.println(WINDOW_SIZE);

  for (uint32_t j = 0; j < WINDOW_SIZE; j++){
    if (results0Ready){                                                          // Display the results in results0 array
      // for (uint32_t i = 0; i < NO_RESULTS; i++)
      // {
      //   //SERIAL.println(adcResults0[i]);
      //   //SERIAL.print(i);
      //   //SERIAL.print(F(": "));
      //   //average +=adcResults0[i];


      //   //SERIAL.println(adcResults0[i]);
      // }
      results0Ready = false;                                                  // Clear the results0 ready flag
      digitalWrite(LED_PIN, HIGH);                                            // turn the LED on
    }
    if (results1Ready)                                                        // Display the results in results1 array
    {
      for (uint32_t i = 0; i < NO_RESULTS; i++)
      {
        long soundVolAvg = 0, soundVolMax = 0, soundVolRMS = 0, t0 = millis();
        //cli();  // UDRE interrupt slows this way down on arduino1.0

        int amp = abs(adcResults0[i] - AmpMax);
        amp <<= VolumeGainFactorBits;
        soundVolMax = max(soundVolMax, amp);
        soundVolAvg += amp;
        soundVolRMS += ((long)amp * amp);

        soundVolAvg /= NO_RESULTS;
        soundVolRMS /= NO_RESULTS;
        float soundVolRMSflt = sqrt(soundVolRMS);
        //sei();
        //SERIAL.println(soundVolMax);
        dB = -20.0 * log10(soundVolRMSflt / AmpMax);

        // convert from 0 to 100
        soundVolAvg = 100 * soundVolAvg / AmpMax;
        soundVolMax = 100 * soundVolMax / AmpMax;
        soundVolRMSflt = 100 * soundVolRMSflt / AmpMax;
        soundVolRMS = 10 * soundVolRMSflt / 7; // RMS to estimate peak (RMS is 0.7 of the peak in sin)
      }
      if (dB<1000)
      {
        maxdB = max ( maxdB, dB);
      }
    }
  }

  // int val = analogRead(WIO_MIC);
  // readings[average_index] = val;
  // average_index++;
  // if (average_index == WINDOW_SIZE)
  //   average_index = 0;
  // int average = 0;
  // for (int i = 0; i < WINDOW_SIZE; i++)
  // {
  //   average += readings[average_index];
  // }

  // average /= WINDOW_SIZE;
  // dB = average;

  // if (dB < 0) {
  //   dB = 0;    // Limit value to emulate needle end stops
  // }
  // if (dB > 100) {
  //     dB = 100;
  // }

  // //delay(35);

  // spr.setTextColor(TFT_BLACK);

  // float sdeg = map(dB, 0, 100, -140, -40); // Map dB value to angle

  // // Calcualte tip of needle coords
  // float sx = cos(sdeg * 0.0174532925);
  // float sy = sin(sdeg * 0.0174532925);

  // // Calculate x delta of needle start (does not start at pivot point)
  // float tx = tan((sdeg + 90) * 0.0174532925);

  // // Erase old needle image
  // spr.drawLine(int(1.33 * (120 + 20 * ltx - 1)), int(1.33 * (140 - 20)), int(osx - 1), int(osy), TFT_WHITE);
  // spr.drawLine(int(1.33 * (120 + 20 * ltx)), int(1.33 * (140 - 20)), int(osx), int(osy), TFT_WHITE);
  // spr.drawLine(int(1.33 * (120 + 20 * ltx + 1)), int(1.33 * (140 - 20)), int(osx + 1), int(osy), TFT_WHITE);

  
  // // Store new needle end coords for next erase
  // ltx = tx;
  // osx = 130 * sx + 160;
  // osy = 130 * sy + 186;

  // // Draw the needle in the new postion, magenta makes needle a bit bolder
  // // draws 3 lines to thicken needle
  // spr.drawLine(int(1.33 * (120 + 20 * ltx - 1)), int(1.33 * (140 - 20)), int(osx - 1), int(osy), TFT_RED);
  // spr.drawLine(int(1.33 * (120 + 20 * ltx)), int(1.33 * (140 - 20)), int(osx), int(osy), TFT_MAGENTA);
  // spr.drawLine(int(1.33 * (120 + 20 * ltx + 1)), int(1.33 * (140 - 20)), int(osx + 1), int(osy), TFT_RED);


  // spr.fillSprite(TFT_DARKCYAN);
  // spr.setTextColor(TFT_WHITE);
  // spr.setTextSize(4);
  // spr.drawFloat(dB, 1, 83, 20);
  // //spr.drawNumber(media, 83, 20);
  // spr.drawString("dB", 208,20);
  // // // Re-plot text under needle
  // // tft.setTextColor(TFT_BLACK);
  // // tft.drawCentreString("dB", 145, 90, 4); // // Comment out to avoid font 4
  // spr.pushSprite(0,165);
  // delay(5);    
}


void DMAC_1_Handler()                                                     // Interrupt handler for DMAC channel 0
{
  static uint8_t count = 0;                                               // Initialise the count
  if (DMAC->Channel[1].CHINTFLAG.bit.SUSP)                                // Check if DMAC channel 0 has been suspended (SUSP)
  {
    DMAC->Channel[1].CHCTRLB.reg = DMAC_CHCTRLB_CMD_RESUME;               // Restart the DMAC on channel 0
    DMAC->Channel[1].CHINTFLAG.bit.SUSP = 1;                              // Clear the suspend (SUSP)interrupt flag
    if (count)                                                            // Test if the count is 1
    {
      results1Ready = true;                                               // Set the results 1 ready flag
    }
    else
    {
      results0Ready = true;                                               // Set the results 0 ready flag
    }
    count = (count + 1) % 2;                                              // Toggle the count between 0 and 1
  }
}
