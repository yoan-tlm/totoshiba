#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <ToshibaHeatpumpIR.h>
#include "PushButton.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// IR
IRSenderPWM irSender(9); 
HeatpumpIR *heatpumpIR = new ToshibaHeatpumpIR();

// Temperature
int temperature = 22;
int fan = FAN_AUTO;
int mode = MODE_AUTO;

// Button Up
PushButton* buttonUp;
PushButton* buttonDown;
PushButton* buttonOn;
PushButton* buttonOff;
PushButton* buttonFan;
PushButton* buttonMode;

// Debounce
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay =50;    // the debounce time; increase if the output flickers


void setup() {
  Serial.begin(9600);
  buttonUp = new PushButton(2);
  buttonDown = new PushButton(3);
  buttonOff = new PushButton(4);
  buttonOn = new PushButton(5);
  buttonFan = new PushButton(6);
  buttonMode = new PushButton(8);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(10,10);             // Start at top-left corner
  display.setTextSize(2);             // Draw 2X-scale text
  display.println(F("Totoshiba"));
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  
  draw();    // Draw 'stylized' characters

}

void loop() {
    // read the state of the pushbutton value:
    buttonUp->readPin();
    buttonDown->readPin();
    buttonOn->readPin();
    buttonOff->readPin();
    buttonFan->readPin();
    buttonMode->readPin();
    // If the switch changed, due to noise or pressing:
  if (buttonUp->getState() != buttonUp->getLastState()
   || buttonDown->getState() != buttonDown->getLastState()
    || buttonOn->getState() != buttonOn->getLastState()
     || buttonOff->getState() != buttonOff->getLastState()
     || buttonFan->getState() != buttonFan->getLastState()
     || buttonMode->getState() != buttonMode->getLastState()) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {


  if (buttonUp->getState() == LOW) {
    if(temperature<25)
      temperature++;
    draw();
  } else if (buttonDown->getState() == LOW) {
    if(temperature>18)
      temperature--;
    draw();
  } else if (buttonOn->getState() == LOW) {
    send();
  } else if (buttonOff->getState() == LOW) {
    sendOff();
  } else if (buttonMode->getState() == LOW) {
    changeMode();
    draw();
  } else if (buttonFan->getState() == LOW) {
    changeFan();
    draw();
  } 
  }
}

void changeMode(){
  if(mode < MODE_FAN){
    mode ++;
  }else {
    mode = MODE_AUTO;
  }
}

void changeFan(){
  if(fan < FAN_SILENT){
    fan ++;
  }else {
    fan = FAN_AUTO;
  }
}

String readFan(){
  switch (fan){
    case FAN_1: return "Speed 1";
    case FAN_2: return "Speed 2";
    case FAN_3: return "Speed 3";
    case FAN_4: return "Speed 4";
    case FAN_5: return "Speed 5";
    case FAN_SILENT: return "Silence";
    default: return "Auto";
  }
}

String readMode(){
  switch (mode){
    case MODE_HEAT: return "Heat";
    case MODE_COOL: return "Cool";
    case MODE_DRY: return "Dry";
    case MODE_FAN: return "Fan";
    case MODE_MAINT: return "Maintenance";
    default: return "Auto";
  }
}

void send(void) {
    // Send the IR command
    heatpumpIR->send(irSender, POWER_ON, mode, fan, temperature, VDIR_AUTO, HDIR_AUTO);
    delay(200);
}

void sendOff(void) {
    // Send the IR command
    heatpumpIR->send(irSender, POWER_OFF, mode, fan, temperature, VDIR_AUTO, HDIR_AUTO);
    delay(200);
}

void draw(void) {
  display.clearDisplay();

  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.print(F("Mode: "));
  display.println(readMode());
  display.print(F("Fan: "));
  display.println(readFan());

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.setCursor(10,20);             // Start at top-left corner
  display.setTextSize(1);             // Draw 2X-scale text
  display.print(F("Temperature: "));
  display.print(temperature, DEC);
  
  display.cp437(true);
  display.write(248);
  display.println(F("C"));

  display.display();
  delay(200);
}

