// final emo code
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <driver/ledc.h> // Include the LEDC driver for PWM

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <time.h>
#include <SPI.h>


const char* ssid = "RAILWIRE+#@";
const char* password = "gentleman@1";

int GMTOffset = 19800;  // Replace with your GMT Offset in seconds
int daylightOffset = 0;  // Replace with your daylight savings offset in seconds

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an Arduino UNO:       A4(SDA), A5(SCL)
// On an Arduino MEGA 2560: 20(SDA), 21(SCL)
// On an Arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUZZER_PIN 19 // Buzzer connected to pin 19
#define TOUCH_PIN 4 // Touch pin 4 on ESP32
#define LDR_PIN 0 // ldr connected to pin 0

// LEDC channel and timer settings
#define LEDC_CHANNEL_0 0
#define LEDC_TIMER_13_BIT 13
#define LEDC_BASE_FREQ 5000

// Reference state
const int ref_eye_height = 40;
const int ref_eye_width = 40;
const int ref_space_between_eye = 10;
const int ref_corner_radius = 10;

// Current state of the eyes
int left_eye_height = ref_eye_height;
int left_eye_width = ref_eye_width;
int left_eye_x = 32;
int left_eye_y = 32;
int right_eye_x = 32 + ref_eye_width + ref_space_between_eye;
int right_eye_y = 32;
int right_eye_height = ref_eye_height;
int right_eye_width = ref_eye_width;

// Function Prototypes
void draw_eyes(bool update = true);
void center_eyes(bool update = true);
void blink(int speed = 12);
void sleepEyes();
void wakeupEyes();
void happy_eye();
void angry_eye();
void sad_eye();
void surprised_eye();
void sleepy_eye();
void winking_eye(bool update = true);
void blink_slowly();
void rapid_eye_movement();
void move_right_big_eye();
void move_left_big_eye();

// Sound Functions
void playHappySound();
void playAngrySound();
void playSadSound();
void playSurprisedSound();
void playSleepySound();
void playWinkingSound();
void playRapidMovementSound();
void playWakeupSound();

// Function to initialize LEDC for PWM
void setupLEDC() {
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(BUZZER_PIN, LEDC_CHANNEL_0);
}

// Define time intervals for each emotion (in milliseconds)
const unsigned long intervalHappy = 2000;
const unsigned long intervalAngry = 2000;
const unsigned long intervalSad = 2000;
const unsigned long intervalSurprised = 2000;
const unsigned long intervalSleepy = 2000;
const unsigned long intervalBlinkSlowly = 2000;
const unsigned long intervalRapidEyeMovement = 2000;
const unsigned long intervalwinkingeye = 2000;

unsigned long previousMillis = 0;
int emotionState = 0;

void setup() {
  // Initialize the display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Initialize Serial for debugging
  Serial.begin(115200);

  // Clear the buffer
  display.clearDisplay();

  // Initial display message
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.println(F("Intellar.ca"));
  display.display();
  delay(300);
  
  // Initialize buzzer pin
  setupLEDC(); // Initialize LEDC for PWM
  // Initialize LDR pin
  pinMode(LDR_PIN,INPUT);
  
  sleepEyes();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  
  Serial.println("Connected to Wi-Fi!");
  
  configTime(GMTOffset, daylightOffset, "pool.ntp.org","time.nist.gov");
}

