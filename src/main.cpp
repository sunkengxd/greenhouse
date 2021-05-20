#include <Arduino.h>
#include <arduino-timer.h>
#include "pins.h"
#include "constants.h"
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>

Timer<4> timer; // 4 tasks
LiquidCrystal_I2C lcd(0x27, 16, 2);
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
  temperature < TEMPERATURE_THRESHOLD ? digitalWrite(COOLER_RELAY, HIGH) : digitalWrite(COOLER_RELAY, LOW);

  humidity < HUMIDITY_THRESHOLD ? digitalWrite(VENT_RELAY, HIGH) : digitalWrite(VENT_RELAY, LOW);

  moisture < MOISTURE_THRESHOLD ? digitalWrite(WATER_PUMP_RELAY, HIGH) : digitalWrite(WATER_PUMP_RELAY, LOW);

  co2 > CO2_THRESHOLD ? digitalWrite(AIR_PUMP_RELAY, HIGH) : digitalWrite(AIR_PUMP_RELAY, LOW);

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
  co2_sensor.begin(v400, v40000);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  pinMode(LAMP_RELAY, OUTPUT);
  pinMode(WATER_PUMP_RELAY, OUTPUT);
  pinMode(COOLER_RELAY, OUTPUT);
  pinMode(VENT_RELAY, OUTPUT);
  pinMode(AIR_PUMP_RELAY, OUTPUT);
  
  pinMode(NEXT_BUTTON, INPUT_PULLUP);
  pinMode(BACK_BUTTON, INPUT_PULLUP);
  
  timer.every(2000, check_sensors);
  timer.every(2000, relay_control);
  timer.every(LAMP_PERIOD, switch_lamp);
  timer.every(SEND_PERIOD, send_data);
}

void loop() {
  timer.tick();
  lcd_navigation();
}