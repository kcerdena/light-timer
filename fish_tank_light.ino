// Sunrise/Sunset Fish Tank Light Switch
//  by Kacey Cerdena

// Hardware:
//  Arduino Pro Micro ATmega32U4 (5v, 16MHz)
//  DS3231 Real Time Clock
//  SRD-05VDC-SL-C Relay
//  Aquatop Pisces 5G Aquarium LED Light (modified)

// Wiring:
//  +12v Fish Tank Light to Arduino RAW (Yellow Wire +11.91v) 
//  GND Fish Tank Light to Arduino GND (Black Wire)
//  Arduino VCC to RTC VCC
//  Arduino GND to RTC GND
//  Arduino PIN 2 to RTC SDA
//  Arduino PIN 3 to RTC SCL
//  Arduino VCC to Relay VCC
//  Arduino GND to Relay GND
//  Arduino PIN 7 to Relay INPUT
//  Relay GND to Relay COM
//  Relay NO to White Light PWR (Red Wire +7.2v) 

// Globals
#include <RTClib.h>

RTC_DS3231 RTC;
bool ERROR_COND = false;
int LIGHT_PIN = 7;

void setup() {
// Setup the Serial
  Serial.begin(57600);
  // while (!Serial) delay(10);
  
// Setup the RTC
  if (! RTC.begin()) {
    Serial.println("Couldn't find RTC");
    ERROR_COND = true;
  }
  if (RTC.lostPower()) {
    Serial.println("RTC lost power. Set new time!");
    ERROR_COND = true;
  }
// SET RTC TIME after battery change.
// Upload UNCOMMENTED to set time.
// Upload again COMMENTED to keep time after power loss.
// rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

// Setup PIN 7 for LIGHT
  pinMode(LIGHT_PIN, OUTPUT);
}

DateTime get_sol_time(DateTime now, int varient) {
  // varient values: 0 = sunrise, 1 = sunset
  // times are non-DST
  String sol_lookup[13][2] = {
    {"0","0"},
    {"08:00:00","17:10:00"},
    {"07:30:00","17:50:00"},
    {"06:50:00","18:40:00"},
    {"05:50:00","19:20:00"},
    {"04:50:00","20:00:00"},
    {"04:20:00","20:10:00"},
    {"04:50:00","20:10:00"},
    {"05:30:00","19:40:00"},
    {"06:10:00","18:50:00"},
    {"06:50:00","17:50:00"},
    {"07:40:00","16:50:00"},
    {"08:00:00","16:20:00"}
  };
  String sol_time_str = now.timestamp(DateTime::TIMESTAMP_DATE)+"T"+sol_lookup[now.month()][varient];
  const char *sol_time_char = sol_time_str.c_str();
  return DateTime(sol_time_char); 
}

void loop() {
// If in ERROR CONDITION, flash the light.
  if (ERROR_COND) {
    digitalWrite(LIGHT_PIN, HIGH);
    delay(2000);
    digitalWrite(LIGHT_PIN, LOW);
    delay(2000);
  } else {
    // get the current non-DST time
    DateTime now = RTC.now();
    Serial.println("Current time: " + now.timestamp());
    // get sunrise time
    DateTime sunrise = get_sol_time(now, 0);
    Serial.println("Sunrise: " + sunrise.timestamp());
    // get sunset time
    DateTime sunset = get_sol_time(now, 1);
    Serial.println("Sunset: " + sunset.timestamp());
    
    if (now > sunset) {
      // Is it after sunset?
      // Turn off the light.
      digitalWrite(LIGHT_PIN, HIGH);
      Serial.println("Light Mode: OFF");
    } else if (now > sunrise) {
      // Is it after sunrise?
      // Turn on the light.
      digitalWrite(LIGHT_PIN, LOW);
      Serial.println("Light Mode: ON");
    } else {
      // Is it before sunrise?
      // Turn off the light.
      digitalWrite(LIGHT_PIN, HIGH);
      Serial.println("Light Mode: OFF");
    }
    // WAIT 60 seconds
    delay(60000);
    Serial.println("");
  }
}