void loop() {
  // Check if touch pin is touched
  int LDRreading = digitalRead(LDR_PIN);
  Serial.println(LDRreading);
  if (LDRreading == 0) { // Adjust threshold as necessary
    sleepEyes();
  }else{
    ///////////////////////////////////////////////////////////////////////////
    
    time_t rawtime = time(nullptr);
    struct tm* timeinfo = localtime(&rawtime);
    
    int hour = timeinfo->tm_hour;
    String period = "AM";
    
    if (hour == 0) {
      hour = 12;  // Midnight case
    } else if (hour >= 12) {
      period = "PM";
      if (hour > 12) {
        hour -= 12;  // Convert to 12-hour format
      }
    }

    Serial.print("Time: ");
    Serial.print(hour);
    Serial.print(":");
    Serial.print(timeinfo->tm_min);
    Serial.print(":");
    Serial.print(timeinfo->tm_sec);
    Serial.print(" ");
    Serial.println(period);
    
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(0, 25);
    display.print(hour);
    display.print(":");
    if (timeinfo->tm_min < 10)
      display.print("0");
    display.print(timeinfo->tm_min);
    
    display.setTextSize(2);
    display.setCursor(90, 30);
    display.print(":");
    if (timeinfo->tm_sec < 10)
      display.print("0");
    display.print(timeinfo->tm_sec);
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print(period);
    display.display();
    
    delay(3000); 
////////////////////////////////////////////////////////////
  int reading = touchRead(TOUCH_PIN);
//  Serial.print("reading");
//  Serial.println(reading);
  if (reading < 30) { // Adjust threshold as necessary
    happy_eye();
    playHappySound();
  }
  else{
    unsigned long currentMillis = millis();

  // Check if it's time to change the emotion
  if (currentMillis - previousMillis >= getCurrentInterval()) {
    previousMillis = currentMillis; // Save the last time an emotion was changed
    emotionState = (emotionState + 1) % 8; // Cycle through 8 emotions
    
    // Update the current emotion
    switch (emotionState) {
      case 0:
        wakeupEyes();
        blink_slowly();
        happy_eye();
        blink_slowly();
        center_eyes(true);
        blink(10);
        move_right_big_eye();
        break;
      case 1:
        blink(10);
        surprised_eye();
        blink(10);
        center_eyes(true);
        blink(20);
        winking_eye(true);
        center_eyes(true);
        blink(10);
        
        break;
      case 2:
        move_left_big_eye();
        blink(20);
        winking_eye(false);
        center_eyes(true);
        blink(20);
        blink_slowly();
        sad_eye();
        break;
      case 3:
        center_eyes(true);
        blink(10);
        angry_eye();
        rapid_eye_movement();
        blink_slowly();
        break;
      case 4:
        move_right_big_eye();
        blink(20);
        center_eyes(true);
        blink(10);
        winking_eye(false);
        move_left_big_eye();
        blink(10);
        blink(20);
        center_eyes(true);
        sleepy_eye();
        break;
      case 5:
        move_left_big_eye();
        winking_eye(true);
        blink_slowly();
        break;
      case 6:
        move_left_big_eye();
        blink(10);
        blink(20);
        move_right_big_eye();
        blink(20);
        center_eyes(true);
        blink(10);
        blink_slowly();
        break;
      case 7:
        sleepy_eye();
        sleepEyes();
        break;  
    }
  }
}
}
}
//  sleepEyes();
//  delay(2000);
//  
//  wakeupEyes();
//  delay(2000);
//  
//  center_eyes(true);
//  delay(2000);
//  
//  blink(10);
//  delay(2000);
//  
//  move_right_big_eye();
//  delay(2000);
//  
//  move_left_big_eye();
//  delay(2000);
//  
//  blink(10);
//  delay(2000);
//  
//  winking_eye(true);  // Winks left eye
//  delay(2000);
//  winking_eye(false); // Winks right eye
//  delay(2000);
//
//  blink(20);
//  delay(2000);
//  
//  happy_eye();
//  delay(2000);
//  
//  angry_eye();
//  delay(2000);
//  
//  sad_eye();
//  delay(2000);
//  
//  surprised_eye();
//  delay(2000);
//  
//  sleepy_eye();
//  delay(2000);
//  
//  winking_eye(true);  // Winks left eye
//  delay(2000);
//  
//  blink_slowly();
//  delay(2000);
//  
//  rapid_eye_movement();
//  delay(2000);
//  
//  sleepEyes();
//  delay(2000);
//}

// Function Definitions

// Function to get the current interval based on the emotion state
unsigned long getCurrentInterval() {
  switch (emotionState) {
    case 0: return intervalHappy;
    case 1: return intervalAngry;
    case 2: return intervalSad;
    case 3: return intervalSurprised;
    case 4: return intervalSleepy;
    case 5: return intervalBlinkSlowly;
    case 6: return intervalRapidEyeMovement;
    case 7: return intervalwinkingeye;
    default: return 2000; // Default to 2 seconds if something goes wrong
  }
}

