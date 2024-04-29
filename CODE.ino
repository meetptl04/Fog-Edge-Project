#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

// Initialize SoftwareSerial for Bluetooth communication
SoftwareSerial BTserial(0, 1); // RX | TX (HC-05 Bluetooth Module)

// Initialize LiquidCrystal for LCD display
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // LCD Pins: (RS, E, D4, D5, D6, D7)

// Variable to store received Bluetooth command
String receivedCommand;

// Flags to indicate room enable/disable status
bool room1Enabled = false;
bool room2Enabled = false;

// Define pin numbers for sensors, LEDs, and visitor count
#define in 8  // In Sensor (Visitor Entering)
#define out 9 // Out Sensor (Visitor Exiting)
#define led1Pin 13
#define led2Pin 10
#define led3Pin 6
#define led4Pin 7

// Initialize visitor count
int count = 0;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  
  // Initialize LCD display
  lcd.begin(16, 2);
  lcd.print("Visitor Counter");
  delay(2000);
  
  // Set pin modes for sensors, LEDs
  pinMode(in, INPUT); // IR sensor in 
  pinMode(out, INPUT); // IR sensor out
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(led4Pin, OUTPUT);
  
  // Clear LCD display and print initial visitor count
  lcd.clear();
  lcd.print("Person In Room:");
  lcd.setCursor(0, 1);
  lcd.print(count);
  
  // Start Bluetooth communication
  BTserial.begin(9600);
}

void loop() {
  // Read sensor values
  int in_value = digitalRead(in);
  int out_value = digitalRead(out);

  // Check for available Bluetooth commands if visitors are present
  if (count > 0) {
    if (BTserial.available()) {
      // Read and process the command
      receivedCommand = BTserial.readStringUntil('\n');
      processCommand(receivedCommand);
    }
  }

  // Increment visitor count when someone enters
  if (in_value == LOW) {
    count++;
    lcd.clear();
    lcd.print("Person In Room:");
    Serial.println(count); // Print count to Serial for debugging
    lcd.setCursor(0, 1);
    lcd.print(count);
    delay(1000);
  }

  // Decrement visitor count when someone exits
  if (out_value == LOW) {
    if (count > 0) {
      count--;
    } else {
      count = 0;
    }
    lcd.clear();
    lcd.print("Person In Room:");
    lcd.setCursor(0, 1);
    lcd.print(count);
    delay(1000);
  }

  // If no visitors, turn off all LEDs
  if (count <= 0) {
    count = 0;
    lcd.clear();
    lcd.print("Nobody In Room");
    lcd.setCursor(0, 1);
    lcd.print("Light is Off");
    delay(200);
    lcd.print("All off");
    digitalWrite(led1Pin, LOW);
    digitalWrite(led2Pin, LOW);
    digitalWrite(led3Pin, LOW);
    digitalWrite(led4Pin, LOW);
  }
}

// Function to process received Bluetooth commands
void processCommand(String command) {
  command.trim(); // Remove leading and trailing whitespace

  // Check if the command is to enable Room 1
  if (command.equals("room 1")) {
    // Enable Room 1 and disable Room 2
    room1Enabled = true;
    room2Enabled = false;
    lcd.println("Room 1 enabled");
    Serial.println("Room 1 enabled"); // Print status to Serial for debugging
  }
  // Check if the command is to enable Room 2
  else if (command.equals("room 2")) {
    // Enable Room 2 and disable Room 1
    room1Enabled = false;
    room2Enabled = true;
    lcd.println("Room 2 enabled");
    Serial.println("Room 2 enabled"); // Print status to Serial for debugging
  }

  // Check if Room 1 is enabled
  else if (room1Enabled) {
    // Control LEDs in Room 1 based on command
    if (command.equals("turn LED 1 on")) {
      digitalWrite(led1Pin, HIGH);
      lcd.println("LED1 in Room1 is ON");
    } else if (command.equals("turn LED 1 off")) {
      digitalWrite(led1Pin, LOW);
      lcd.println("LED1 in Room1 is OFF");
    } else if (command.equals("turn LED 2 on")) {
      digitalWrite(led2Pin, HIGH);
      lcd.println("LED2 in Room1 is ON");
    } else if (command.equals("turn LED 2 off")) {
      digitalWrite(led2Pin, LOW);
      lcd.println("LED2 in Room1 is OFF");
    }
  }

  // Check if Room 2 is enabled
  else if (room2Enabled) {
    // Control LEDs in Room 2 based on command
    if (command.equals("turn LED 1 on")) {
      digitalWrite(led3Pin, HIGH);
      lcd.println("LED1 in Room2 is ON");
    } else if (command.equals("turn LED 1 off")) {
      digitalWrite(led3Pin, LOW);
      lcd.println("LED1 in Room2 is OFF");
    } else if (command.equals("turn LED 2 on")) {
      digitalWrite(led4Pin, HIGH);
      lcd.println("LED2 in Room2 is ON");
    } else if (command.equals("turn LED 2 off")) {
      digitalWrite(led4Pin, LOW);
      lcd.println("LED2 in Room2 is OFF");
    }
  }
  // Check for commands to turn on or off all LEDs
  else if (command.equals("turn on all")) {
    digitalWrite(led1Pin, HIGH);
    digitalWrite(led2Pin, HIGH);
    digitalWrite(led3Pin, HIGH);
    digitalWrite(led4Pin, HIGH);
    lcd.print("All ON");
  } else if (command.equals("turn off all")) {
    digitalWrite(led1Pin, LOW);
    digitalWrite(led2Pin, LOW);
    digitalWrite(led3Pin, LOW);
    digitalWrite(led4Pin, LOW);
    lcd.print("All OFF");
  }
}
