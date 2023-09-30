#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>



// Define pins
#define RED_PIN 1    // PWM capable
#define GREEN_PIN 0  // PWM capable
#define BLUE_PIN 4   // Manually set to PWM via timer
#define TEMP_PIN PB3

#define SETTLE 1000
#define FADE_TIME 2

SoftwareSerial mySerial(PB3, PB2);
OneWire oneWire(TEMP_PIN);
DallasTemperature sensor(&oneWire);


uint8_t currentR = 0, currentG = 0, currentB = 0;
char buf[45];


void reset_color() {
  //reset
  digitalWrite(RED_PIN, 0);
  digitalWrite(GREEN_PIN, 0);
  OCR1B = 0;
}

void set_color(int r, int g, int b) {

  currentR = r;
  currentG = g;
  currentB = b;

  g = g / 4;
  b = b / 4;

  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  OCR1B = b;


  // sprintf(buf, "set_color: %d %d %d, %d %d %d", currentR, currentG, currentB, r, g, b);
  // mySerial.println(buf);
}

void set_color(uint32_t hexColor) {
  int r = (hexColor >> 16) & 0xFF;
  int g = (hexColor >> 8) & 0xFF;
  int b = hexColor & 0xFF;
  set_color(r, g, b);
}


uint32_t getCurrentColor() {

  uint32_t toReturn = ((uint32_t)0x0 << 24) | ((uint32_t)currentR << 16) | ((uint32_t)currentG << 8) | (uint32_t)currentB;
  sprintf(buf, "getCurrentColor: 0x%08lx %d %d %d", toReturn, currentR, currentG, currentB);
  mySerial.println(buf);
  return toReturn;
}


void fade_to_color(uint32_t fromHexColor, uint32_t toHexColor, uint32_t howManySeconds) {

  if (fromHexColor == toHexColor)
    return;

  int fromR = (fromHexColor >> 16) & 0xFF;
  int fromG = (fromHexColor >> 8) & 0xFF;
  int fromB = fromHexColor & 0xFF;

  int toR = (toHexColor >> 16) & 0xFF;
  int toG = (toHexColor >> 8) & 0xFF;
  int toB = toHexColor & 0xFF;

  int steps = howManySeconds * 1000 / 20;
  uint8_t curR = fromR;
  uint8_t curG = fromG;
  uint8_t curB = fromB;
  for (int i = 0; (i <= steps); i++) {
    float fraction = (float)i / steps;
    curR = fromR + (toR - fromR) * fraction;
    curG = fromG + (toG - fromG) * fraction;
    curB = fromB + (toB - fromB) * fraction;
    // sprintf(buf, "iter:  %d %d %d", curR, curG, curB);
    // mySerial.println(buf);
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
  return 0x000000;                                 // Default Black
}

void fade_based_on_temperature(int temp, uint32_t howManySeconds) {
  uint32_t currentColor = getCurrentColor();
  uint32_t targetColor = colorForTemperature(temp);

  sprintf(buf, "From: 0x%08lx To: 0x%08lx", (unsigned long)currentColor, (unsigned long)targetColor);
  mySerial.println(buf);
  fade_to_color(currentColor, targetColor, howManySeconds);
}




void fade_out() {
  fade_to_color((currentR << 16) | (currentG << 8) | currentB, 0x000000, FADE_TIME);
}

void fade_in(uint32_t hexColor) {
  fade_to_color(0x000000, hexColor, FADE_TIME);
}





void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Setup Timer/Counter0 for red and green channels
  TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);
  TCCR0B |= (1 << CS00);

  // Timer/Counter1 for Blue (PB4)
  GTCCR |= (1 << PWM1B) | (1 << COM1B1);  // Enable PWM and set compare output mode

  reset_color();

  mySerial.begin(9600);
  sensor.begin();
}

void loop() {

  sensor.requestTemperatures();
  double temp = sensor.getTempCByIndex(0);

  mySerial.print("Temp: ");
  mySerial.println(temp);

  fade_based_on_temperature((int)temp, 5);

  delay(1000);

}