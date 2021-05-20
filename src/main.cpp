#include <Arduino.h>
#include <arduino-timer.h>
#include "pins.h"
#include "constants.h"
#include <rgb_lcd.h>
#include <DHT.h>
#include <DHT_U.h>

Timer<4> timer; // 4 tasks
rgb_lcd lcd;
DHT dht(DHT_PIN, DHT_TYPE);

int temperature = 0,
    humidity = 0,
    moisture = 0,
    co2 = 0;

bool lamp_state = false;
int currentMessage = 0;

bool check_sensors(void *) {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  moisture = map(analogRead(MOISTURE_PIN), 0, 1023, 0, 100);
  co2 = analogRead(MQ7);

  return true;
}

bool relay_control(void *) {
  temperature < TEMPERATURE_THRESHOLD ? digitalWrite(COOLER, HIGH) : digitalWrite(COOLER, LOW);

  humidity < HUMIDITY_THRESHOLD ? digitalWrite(VENT, HIGH) : digitalWrite(VENT, LOW);

  moisture < MOISTURE_THRESHOLD ? digitalWrite(WATER_PUMP, HIGH) : digitalWrite(WATER_PUMP, LOW);

  co2 > CO2_THRESHOLD ? digitalWrite(AIR_PUMP, HIGH) : digitalWrite(AIR_PUMP, LOW);

  return true;
}

bool switch_lamp(void *) {
  lamp_state = !lamp_state;
  digitalWrite(LAMP_RELAY, lamp_state);

  return true;
}

bool send_data(void *) {
  int data[] = {
    temperature, humidity, moisture, co2
  };

  for(const int &it : data) {
    if(Serial.available() <= 0) 
      Serial.write(it);
  }

  return true;
}

void lcd_navigation() {
  if(!digitalRead(NEXT_BUTTON) && currentMessage <= 4) { 
    currentMessage++;
    lcd.clear();
  }

  if(!digitalRead(BACK_BUTTON) && currentMessage > 0) {
    currentMessage--;
    lcd.clear();
  }

  switch (currentMessage) {
    case 1:
      lcd.print("Air temperature: ");
      lcd.print(temperature);
      break;

    case 2:
      lcd.print("Air humidity: ");
      lcd.print(humidity);
      break;
      
    case 3:
      lcd.print("Soil moisture: ");
      lcd.print(moisture);
      break;
      
    case 4:
      lcd.print("CO2 level: ");
      lcd.print(co2);
      break;
    
    default:
      break;
  }  
}

void setup() {
  Serial.begin(9600);

  dht.begin();

  lcd.begin(16, 2);
  lcd.setRGB(rED, GREEN, BLUE);
  lcd.setCursor(0, 0);

  pinMode(LAMP, OUTPUT);
  pinMode(WATER_PUMP, OUTPUT);
  pinMode(COOLER, OUTPUT);
  pinMode(VENT, OUTPUT);
  pinMode(AIR_PUMP, OUTPUT);
  
  pinMode(NEXT_BUTTON, INPUT_PULLUP);
  pinMode(BACK_BUTTON, INPUT_PULLUP);
  
  timer.every(2000, check_sensors);
  timer.every(2000, relay_control);
  //timer.every(LAMP_PERIOD, switch_lamp);
  timer.every(SEND_PERIOD, send_data);
}

void loop() {
  timer.tick();
  lcd_navigation();
}