// Sound Functions
void playHappySound() {
  ledcWriteTone(LEDC_CHANNEL_0, 1000); // Set frequency to 1000 Hz
  delay(200);
  ledcWriteTone(LEDC_CHANNEL_0, 0); // Turn off sound
}

void playAngrySound() {
  ledcWriteTone(LEDC_CHANNEL_0, 400);
  delay(500);
  ledcWriteTone(LEDC_CHANNEL_0, 0);
}

void playSadSound() {
  ledcWriteTone(LEDC_CHANNEL_0, 200);
  delay(500);
  ledcWriteTone(LEDC_CHANNEL_0, 0);
}

void playSurprisedSound() {
  ledcWriteTone(LEDC_CHANNEL_0, 1200);
  delay(300);
  ledcWriteTone(LEDC_CHANNEL_0, 0);
}

void playSleepySound() {
  ledcWriteTone(LEDC_CHANNEL_0, 300);
  delay(500);
  ledcWriteTone(LEDC_CHANNEL_0, 0);
}

void playWinkingSound() {
  ledcWriteTone(LEDC_CHANNEL_0, 800);
  delay(150);
  ledcWriteTone(LEDC_CHANNEL_0, 0);
}

void playRapidMovementSound() {
  for (int i = 0; i < 3; i++) {
    ledcWriteTone(LEDC_CHANNEL_0, 1000);
    delay(100);
    ledcWriteTone(LEDC_CHANNEL_0, 500);
    delay(100);
  }
  ledcWriteTone(LEDC_CHANNEL_0, 0);
}

void playWakeupSound() {
  ledcWriteTone(LEDC_CHANNEL_0, 1000);
  delay(300);
  ledcWriteTone(LEDC_CHANNEL_0, 0);
}
//void playHappySound() {
//  tone(BUZZER_PIN, 1000, 200);
//}
//
//void playAngrySound() {
//  tone(BUZZER_PIN, 400, 500);
//}
//
//void playSadSound() {
//  tone(BUZZER_PIN, 200, 500);
//}
//
//void playSurprisedSound() {
//  tone(BUZZER_PIN, 1200, 300);
//}
//
//void playSleepySound() {
//  tone(BUZZER_PIN, 300, 500);
//}
//
//void playWinkingSound() {
//  tone(BUZZER_PIN, 800, 150);
//}
//
//void playRapidMovementSound() {
//  for (int i = 0; i < 3; i++) {
//    tone(BUZZER_PIN, 1000, 100);
//    delay(150);
//    tone(BUZZER_PIN, 500, 100);
//    delay(150);
//  }
//}
//
//void playWakeupSound() {
//  tone(BUZZER_PIN, 1000, 300);
//}


// Draw both eyes based on current state
void draw_eyes(bool update) {
  display.clearDisplay();        
  
  // Draw left eye
  int x = left_eye_x - left_eye_width / 2;
  int y = left_eye_y - left_eye_height / 2;
  display.fillRoundRect(x, y, left_eye_width, left_eye_height, ref_corner_radius, SSD1306_WHITE);
  
  // Draw right eye
  x = right_eye_x - right_eye_width / 2;
  y = right_eye_y - right_eye_height / 2;
  display.fillRoundRect(x, y, right_eye_width, right_eye_height, ref_corner_radius, SSD1306_WHITE);    
  
  if(update) {
    display.display();
  }
}

// Center the eyes on the display
void center_eyes(bool update) {
  left_eye_height = ref_eye_height;
  left_eye_width = ref_eye_width;
  right_eye_height = ref_eye_height;
  right_eye_width = ref_eye_width;
  
  left_eye_x = SCREEN_WIDTH / 2 - ref_eye_width / 2 - ref_space_between_eye / 2;
  left_eye_y = SCREEN_HEIGHT / 2;
  right_eye_x = SCREEN_WIDTH / 2 + ref_eye_width / 2 + ref_space_between_eye / 2;
  right_eye_y = SCREEN_HEIGHT / 2;
  
  draw_eyes(update);
}

