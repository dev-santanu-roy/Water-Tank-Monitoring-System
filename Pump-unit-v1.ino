#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <WiFi.h>
#include <esp_now.h>
#include <NewPing.h>
#define WATER_TRIGGER_PIN 33 // Pin connected to ultrasonic sensor trigger
#define WATER_ECHO_PIN 32 // Pin connected to ultrasonic sensor echo
#define MAX_DISTANCE 20
// Maximum distance for ultrasonic sensor (cm)
#define TANK_HEIGHT 20
// Height of the tank (cm)
#define FULL_TANK_HEIGHT 15 // Height at which the tank is considered full (cm)
Adafruit_BMP280 bmp; // I2C interface for BMP280 sensor
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();
NewPing sonar(WATER_TRIGGER_PIN, WATER_ECHO_PIN, MAX_DISTANCE);
// MAC address of the receiver ESP32
uint8_t receiverMac[] = {0xbc, 0xdd, 0xc2, 0x96, 0x64, 0x0c}; // Example MAC, replace with
actual address
typedef struct struct_message {
float temperature;
float pressure; // Now in hPa
int waterlevel;
} struct_message;
struct_message sensorData;
// Calibration parameters
float distanceOffset = 0.0; // Default offset to correct distance measurement
float distanceScaling = 1.0; // Default scaling factor to adjust measurement
// Variable to track last send status
bool lastSendSuccess = false;
// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
Serial.print("Last Packet Send Status: ");
if (status == ESP_NOW_SEND_SUCCESS) {
Serial.println("Delivery Success");
lastSendSuccess = true;
} else {Serial.println("Delivery Fail");
lastSendSuccess = false;
}
}
void setup() {
Serial.begin(115200);
// Initialize BMP280 sensor using the original Adafruit library
if (!bmp.begin(0x76)) {
Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
while (1);
}
// Set up ESP-NOW
WiFi.mode(WIFI_STA);
if (esp_now_init() != ESP_OK) {
Serial.println("Error initializing ESP-NOW");
return;
}
// Register the send callback function
esp_now_register_send_cb(OnDataSent);
// Add peer
esp_now_peer_info_t peerInfo;
memcpy(peerInfo.peer_addr, receiverMac, 6);
peerInfo.channel = 0;
peerInfo.encrypt = false;
if (esp_now_add_peer(&peerInfo) != ESP_OK) {
Serial.println("Failed to add peer");
return;
}
// Set BMP280 sensor settings
bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, /* Operating Mode. */
Adafruit_BMP280::SAMPLING_X2, /* Temp. oversampling */
Adafruit_BMP280::SAMPLING_X16, /* Pressure oversampling */
Adafruit_BMP280::FILTER_X16,
/* Filtering. */
Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
Serial.println("Tank Unit ready and starting measurements...");
Serial.print("Default Calibration - Offset: ");
Serial.print(distanceOffset);
Serial.print(", Scaling: ");
Serial.println(distanceScaling);
}
int readWaterLevel() {
unsigned int distance = sonar.ping_cm(); // Get distance in cmif (distance == 0 || distance > MAX_DISTANCE) {
Serial.println("Error: Distance measurement out of range or failure.");
return 0; // No valid level detected
}
// Apply calibration
distance = distance * distanceScaling + distanceOffset;
// Calculate water height in the tank
float waterHeight = TANK_HEIGHT - distance;
// Clamp the water height to be within the range of 0 to TANK_HEIGHT
if (waterHeight > TANK_HEIGHT) {
waterHeight = TANK_HEIGHT;
} else if (waterHeight < 0) {
waterHeight = 0;
}
// Calculate water level percentage based on FULL_TANK_HEIGHT
int waterLevelPercent = (int)((waterHeight / FULL_TANK_HEIGHT) * 100);
// Ensure the percentage does not exceed 100%
if (waterLevelPercent > 100) {
waterLevelPercent = 100;
}
return waterLevelPercent;
}
// Function to set calibration parameters
void setCalibration(float offset, float scaling) {
distanceOffset = offset;
distanceScaling = scaling;
Serial.print("Calibration set - Offset: ");
Serial.print(distanceOffset);
Serial.print(", Scaling: ");
Serial.println(distanceScaling);
}
// Function to check the last send status
void checkSendStatus() {
if (lastSendSuccess) {
Serial.println("Last data sent successfully.");
} else {
Serial.println("Failed to send last data.");
}
}
void loop() {
sensors_event_t temp_event, pressure_event;
bmp_temp->getEvent(&temp_event);
// Get temperature event
bmp_pressure->getEvent(&pressure_event); // Get pressure event// Read water level (in cm) from ultrasonic sensor
int waterLevel = readWaterLevel();
sensorData.waterlevel = waterLevel;
// Update sensor data
sensorData.temperature = temp_event.temperature; // Temperature in Celsius
sensorData.pressure = pressure_event.pressure; // Pressure in hPa
// Send data to receiver ESP32
esp_now_send(receiverMac, (uint8_t *)&sensorData, sizeof(sensorData));
// Debugging output
Serial.print("Water Level: ");
Serial.print(sensorData.waterlevel);
Serial.print(" %, Temperature: ");
Serial.print(sensorData.temperature);
Serial.print(" °C, Pressure: ");
Serial.print(sensorData.pressure);
Serial.println(" hPa");
// Check the last send status
checkSendStatus();
delay(2000); // Send data every 2 seconds
}