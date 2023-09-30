#include <OneWire.h>
#include <DallasTemperature.h>

// RGB pins
#define RED_PIN 1
#define GREEN_PIN 4
#define BLUE_PIN 0

// Temperature Sensor
#define TEMP_PIN PB3

#define FADE_TIME 2

float darkening_factor = 0.6;  // Set between 0 and 1
float lower_bound = 0.05;
float upper_bound = 0.25;

int prevTemp = 0;

OneWire oneWire(TEMP_PIN);
DallasTemperature sensor(&oneWire);
uint8_t currentR = 0, currentG = 0, currentB = 0;

uint32_t arr[] = { 0x0000FF, 0x00FFFF, 0x00FFFF, 0x00FF00, 0x7FFF00, 0x7FFF7F, 0xFF7F00, 0xFFFF00, 0xFFFF7F, 0xFF0000, 0xFF007F, 0x7F007F };


void reset_color() {
  digitalWrite(RED_PIN, 0);
  digitalWrite(BLUE_PIN, 0);
  OCR1B = 0;
}

void set_color(int r, int g, int b) {

  currentR = r;
  currentG = g;
  currentB = b;

  g = g / 4;
  b = b / 4;

  analogWrite(RED_PIN, r);
  analogWrite(BLUE_PIN, b);
  OCR1B = g;
}

void set_color(uint32_t hexColor) {
  int r = (hexColor >> 16) & 0xFF;
  int g = (hexColor >> 8) & 0xFF;
  int b = hexColor & 0xFF;
  set_color(r, g, b);
}

uint32_t getCurrentColor() {

  uint32_t toReturn = ((uint32_t)0x0 << 24) | ((uint32_t)currentR << 16) | ((uint32_t)currentG << 8) | (uint32_t)currentB;
  return toReturn;
}

void fade_to_color(uint32_t fromHexColor, uint32_t toHexColor, float howManySeconds) {

  if (fromHexColor == toHexColor)
    return;

  float fromR = (float)((fromHexColor >> 16) & 0xFF);
  float fromG = (float)((fromHexColor >> 8) & 0xFF);
  float fromB = (float)(fromHexColor & 0xFF);

  float toR = (float)((toHexColor >> 16) & 0xFF);
  float toG = (float)((toHexColor >> 8) & 0xFF);
  float toB = (float)(toHexColor & 0xFF);

  int steps = (int)(howManySeconds * 1000.0f / 20.0f);

  for (int i = 0; i <= steps; i++) {
    float fraction = (float)i / (float)steps;
    uint8_t curR = (uint8_t)(fromR + (toR - fromR) * fraction);
    uint8_t curG = (uint8_t)(fromG + (toG - fromG) * fraction);
    uint8_t curB = (uint8_t)(fromB + (toB - fromB) * fraction);

    set_color(curR, curG, curB);
    delay(20);
  }
}

uint32_t colorForTemperature(int temp) {
  if (temp <= 17) return 0x0000FF;                // Deep Blue
  if (temp >= 18 && temp <= 20) return 0x00FFFF;  // Light Blue
  if (temp >= 21 && temp <= 22) return 0x00FFFF;  // Cyan
  if (temp >= 23 && temp <= 24) return 0x00FF00;  // Green
  if (temp == 25) return 0x7FFF00;                // Light Green
  if (temp == 26) return 0x7FFF7F;                // Green with Orange
  if (temp == 27) return 0xFF7F00;                // Orange
  if (temp == 28) return 0xFFFF00;                // Yellow
  if (temp == 29) return 0xFFFF7F;                // Yellow-Red
  if (temp >= 30 && temp <= 31) return 0xFF0000;  // Red
  if (temp == 32) return 0xFF007F;                // Red-Purple
  if (temp >= 33) return 0x7F007F;                // Purple
  return 0x000000;                                // Default Black
}

void fade_based_on_temperature(int temp, uint32_t howManySeconds) {
  uint32_t currentColor = getCurrentColor();
  uint32_t targetColor = colorForTemperature(temp);

  fade_to_color(currentColor, targetColor, howManySeconds);

  set_color(targetColor);
}


void fade_out() {
  fade_to_color((currentR << 16) | (currentG << 8) | currentB, 0x000000, FADE_TIME);
}

void fade_in(uint32_t hexColor) {
  fade_to_color(0x000000, hexColor, FADE_TIME);
}

void fade_out_in(float howManySeconds) {
  // Get the current color in hex format
  uint32_t current_color = getCurrentColor();

  // Extract RGB components
  uint8_t originalR = (current_color >> 16) & 0xFF;
  uint8_t originalG = (current_color >> 8) & 0xFF;
  uint8_t originalB = current_color & 0xFF;

  // Calculate the darker color
  uint8_t darkerR = (uint8_t)((float)originalR * darkening_factor);
  uint8_t darkerG = (uint8_t)((float)originalG * darkening_factor);
  uint8_t darkerB = (uint8_t)((float)originalB * darkening_factor);

  // Assemble them into a single unsigned int
  uint32_t darker_color = ((uint32_t)darkerR << 16) | ((uint32_t)darkerG << 8) | (uint32_t)darkerB;
  float transitionTime = howManySeconds / 2;

  // Fade to darker color
  fade_to_color(current_color, darker_color, transitionTime);

  // Fade back to original color
  fade_to_color(darker_color, current_color, 1.0);
}


void setup() {

  pinMode(RED_PIN, INPUT);
  srand(analogRead(RED_PIN));

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Setup Timer/Counter0 for red and green channels
  TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);
  TCCR0B |= (1 << CS00);

  // Timer/Counter1 for Blue (PB4)
  GTCCR |= (1 << PWM1B) | (1 << COM1B1);  // Enable PWM and set compare output mode

  reset_color();
  sensor.begin();

  // Hello from RGB
  set_color(255, 0, 0);
  delay(500);
  set_color(0, 255, 0);
  delay(500);
  set_color(0, 0, 255);
  delay(500);
}

void rainbow() {
  uint32_t arrsize = sizeof(arr) / sizeof(arr[0]);

  for (int i = 0; i < arrsize; i++) {
    fade_to_color(getCurrentColor(), arr[i], .2);
  }
}


void loop() {

  // surprise!
  if (random(0, 100) == 3)
    rainbow();

  sensor.requestTemperatures();
  int temp = (int)sensor.getTempCByIndex(0);

  if (prevTemp != temp) {
    prevTemp = temp;
    fade_based_on_temperature((int)temp, random(3, 10));
  }

  delay(random(2000, 10000));
  darkening_factor = lower_bound + static_cast<float>(random(0, 10000)) / 10000.0 * (upper_bound - lower_bound);

  fade_out_in(random(2, 10));
}