// Blink function: closes and opens both eyes
void blink(int speed) {
  draw_eyes();
  
  // Close eyes
  for(int i = 0; i < 3; i++) {
    left_eye_height -= speed;
    right_eye_height -= speed;    
    draw_eyes();
    delay(100);
  }
  
  // Open eyes
  for(int i = 0; i < 3; i++) {
    left_eye_height += speed;
    right_eye_height += speed;
    draw_eyes();
    delay(100);
  }
}

// Sleep eyes: eyes appear closed or nearly closed
void sleepEyes() {
  left_eye_height = 2;
  right_eye_height = 2;
  draw_eyes(true);  
}

// Wakeup eyes: eyes open gradually
void wakeupEyes() {
  sleepEyes();
  for(int h = 0; h <= ref_eye_height; h += 2) {
    left_eye_height = h;
    right_eye_height = h;
    draw_eyes(true);
    delay(50);
  }
  playWakeupSound();
}

// Happy eye: adding smile or bright eyes
//void happy_eye() {
//  center_eyes(false);
//  // Draw smiling mouth or other happy features if desired
//  // For simplicity, we'll just display the happy eyes
//  draw_eyes(true);
//  delay(1000);
//}
void happy_eye()
{
  center_eyes(false);
  //draw inverted triangle over eye lower part
  int offset = ref_eye_height/2;
  for(int i=0;i<10;i++)
  {
    display.fillTriangle(left_eye_x-left_eye_width/2-1, left_eye_y+offset, left_eye_x+left_eye_width/2+1, left_eye_y+5+offset, left_eye_x-left_eye_width/2-1,left_eye_y+left_eye_height+offset,SSD1306_BLACK);
    //display.fillRect(left_eye_x-left_eye_width/2-1, left_eye_y+5, left_eye_width+1, 20,SSD1306_BLACK);

    display.fillTriangle(right_eye_x+right_eye_width/2+1, right_eye_y+offset, right_eye_x-left_eye_width/2-1, right_eye_y+5+offset, right_eye_x+right_eye_width/2+1,right_eye_y+right_eye_height+offset,SSD1306_BLACK);
    //display.fillRect(right_eye_x-right_eye_width/2-1, right_eye_y+5, right_eye_width+1, 20,SSD1306_BLACK);  
    offset -= 2;
    display.display();
    delay(1);
  }
  
  playHappySound();
  display.display();
  delay(1000);
}

// Angry eye: draw eyebrows or adjust eyes to look angry
//void angry_eye() {
//  center_eyes(false);
//  
//  // Draw angry eyebrows using lines
//  // Left eyebrow
//  display.drawLine(left_eye_x - left_eye_width / 2, left_eye_y - left_eye_height / 2 - 5,
//                 left_eye_x + left_eye_width / 2, left_eye_y - left_eye_height / 2 - 5, SSD1306_WHITE);
//  
//  // Right eyebrow
//  display.drawLine(right_eye_x - right_eye_width / 2, right_eye_y - right_eye_height / 2 - 5,
//                 right_eye_x + right_eye_width / 2, right_eye_y - right_eye_height / 2 - 5, SSD1306_WHITE);
//  
//  display.display();
//  delay(1000);
//}

