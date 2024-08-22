//Include libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <WiFi.h>
#include <time.h>

// Define OLED parameters
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS  0x3C

#define BUZZER 5
#define LED_1 15
#define PB_CANCEL 34
#define PB_OK 32
#define PB_UP 33
#define PB_DOWN 35
#define DHTPIN 12

#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET     UTC_hour*3600 + UTC_minute*60
#define UTC_OFFSET_DST 0

// Declare objects
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHTesp dhtSensor;

//Global variables
int date = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;

//UTC Variables
int UTC_hour = 5;
int UTC_minute = 30;

unsigned long timeNow = 0;
unsigned long timeLast = 0;

bool alarm_enabled = true;
int n_alarms = 3;
int alarm_hours[] = {0, 1, 2};
int alarm_minutes[] = {1, 10, 20};
bool alarm_triggered[] = {false, false, false};

//Buzzer Notes
int n_notes= 8;
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440;
int B = 494;
int C_H = 523;
int notes[] = {C, D, E, F, G, A, B, C_H};

int current_mode = 0;
int max_modes = 5;
String modes[] = {"1-Set Time UTC", "2-Set Alarm 1", "3-Set Alarm 2", "4-Set Alarm 3", "5-Disable Alarms"};

void setup() { 
  Serial.begin(9600);

  pinMode(BUZZER, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(PB_CANCEL, INPUT);
  pinMode(PB_OK, INPUT);
  pinMode(PB_UP, INPUT);
  pinMode(PB_DOWN, INPUT);

  dhtSensor.setup(DHTPIN, DHTesp::DHT22);  

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3v internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)){
    Serial.println(F("SSD1036 allocation failed"));
    for(;;);
  }

  // Show the display buffer on ther screen. You MUST call display() after
  // drawing commands to make them visible on screen! 
  display.display();
  delay(500);

  display.clearDisplay();
  print_line("Welcome to medibox!", 0, 0, 2);
  delay(500);

  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED){
    delay(250);
    display.clearDisplay();
    print_line("Connecting to WIFI", 0, 0, 2);
  }

  display.clearDisplay();
  print_line("Connected to WIFI", 0, 0, 2);

  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);

  //Clear the buffer
  display.clearDisplay();
}

void loop() {
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
  update_time_with_check_alarm();
  if(digitalRead(PB_OK) == LOW){
    delay(200);
    go_to_menu();
  }
  check_temp();
}

void print_line(String text, int row, int column, int text_size){
  display.setTextSize(text_size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column, row);
  display.println(text);
  display.display();
}

void print_time_now(void){
  display.clearDisplay();
  print_line(String(date) + ":" + String(hours) + ":" + String(minutes) + ":" + String(seconds), 0, 0, 2);
 }

void update_time(){

  struct tm timeinfo;
  getLocalTime(&timeinfo);

  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  hours = atoi(timeHour);

  char timeMinute[3];
  strftime(timeMinute, 3, "%M", &timeinfo);
  minutes = atoi(timeMinute);

  char timeSecond[3];
  strftime(timeSecond, 3, "%S", &timeinfo);
  seconds = atoi(timeSecond);

  char timeDay[3];
  strftime(timeDay, 3, "%d", &timeinfo);
  date = atoi(timeDay);
 }

 void ring_alarm(){
  //show messages on display
  display.clearDisplay();
  print_line("Medicine Time!", 10, 20, 2);

  //light up LED
  digitalWrite(LED_1, HIGH);

  bool break_happend = false;

  //ring the buzzer
  while(break_happend == false && digitalRead(PB_CANCEL) == HIGH){
    for(int i = 0; i < n_notes; i++){
      if(digitalRead(PB_CANCEL) == LOW){
        delay(200);
        break_happend = true;
        break;
      }
      tone(BUZZER, notes[i]);
      delay(500);
      noTone(BUZZER);
      delay(2);
    }

    //turn off LED
    digitalWrite(LED_1, LOW);
    display.clearDisplay();
  }
 }

void update_time_with_check_alarm(){
  update_time();
  print_time_now();

  if(alarm_enabled){
    for(int i = 0; i < n_alarms; i++){
      if(alarm_hours[i] == hours && alarm_minutes[i] == minutes && !alarm_triggered[i]){
        ring_alarm();
        alarm_triggered[i] = true;
      }
    }
  }
 }

int wait_for_button_press(){
  while(true){
    if(digitalRead(PB_CANCEL) == LOW){
      delay(200);
      return PB_CANCEL;
    }
    else if(digitalRead(PB_UP) == LOW){
      delay(200);
      return PB_UP;
    }
    else if(digitalRead(PB_DOWN) == LOW){
      delay(200);
      return PB_DOWN;
    }
    else if(digitalRead(PB_OK) == LOW){
      delay(200);
      return PB_OK;
    }
    update_time();
  }
 }

