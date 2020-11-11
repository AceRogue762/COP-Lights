/**
 * Audio Extension
 * 
 * Uses an analog microphone to sample audio. Audio data is used to 
 * calculate an 8-bit (8 bands) Fourier transform. Upon request, the 
 * transform is updated and streamed continuously to the main LED
 * controller. 
 */

#include <arduinoFFT.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <WiFi.h>

const int MIC_PIN = 35;

const int BLOCK_SIZE = 512;

const uint8_t amplitude = 1000;

double vReal[BLOCK_SIZE];
double vImag[BLOCK_SIZE];
int32_t samples[BLOCK_SIZE];

String labels[] = {"125", "250", "500", "1K", "2K", "4K", "8K", "16K"};

arduinoFFT FFT = arduinoFFT();

int bands[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// This device's broadcasted ESPNow address
uint8_t broadcastAddress[] = {0x2F, 0x0E, 0xAC, 0x2B, 0x3C, 0x8F};

// The LED controller's ESPNow address
uint8_t controllerAddress[] = {0xAC, 0x67, 0xB2, 0x2A, 0x7A, 0x84};

// Global copy of controller connection and pair status
bool controllerPaired = false;
esp_now_peer_info_t controller;

// Wrapper for a single message sent to the controller
typedef struct message {
  int FFTBands[8]; // Frequency band values (0 - 12)
} message;

message currentFFT;

// Configure WiFi station
void configWiFi() {
  WiFi.mode(WIFI_STA);

  // Set custom MAC address
  esp_wifi_set_mac(ESP_IF_WIFI_STA, &broadcastAddress[0]);
}

// Initialize ESPNow with fallback
void initESPNow() {
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow initialized");
  } else {
    Serial.println("ESPNow init failed. Retrying.");
    ESP.restart();
  }

  // Register send and receive callbacks
  esp_now_register_recv_cb(onDataReceived);
  esp_now_register_send_cb(onDataSent);

  // Pair with the controller
  controllerPaired = pairWithController();
}

// Connect and register the controller
bool pairWithController() {
  Serial.println("Attempting to pair with the controller...");
  
  // Check if the peer exists
  bool exists = esp_now_is_peer_exist(controller.peer_addr);
  
  if (exists) {
    // Already paired.
    Serial.println("Already paired");
    return true;
  } else {
    // Not paired. Attempt pairing now.
    memcpy(controller.peer_addr, controllerAddress, 6);
    controller.channel = 0;  
    controller.encrypt = false;
    
    esp_err_t addStatus = esp_now_add_peer(&controller);
    
    if (addStatus == ESP_OK) {
      // Pair success
      Serial.println("Pair successful");
      return true;
    } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
      // How did we get so far!!
      Serial.println("ESPNOW not initialized!");
      return false;
    } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
      Serial.println("Invalid argument");
      return false;
    } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
      Serial.println("Peer list full");
      return false;
    } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
      Serial.println("Out of memory");
      return false;
    } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
      Serial.println("Already paired");
      return true;
    } else {
      Serial.println("Unknown error!");
      return false;
    }
  }
}

// ESPNow data received callback
void onDataReceived(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  Serial.println(*data);
}

// ESPNow data send callback
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  pinMode(MIC_PIN, INPUT);

  configWiFi();
  initESPNow();
}

void loop() {
  // Read multiple samples at once and calculate the sound pressure
  for (uint16_t i = 0; i < BLOCK_SIZE; i++) {
    vReal[i] = analogRead(MIC_PIN);
    vImag[i] = 0.0;
  }

  FFT.Windowing(vReal, BLOCK_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, BLOCK_SIZE, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, BLOCK_SIZE);
  
  for (int i = 0; i <= 8; i++) {
    bands[i] = 0;
  }

  // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a 
  // frequency and its value the amplitude.
  for (int i = 2; i < (BLOCK_SIZE/2); i++){
    // Add a crude noise filter
    if (vReal[i] > amplitude && vReal[i] < 1000) {
      // Seperate the frequency bands
      if (i<=2            ) bands[0] = max(bands[0], (int)(vReal[i]) / amplitude * 3); // 125Hz
      if (i >3  && i<=5   ) bands[1] = max(bands[1], (int)(vReal[i]) / amplitude * 3); // 250Hz
      if (i >5  && i<=7   ) bands[2] = max(bands[2], (int)(vReal[i]) / amplitude * 3); // 500Hz
      if (i >7  && i<=15  ) bands[3] = max(bands[3], (int)(vReal[i]) / amplitude * 3); // 1000Hz
      if (i >15 && i<=30  ) bands[4] = max(bands[4], (int)(vReal[i]) / amplitude * 3); // 2000Hz
      if (i >30 && i<=53  ) bands[5] = max(bands[5], (int)(vReal[i]) / amplitude * 3); // 4000Hz
      if (i >53 && i<=200 ) bands[6] = max(bands[6], (int)(vReal[i]) / amplitude * 3); // 8000Hz
      if (i >200          ) bands[7] = max(bands[7], (int)(vReal[i]) / amplitude * 3); // 16000Hz
    }
  }

  // Send the result to the controller
  memcpy(currentFFT.FFTBands, bands, 8*sizeof(int));
  esp_err_t result = esp_now_send(controllerAddress, (uint8_t *) &currentFFT, sizeof(currentFFT));

  /*
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }*/

  for (int i = 0; i < 8; i++) {
    Serial.println(bands[i]);
  }

  delay(20);
}