// Sad eye: draw frowning mouth or adjust eyes to look sad
//void sad_eye() {
//  center_eyes(false);
//  
//  // Draw sad lines below the eyes to represent frowning
//  // Left eye
//  display.drawLine(left_eye_x - left_eye_width / 2, left_eye_y + left_eye_height / 2,
//                 left_eye_x, left_eye_y + left_eye_height / 2 + 10, SSD1306_WHITE);
//  display.drawLine(left_eye_x, left_eye_y + left_eye_height / 2 + 10,
//                 left_eye_x + left_eye_width / 2, left_eye_y + left_eye_height / 2, SSD1306_WHITE);
//  
//  // Right eye
//  display.drawLine(right_eye_x - right_eye_width / 2, right_eye_y + right_eye_height / 2,
//                 right_eye_x, right_eye_y + right_eye_height / 2 + 10, SSD1306_WHITE);
//  display.drawLine(right_eye_x, right_eye_y + right_eye_height / 2 + 10,
//                 right_eye_x + right_eye_width / 2, right_eye_y + right_eye_height / 2, SSD1306_WHITE);
//  
//  display.display();
//  delay(1000);
//}
void angry_eye() {
  center_eyes(false);
  int offset = ref_eye_height / 2;
  
  for (int i = 0; i < 10; i++) {
    display.fillTriangle(left_eye_x - left_eye_width / 2 - 1, left_eye_y - offset, left_eye_x + left_eye_width / 2 + 1, left_eye_y - 5 - offset, left_eye_x - left_eye_width / 2 - 1, left_eye_y - left_eye_height - offset, SSD1306_BLACK);
    display.fillTriangle(right_eye_x + right_eye_width / 2 + 1, right_eye_y - offset, right_eye_x - left_eye_width / 2 - 1, right_eye_y - 5 - offset, right_eye_x + right_eye_width / 2 + 1, right_eye_y - right_eye_height - offset, SSD1306_BLACK);
    offset -= 2;
    display.display();
    delay(1);
  }
  playAngrySound();
  display.display();
  delay(1000);
}

void sad_eye() {
  center_eyes(false);
  int y_offset = ref_eye_height / 2;

  // Draw sad lines below the eyes
  display.drawLine(left_eye_x - left_eye_width / 2, left_eye_y + y_offset, left_eye_x, left_eye_y + y_offset + 10, SSD1306_BLACK);
  display.drawLine(left_eye_x, left_eye_y + y_offset + 10, left_eye_x + left_eye_width / 2, left_eye_y + y_offset, SSD1306_BLACK);
  
  display.drawLine(right_eye_x - right_eye_width / 2, right_eye_y + y_offset, right_eye_x, right_eye_y + y_offset + 10, SSD1306_BLACK);
  display.drawLine(right_eye_x, right_eye_y + y_offset + 10, right_eye_x + right_eye_width / 2, right_eye_y + y_offset, SSD1306_BLACK);
  playSadSound();
  display.display();
  delay(1000);
}

// Surprised eye: make eyes bigger and rounder
void surprised_eye() {
  
  left_eye_height = ref_eye_height + 10;
  left_eye_width = ref_eye_width + 10;
  right_eye_height = ref_eye_height + 10;
  right_eye_width = ref_eye_width + 10;
  
  playSurprisedSound();
  draw_eyes();
  delay(1000);
}

// Sleepy eye: draw half-closed eyes
void sleepy_eye() {
  playSleepySound();
  center_eyes(false);
  int y_offset = ref_eye_height / 2;
  
  // Draw half-closed left eye
  display.fillRect(left_eye_x - left_eye_width / 2, left_eye_y + y_offset, left_eye_width, left_eye_height / 2, SSD1306_BLACK);
  
  // Draw half-closed right eye
  display.fillRect(right_eye_x - right_eye_width / 2, right_eye_y + y_offset, right_eye_width, right_eye_height / 2, SSD1306_BLACK);
  
  display.display();
  delay(1000);
}

// Winking eye: close one eye briefly
//void winking_eye() {
//  center_eyes(false);
//  draw_eyes(true);
//  delay(500);
//  
//  // Close left eye
//  left_eye_height = 2;
//  draw_eyes(true);
//  delay(500);
//  
//  // Open left eye
//  left_eye_height = ref_eye_height;
//  draw_eyes(true);
//  delay(500);
//  
//  // Optionally, you can alternate which eye winks
//}
void winking_eye(bool update) {
  playWinkingSound();
  center_eyes(false);
  draw_eyes(true);
  delay(500);
  
  if(update) {
    // Close left eye
    left_eye_height = 2;
  } else {
    // Close right eye
    right_eye_height = 2;
  }
  
  draw_eyes(true);
  delay(500);
  
  if(update) {
    left_eye_height = ref_eye_height;
  } else {
    right_eye_height = ref_eye_height;
  }
  
  draw_eyes(true);
  delay(500);
}

// Blink Slowly: simulate tired blinking
void blink_slowly() {
  for(int i = 0; i < 3; i++) {
    blink(5); // Slower blink with smaller speed decrement
    delay(1000);
  }
}