void go_to_menu(){
  while(digitalRead(PB_CANCEL) == HIGH){
    display.clearDisplay();
    print_line(modes[current_mode], 0, 0, 2);

    int pressed = wait_for_button_press();
    if(pressed == PB_UP){
      current_mode = (current_mode +1) % max_modes;
    }

    else if(pressed == PB_DOWN){
      current_mode = (current_mode - 1);
      if(current_mode < 0){
        current_mode = max_modes - 1;
      }
    }

    else if(pressed == PB_OK){
      run_mode(current_mode);
    }

    else if(pressed == PB_CANCEL){
      delay(200);
      break;
    }
  }
 }

void set_time(){

  int temp_hour = UTC_hour;
  while(true){
    display.clearDisplay();
    print_line("Enter hour: " + String(temp_hour), 0, 0, 2);

    int pressed = wait_for_button_press();
    if(pressed == PB_UP){
      delay(200);
      temp_hour += 1;
      if(temp_hour > 14){
        temp_hour = -12;
      }
    }

    else if (pressed == PB_DOWN){
      delay(200);
      temp_hour -= 1;
      if(temp_hour < -12){
        temp_hour = 14;
      }
    }

    else if (pressed == PB_OK){
      delay(200);
      UTC_hour = temp_hour;
      break;
    }

    else if (pressed == PB_CANCEL){
      delay(200);
      break;
    }
  }

  int temp_minute = UTC_minute;
  while(true){
    display.clearDisplay();
    print_line("Enter minute: " + String(temp_minute), 0, 0, 2);

    int pressed = wait_for_button_press();
    if (pressed == PB_UP){
      delay(200);
      temp_minute += 15;
      temp_minute = temp_minute % 60;
    }

    else if (pressed == PB_DOWN){
      delay(200);
      temp_minute -= 15;
      if (temp_minute < 0){
        temp_minute = 45;
      }
    }

    else if (pressed == PB_OK){
      delay(200);
      UTC_minute = temp_minute;
      break;
    }
    else if (pressed == PB_CANCEL){
      delay(200);
      break;
    }
  }

  display.clearDisplay();
  print_line("Time is set", 0, 0, 2);
  delay(1000);

}

void set_alarm(int alarm){

  int temp_hour = alarm_hours[alarm];
  while (true){
    display.clearDisplay();
    print_line("Enter hour: " + String(temp_hour), 0, 0, 2);

    int pressed = wait_for_button_press();
    if( pressed == PB_UP){
      delay(200);
      temp_hour += 1;
      temp_hour = temp_hour % 24;
    }

    else if (pressed == PB_DOWN){
      delay(200);
      temp_hour -= 1;
      if (temp_hour < 0){
        temp_hour = 23;
      }
    }

    else if (pressed == PB_OK){
      delay(200);
      alarm_hours[alarm] = temp_hour;
      break;
    }

    else if (pressed == PB_CANCEL){
      delay(200);
      break;
    }
  }

  int temp_minute = alarm_minutes[alarm];
  while (true){
    display.clearDisplay();
    print_line("Enter minute: " + String(temp_minute), 0, 0, 2);

    int pressed = wait_for_button_press();
    if (pressed == PB_UP){
      delay(200);
      temp_minute += 1;
      temp_minute = temp_minute % 60;
    }

    else if (pressed == PB_DOWN){
      delay(200);
      temp_minute -= 1;
      if (temp_minute < 0){
        temp_minute = 59;
      }
    }

    else if (pressed == PB_OK){
      delay(200);
      alarm_minutes[alarm] = temp_minute;
      break;
    }

    else if (pressed == PB_CANCEL){
      delay(200);
      break;
    }
  }

  display.clearDisplay();
  print_line("Alarm is set", 0, 0, 2);
  delay(1000);
}

void run_mode(int mode){
  if (mode == 0){
    set_time();
  }

  else if (mode == 1 || mode == 2 || mode == 3){
    set_alarm(mode - 1);
  }

  else if (mode == 4){
    alarm_enabled = false;
    display.clearDisplay();
    print_line("Alarms are disabled", 0, 0, 2);
    delay(2000);
  }

}

void check_temp(){
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  if (data.temperature > 32){
    display.clearDisplay();
    print_line("TEMP HIGH", 0, 0, 2);
    delay(3000);
  }
  else if (data.temperature < 26){
    display.clearDisplay();
    print_line("TEMP LOW", 0, 0, 2);
    delay(3000);
  }

  if (data.humidity > 80){
    display.clearDisplay();
    print_line("HUMIDITY HIGH", 0, 0, 2);
    delay(3000);
  }
  else if (data.humidity < 60){
    display.clearDisplay();
    print_line("HUMIDITY LOW", 0, 0, 2);
    delay(3000);
  }
}


