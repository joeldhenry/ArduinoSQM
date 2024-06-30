#include <math.h>
#include <FreqMeasure.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Sky Quality Meter
#define SQM_OFFSET 21
#define SQM_GAIN 1
#define SQM_SAMPLES 5
#define SQM_AVG_DELAY 5

// Display
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET     -1// Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// PINS
// TSL237 - D8
// OLED SDA - A4
// OLED SCL - A5

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // put your setup code here, to run once:
  FreqMeasure.begin();
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.display();
}

double sum=0;
int count=0;

void loop() {
  double mag = getMag();
  Serial.print("Mag: ");
  Serial.println(mag);
  String sqmString = String(mag, 2);
  String bortleString = getBortle(mag);
  updateDisplaySQM(sqmString, bortleString);
  delay(5000);
  display.clearDisplay();
  display.display();
}

double getMag() {
    int count = 0;
    double sum = 0.0;
    updateDisplayReading(0, "");
    while (count < SQM_SAMPLES) {
      if (FreqMeasure.available()) {
        double frequency = FreqMeasure.read();
        sum = sum + frequency;
        count++;
        String frequencyString = String(frequency, 2);
        updateDisplayReading(count, frequencyString);
        delay(SQM_AVG_DELAY);
      }
    }
    float frequency = FreqMeasure.countToFrequency(sum / count);
    Serial.print("Frequency: ");
    Serial.println(frequency);
    return SQM_OFFSET - 2.5 * log10(frequency) * SQM_GAIN;
}

void updateDisplaySQM(String mag, String bortle){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(7,2);
  display.print("Mag/Arc-Sec");
  display.setTextSize(3);
  display.setCursor(5, 17);
  display.print(mag);
  display.setCursor(5, 47);
  display.setTextSize(2);
  display.print(bortle);
  display.display();
}

void updateDisplayReading(int count, String frequency){

  String percentage = String((count / SQM_SAMPLES) * 100, 0) + "%";
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(7,2);
  display.print(percentage);
  display.setTextSize(2);
  display.setCursor(5, 17);
  display.print("Reading...");
  display.setCursor(5, 47);
  display.setTextSize(2);
  display.print(frequency);
  display.display();
}

String getBortle(double mag) {
  if (mag >= 21.99) {
    return "Bortle 1";
  } else if (mag >= 21.89) {
    return "Bortle 2";
  } else if (mag >= 21.69) {
    return "Bortle 3";
  } else if (mag >= 21.25) {
    return "Bortle 4";
  } else if (mag >= 20.49) {
    return "Bortle 4.5";
  } else if (mag >= 19.50) {
    return "Bortle 5";
  } else if (mag >= 18.95) {
    return "Bortle 6";
  } else if (mag >= 18.38) {
    return "Bortle 7";
  } else if (mag >= 17.80) {
    return "Bortle 8";
  } else {
    return "Bortle 9";
  }
}
