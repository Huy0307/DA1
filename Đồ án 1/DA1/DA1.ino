#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define laser 12

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

double temp_amb;
double temp_obj;

const int led_pin = 8;
const int sensor_pin = 5;//sig_pin
const int buzzer_pin = 13;
const int en_pin = 4;

const long interval = 2000;
unsigned long current_time = millis();
unsigned long last_trigger = 0;
boolean timer_on = false;

void movement_detection() {
  Serial.println("Motion was detected");
  digitalWrite(led_pin, HIGH);
  // digitalWrite(buzzer_pin, HIGH);
  // delay(1000);
  digitalWrite(led_pin, LOW);
  digitalWrite(buzzer_pin, LOW);
  timer_on = true;
  last_trigger = millis();
}

void setup() {
  Serial.begin(115200);
  mlx.begin();                                
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  else{
    Serial.println(F("SSD1306 allocation success"));
  }
  pinMode(en_pin, OUTPUT);
  digitalWrite(en_pin, HIGH); // Bật chế độ phát hiện mô-đun hồng ngoại
  pinMode(sensor_pin, INPUT_PULLUP);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, LOW);  

  Serial.println("Temperature Sensor MLX90614");

  pinMode(laser, OUTPUT);
  digitalWrite(laser, LOW);
  display.clearDisplay();
  display.setCursor(25, 15);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(" Thermometer");
  display.setCursor(25, 35);
  display.setTextSize(1);
  display.print("Initializing");
  display.display();
  delay(2000);
}

void loop() {
  int sensor_state = digitalRead(sensor_pin);
  if (sensor_state == LOW) {
    movement_detection();
  }
  current_time = millis();
  if (timer_on && (current_time - last_trigger <= interval)) {
    temp_amb = mlx.readAmbientTempC();
    temp_obj = mlx.readObjectTempC()+1.65;
    
    digitalWrite(laser, HIGH);

    Serial.print("Room Temp = ");
    Serial.println(temp_amb);
    Serial.print("Object temp = ");
    Serial.println(temp_obj);
    
    display.clearDisplay();
    display.setCursor(25, 10);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.println(" Temperature");
    display.setCursor(25, 30);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.print(temp_obj);
    display.println(" C");
    display.display();
    if (temp_obj >= 37.5) {
      digitalWrite(buzzer_pin, HIGH);
      Serial.print("ALERT!!!!\n ");
    }
    delay(2000);

  } 
  if (timer_on && (current_time - last_trigger > interval)) {
    Serial.println("Motion has stopped");
    digitalWrite(led_pin, LOW);//LED tắt
    digitalWrite(buzzer_pin, LOW);//Buzzer tắt
    digitalWrite(laser, LOW);//laser tắt
    timer_on = false;//Tắt timer
    display.clearDisplay();
    display.setCursor(25, 10);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.println(" Waiting for motion.....");
    display.display();
    delay(500);
  }
}
