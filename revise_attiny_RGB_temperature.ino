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

SoftwareSerial mySerial(PB3,PB2);
OneWire oneWire(TEMP_PIN);
DallasTemperature sensor(&oneWire);


uint8_t currentR = 0, currentG = 0, currentB = 0;

// Common code for both sources of an ADC conversion
int getADC() {
  ADCSRA  |=_BV(ADSC);           // Start conversion
  while((ADCSRA & _BV(ADSC)));    // Wait until conversion is finished
  return ADC;
}

float chipTemp(float raw) {
  const float chipTempOffset = 272.9;           // Your value here, it may vary 
  const float chipTempCoeff = 1.075;            // Your value here, it may vary
  return((raw - chipTempOffset) / chipTempCoeff);
}

int getChipTemperatureCelsius() {
  int i;
  int t_celsius; 
  uint8_t vccIndex;
  float rawTemp, rawVcc;
  
  // Measure temperature
  ADCSRA |= _BV(ADEN);           // Enable AD and start conversion
  ADMUX = 0xF | _BV( REFS1 );    // ADC4 (Temp Sensor) and Ref voltage = 1.1V;
  delay(SETTLE);                 // Settling time min 1 ms, wait 100 ms

  rawTemp = (float)getADC();     // use next sample as initial average
  for (int i=2; i<2000; i++) {   // calculate running average for 2000 measurements
    rawTemp += ((float)getADC() - rawTemp) / float(i); 
  }  
  ADCSRA &= ~(_BV(ADEN));        // disable ADC  

  // Measure chip voltage (Vcc)
  ADCSRA |= _BV(ADEN);   // Enable ADC
  ADMUX  = 0x0c | _BV(REFS2);    // Use Vcc as voltage reference, 
                                 //    bandgap reference as ADC input
  delay(SETTLE);                    // Settling time min 1 ms, there is 
                                 //    time so wait 100 ms
  rawVcc = (float)getADC();      // use next sample as initial average
  for (int i=2; i<2000; i++) {   // calculate running average for 2000 measurements
    rawVcc += ((float)getADC() - rawVcc) / float(i);
  }
  ADCSRA &= ~(_BV(ADEN));        // disable ADC
  
  rawVcc = 1024 * 1.1f / rawVcc;
  //index 0..13 for vcc 1.7 ... 3.0
  vccIndex = min(max(17,(uint8_t)(rawVcc * 10)),30) - 17;   

  // Temperature compensation using the chip voltage 
  // with 3.0 V VCC is 1 lower than measured with 1.7 V VCC 
  t_celsius = (int)(chipTemp(rawTemp) + (float)vccIndex / 13);  
                                                                                   
  return t_celsius;
}

void reset_color(){
  //reset
   digitalWrite(RED_PIN,0);
   digitalWrite(GREEN_PIN,0);
   OCR1B=0;
  
}

void set_color(int r, int g, int b){
  
  currentR = r;
  currentG = g;
  currentB = b;

  g = g/4;
  b = b/4;

  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  OCR1B = b;
 
  
}

void set_color(uint32_t  hexColor) {
  int r = (hexColor >> 16) & 0xFF;
  int g = (hexColor >> 8) & 0xFF;
  int b = hexColor & 0xFF;
  set_color(r, g, b);
}


uint32_t getCurrentColor() {
  return (currentR << 16) | (currentG << 8) | currentB;
}


void fade_to_color(uint32_t fromHexColor, uint32_t toHexColor, uint32_t howManySeconds) {
  int fromR = (fromHexColor >> 16) & 0xFF;
  int fromG = (fromHexColor >> 8) & 0xFF;
  int fromB = fromHexColor & 0xFF;

  int toR = (toHexColor >> 16) & 0xFF;
  int toG = (toHexColor >> 8) & 0xFF;
  int toB = toHexColor & 0xFF;

  int steps = howManySeconds * 1000 / 20;
  for (int i = 0; i <= steps; i++) {
    float fraction = (float)i / steps;
    int curR = fromR + (toR - fromR) * fraction;
    int curG = fromG + (toG - fromG) * fraction;
    int curB = fromB + (toB - fromB) * fraction;
    set_color(curR, curG, curB);
    delay(20);
  }
}

uint32_t colorForTemperature(int temp) {
  if (temp <= 17) return 0x0000FF; // Deep Blue
  if (temp >= 18 && temp <= 20) return 0x00FFFF; // Light Blue
  if (temp >= 21 && temp <= 22) return 0x00FFFF; // Cyan
  if (temp >= 23 && temp <= 24) return 0x00FF00; // Green
  if (temp == 25) return 0x7FFF00; // Light Green
  if (temp == 26) return 0x7FFF7F; // Green with Orange
  if (temp == 27) return 0xFF7F00; // Orange
  if (temp == 28) return 0xFFFF00; // Yellow
  if (temp == 29) return 0xFFFF7F; // Yellow-Red
  if (temp >= 30 && temp <= 31) return 0xFF0000; // Red
  if (temp == 32) return 0xFF007F; // Red-Purple
  if (temp >= 33) return 0x7F007F; // Purple
  return 0x000000; // Default Black
}

void fade_based_on_temperature(int temp, uint32_t howManySeconds) {
  uint32_t currentColor = getCurrentColor(); // Assume this function returns the current color
  uint32_t targetColor = colorForTemperature(temp);
  char buf[20];
  sprintf(buf, "Color: 0x%08lx", (unsigned long) targetColor);
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
  GTCCR |= (1 << PWM1B) | (1 << COM1B1); // Enable PWM and set compare output mode

  reset_color();

  mySerial.begin(9600);
  sensor.begin();
  // fade_in(0x111111);
  // fade_out();
  // fade_in(0x333333);
  // fade_out();
  // fade_in(0x666666);
  // fade_out();
  
}

void loop() {

  // int a = getChipTemperatureCelsius();
   mySerial.println("hello");
  // mySerial.println(a);
  //set_color(0xFFFF00);
  //set_color(0xFF7F00);
  //set_color(0xFF0000);
  sensor.requestTemperatures();
  double temp = sensor.getTempCByIndex(0);

  mySerial.print("Temp: ");
  mySerial.println(temp);


  //fade_based_on_temperature((int)temp, 5);
  

  delay(1000); 
 
  //Fade in to red
  //fade_in(0x110000);

  // // Fade from red to green
  // fade_to_color(0x110000, 0x001100, 2);
  // delay(1000);

  // // Fade from green to blue
  // fade_to_color(0x001100, 0x000011, 2);
  // delay(1000);

  // // Fade out
  // fade_out();
  delay(500);
}