// Rapid Eye Movement: simulate excitement by moving eyes quickly
void rapid_eye_movement() {
  
  // Define movement amplitude
  int move_amplitude_x = 5;
  int move_amplitude_y = 3;
  // Number of rapid movements
  int movements = 10;
  
  playRapidMovementSound();
  for(int i = 0; i < movements; i++) {
    // Move eyes to the right
    left_eye_x += move_amplitude_x;
    right_eye_x += move_amplitude_x;
    draw_eyes(true);
    delay(50);
    
    // Move eyes to the left
    left_eye_x -= 2 * move_amplitude_x;
    right_eye_x -= 2 * move_amplitude_x;
    draw_eyes(true);
    delay(50);
    
    // Move eyes back to the center
    left_eye_x += move_amplitude_x;
    right_eye_x += move_amplitude_x;
    draw_eyes(true);
    delay(50);
    
    // Optional: Move eyes up and down
    left_eye_y += move_amplitude_y;
    right_eye_y += move_amplitude_y;
    draw_eyes(true);
    delay(50);
    
    left_eye_y -= 2 * move_amplitude_y;
    right_eye_y -= 2 * move_amplitude_y;
    draw_eyes(true);
    delay(50);
    
    left_eye_y += move_amplitude_y;
    right_eye_y += move_amplitude_y;
    draw_eyes(true);
    delay(50);
  }
  
  // Return eyes to original position
  center_eyes(true);
  delay(500);
}

// Move eyes to the right with a big eye animation
void move_right_big_eye() {
  move_big_eye(1);
}

// Move eyes to the left with a big eye animation
void move_left_big_eye() {
  move_big_eye(-1);
}

// Function to move eyes in a direction
void move_big_eye(int direction) {
  // direction == -1 : move left
  // direction == 1 : move right

  int direction_oversize = 1;
  int direction_movement_amplitude = 2;
  int blink_amplitude = 5;

  for(int i = 0; i < 3; i++) {
    left_eye_x += direction_movement_amplitude * direction;
    right_eye_x += direction_movement_amplitude * direction;    
    right_eye_height -= blink_amplitude;
    left_eye_height -= blink_amplitude;
    
    if(direction > 0) {
      right_eye_height += direction_oversize;
      right_eye_width += direction_oversize;
    } else {
      left_eye_height += direction_oversize;
      left_eye_width += direction_oversize;
    }

    draw_eyes();
    delay(50);
  }
  
  for(int i = 0; i < 3; i++) {
    left_eye_x += direction_movement_amplitude * direction;
    right_eye_x += direction_movement_amplitude * direction;
    right_eye_height += blink_amplitude;
    left_eye_height += blink_amplitude;
    
    if(direction > 0) {
      right_eye_height += direction_oversize;
      right_eye_width += direction_oversize;
    } else {
      left_eye_height += direction_oversize;
      left_eye_width += direction_oversize;
    }
    draw_eyes();
    delay(50);
  }

  delay(1000);

  for(int i = 0; i < 3; i++) {
    left_eye_x -= direction_movement_amplitude * direction;
    right_eye_x -= direction_movement_amplitude * direction;    
    right_eye_height -= blink_amplitude;
    left_eye_height -= blink_amplitude;
    
    if(direction > 0) {
      right_eye_height -= direction_oversize;
      right_eye_width -= direction_oversize;
    } else {
      left_eye_height -= direction_oversize;
      left_eye_width -= direction_oversize;
    }
    draw_eyes();
    delay(50);
  }
  
  for(int i = 0; i < 3; i++) {
    left_eye_x -= direction_movement_amplitude * direction;
    right_eye_x -= direction_movement_amplitude * direction;    
    right_eye_height += blink_amplitude;
    left_eye_height += blink_amplitude;
    
    if(direction > 0) {
      right_eye_height -= direction_oversize;
      right_eye_width -= direction_oversize;
    } else {
      left_eye_height -= direction_oversize;
      left_eye_width -= direction_oversize;
    }
    draw_eyes();
    delay(50);
  }

  center_eyes();
}
