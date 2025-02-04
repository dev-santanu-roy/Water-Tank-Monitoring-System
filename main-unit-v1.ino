#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Define button pins
#define UP_BUTTON 12
#define DOWN_BUTTON 14
#define OK_BUTTON 13
#define MENU_BUTTON 15
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Structure to receive data from Tank Unit
typedef struct struct_message {
float temperature; // Temperature in Celsius
float pressure; // Pressure in hPa
int waterlevel; // Water level percentage
} struct_message;
struct_message receivedData; // Variable to store received data
// Structure to send data to Pump Unit
typedef struct pump_message {
bool pumpState; // true = ON, false = OFF
} pump_message;
pump_message pumpCmd; // Variable to store pump command
// Menu control variables
bool inMenu = false; // Flag to track if we're in the menu
int menuOption = 0; // Tracks the current menu option (0 = Pump Start, 1 = Pump Stop)
int startLevel = 20; // Variable for the pump start level (in percentage)
int stopLevel = 100; // Variable for the pump stop level (in percentage)
// Debounce variables
#define DEBOUNCE_DELAY 30 // Debounce time in milliseconds, slightly increased for
reliability
unsigned long lastDebounceTime[4] = {0, 0, 0, 0}; // To store the last time the button state was
changed
int lastButtonState[4] = {LOW, LOW, LOW, LOW}; // To store the last stable state of the buttons
int currentButtonState[4] = {LOW, LOW, LOW, LOW}; // To store the current reading of the
buttons
int buttonPins[4] = {UP_BUTTON, DOWN_BUTTON, OK_BUTTON, MENU_BUTTON};// Function to handle the menu
void handleMenu() {
lcd.clear();
lcd.setCursor(0, 0);
if (inMenu) {
if (menuOption == 0) {
lcd.print("Set Pump Start: ");
lcd.setCursor(0, 1);
lcd.print(startLevel);
lcd.print("%");
} else if (menuOption == 1) {
lcd.print("Set Pump Stop: ");
lcd.setCursor(0, 1);
lcd.print(stopLevel);
lcd.print("%");
}
} else {
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Waiting for data");
}
}
// Debounce function for buttons with LOW default state
bool debounceButton(int buttonIndex) {
int reading = digitalRead(buttonPins[buttonIndex]);
// Check if the button state has changed
if (reading != currentButtonState[buttonIndex]) {
lastDebounceTime[buttonIndex] = millis(); // Reset the debounce timer
}
// If the state has been stable for longer than the debounce delay, consider it stable
if ((millis() - lastDebounceTime[buttonIndex]) > DEBOUNCE_DELAY) {
// If the button state has changed to HIGH (button press), register it
if (reading != lastButtonState[buttonIndex]) {
lastButtonState[buttonIndex] = reading;
if (lastButtonState[buttonIndex] == HIGH) { // Button press detected
return true;
}
}
}
currentButtonState[buttonIndex] = reading;
return false; // Button is not pressed
}
// Check button presses with debounce
void checkButtons() {
if (debounceButton(0)) { // UP buttonif (inMenu) {
if (menuOption == 0 && startLevel < 100) startLevel +=20; // Increment start level by 1
if (menuOption == 1 && stopLevel < 100) stopLevel +=20; // Increment stop level by 1
handleMenu();
}
}
if (debounceButton(1)) { // DOWN button
if (inMenu) {
if (menuOption == 0 && startLevel > 0) startLevel -=20;
if (menuOption == 1 && stopLevel > 0) stopLevel -=20;
handleMenu();
}
}
// Decrement start level by 1
// Decrement stop level by 1
if (debounceButton(2)) { // OK button
if (inMenu) {
menuOption = (menuOption + 1) % 2; // Toggle between pump start/stop options
handleMenu();
}
}
if (debounceButton(3)) { // MENU button
inMenu = !inMenu; // Toggle menu mode on/off
handleMenu();
}
}
// Callback function for data reception via ESP-NOW
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
Serial.print("Bytes received: ");
Serial.println(len);
memcpy(&receivedData, incomingData, sizeof(receivedData));
// Test print received data
Serial.println("Received data:");
Serial.print("Water Level: ");
Serial.println(receivedData.waterlevel);
Serial.print("Temperature: ");
Serial.println(receivedData.temperature);
Serial.print("Pressure: ");
Serial.println(receivedData.pressure);
if (!inMenu) {
lcd.setCursor(0, 0); // Set cursor to the first row
lcd.print("Water: ");
lcd.print(receivedData.waterlevel);
lcd.print("%
");
lcd.setCursor(0, 1); // Set cursor to the second row
lcd.print("Temp: ");lcd.print(receivedData.temperature);
lcd.print("C ");
}
}
// Pump control variables
bool pumpRunning = false; // Track pump state (false = off, true = on)
unsigned long previousMillis = 0;
const long interval = 500; // Interval to send ON command (in milliseconds), here set to 5 seconds
void controlPump() {
int hysteresis = 0;
int adjustedWaterLevel = receivedData.waterlevel + hysteresis;
unsigned long currentMillis = millis(); // Get the current time
// If water level is low, and pump should turn on
if (adjustedWaterLevel <= startLevel) {
// Send the ON command if pump is not running, or at regular intervals if it's already running
if (!pumpRunning || (currentMillis - previousMillis >= interval)) {
sendPumpCommand(true); // Send pump ON command
pumpRunning = true;
previousMillis = currentMillis; // Reset the timer
Serial.println("Pump ON command sent");
}
}
// If water level is high, and pump should turn off
else if (adjustedWaterLevel >= stopLevel && pumpRunning) {
sendPumpCommand(false); // Send pump OFF command
pumpRunning = false;
Serial.println("Pump OFF command sent");
}
else if (pumpRunning) {
Serial.println("Pump running, waiting for water level to reach stop level");
sendPumpCommand(true); // Send pump ON command
} else {
Serial.println("Pump off, waiting for water level to reach start level");
}
}
// Function to send pump control command via ESP-NOW
void sendPumpCommand(bool pumpState) {
pumpCmd.pumpState = pumpState; // true = ON, false = OFF
uint8_t broadcastAddress[] = {0xE8, 0x9F, 0x6D, 0x8F, 0xFB, 0x0E}; // Replace with Pump Unit Mac adds
MAC address
esp_now_send(broadcastAddress, (uint8_t *) &pumpCmd, sizeof(pumpCmd));
}
void setup() {
Serial.begin(115200);
// Initialize buttons as INPUTpinMode(UP_BUTTON, INPUT);
pinMode(DOWN_BUTTON, INPUT);
pinMode(OK_BUTTON, INPUT);
pinMode(MENU_BUTTON, INPUT);
// Initialize display
lcd.init();
lcd.backlight();
lcd.setCursor(0, 0);
lcd.print("Waiting for data");
// Init ESP-NOW
WiFi.mode(WIFI_STA);
if (esp_now_init() != 0) {
Serial.println("Error initializing ESP-NOW");
return;
}
esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
esp_now_register_recv_cb(OnDataRecv);
}
void loop() {
checkButtons(); // Check for button presses
// Control the pump when not in the menu
if (!inMenu) {
controlPump();
}
delay(20); // Delay to prevent overwhelming the serial monitor
}