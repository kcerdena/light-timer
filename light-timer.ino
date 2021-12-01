// Sunrise/Sunset Fish Tank Light Switch
//  by Kacey Cerdena

#include <RTClib.h>

RTC_DS3231 RTC;
bool ERROR_COND = false;
int LIGHT_PIN = 9;
// PWM Range 0 - 255;
int LIGHT_LEVEL;

void setup()
{
  // Setup the Serial
  Serial.begin(57600);

  // Setup the RTC
  if (!RTC.begin())
  {
    Serial.println("Couldn't find RTC");
    ERROR_COND = true;
  }
  if (RTC.lostPower())
  {
    Serial.println("RTC lost power. Set new time!");
    ERROR_COND = true;
  }
  // SET RTC TIME after battery change.
  // Upload UNCOMMENTED to set time.
  // Upload again COMMENTED to keep time after power loss.
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Setup PIN 9 for LIGHT
  pinMode(LIGHT_PIN, OUTPUT);

  // If in ERROR CONDITION, flash the light.
  if (ERROR_COND)
  {
    while (true)
    {
      digitalWrite(LIGHT_PIN, HIGH);
      delay(2000);
      digitalWrite(LIGHT_PIN, LOW);
      delay(2000);
    }
  }

  // Set initial light state.
  if (light_should_be_on(RTC.now()))
  {
    LIGHT_LEVEL = 255;
    digitalWrite(LIGHT_PIN, HIGH);
  }
  else
  {
    LIGHT_LEVEL = 0;
    digitalWrite(LIGHT_PIN, LOW);
  }
}

DateTime get_sol_time(DateTime now, int varient)
{
  // varient values: 0 = sunrise, 1 = sunset
  // times are non-DST
  String sol_lookup[13][2] = {
      {"0", "0"},
      {"08:00:00", "17:10:00"},
      {"07:30:00", "17:50:00"},
      {"06:50:00", "18:40:00"},
      {"05:50:00", "19:20:00"},
      {"04:50:00", "20:00:00"},
      {"04:20:00", "20:10:00"},
      {"04:50:00", "20:10:00"},
      {"05:30:00", "19:40:00"},
      {"06:10:00", "18:50:00"},
      {"06:50:00", "17:50:00"},
      {"07:40:00", "16:50:00"},
      {"08:00:00", "16:20:00"}};
  String sol_time_str = now.timestamp(DateTime::TIMESTAMP_DATE) + "T" + sol_lookup[now.month()][varient];
  const char *sol_time_char = sol_time_str.c_str();
  return DateTime(sol_time_char);
}

bool light_should_be_on(DateTime now)
{
  // Return: false = OFF; true = ON
  DateTime sunrise = get_sol_time(now, 0);
  DateTime sunset = get_sol_time(now, 1);
  Serial.println("Sunrise: " + sunrise.timestamp());
  Serial.println("Sunset: " + sunset.timestamp());
  if (now > sunset || now < sunrise)
  {
    Serial.println("Expected Light Mode: OFF");
    return false;
  }
  else
  {
    Serial.println("Expected Light Mode: ON");
    return true;
  }
}

void transition_light(bool want_light_on)
{
  if (want_light_on) {
    Serial.println("Turning light on.");
    while (LIGHT_LEVEL < 255) {
      LIGHT_LEVEL++;
      analogWrite(LIGHT_PIN, LIGHT_LEVEL);
      int ll_pct = (LIGHT_LEVEL / 255.0) * 100;
      Serial.println(ll_pct);
      delay(2500);
    }
    digitalWrite(LIGHT_PIN, HIGH);
    Serial.println("Light Mode: ON");
  } else {
    Serial.println("Turning light off.");
    while (LIGHT_LEVEL > 0) {
      LIGHT_LEVEL--;
      analogWrite(LIGHT_PIN, LIGHT_LEVEL);
      int ll_pct = (LIGHT_LEVEL / 255.0) * 100;
      Serial.println(ll_pct);
      delay(2500);
    }
    digitalWrite(LIGHT_PIN, LOW);
    Serial.println("Light Mode: OFF");
  }
}

void loop()
{
  // get the current non-DST time
  DateTime now = RTC.now();
  Serial.println("Current time: " + now.timestamp());
  bool want_light_on = light_should_be_on(now);

  // transition light if necessary
  if (want_light_on && LIGHT_LEVEL < 255) {
    transition_light(want_light_on);
  } else if (!want_light_on && LIGHT_LEVEL > 0) {
    transition_light(want_light_on);
  }

  // WAIT 60 seconds
  delay(60000);
  Serial.println("");
}
