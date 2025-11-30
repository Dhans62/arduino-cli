#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// =========================
// CONFIG
// =========================
const char* ssid = "ESP32_OTA";
const char* password = "12345678";

WebServer server(80);

// BLE UUID
#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

bool wifiMode = false;   // false = BLE aktif, true = WiFi aktif

// =====================================================
// OTA PAGE
// =====================================================
const char* otaPage =
"<html><body><h2>ESP32 OTA Update</h2>"
"<form method='POST' action='/update' enctype='multipart/form-data'>"
"<input type='file' name='update'>"
"<input type='submit' value='Upload'>"
"</form><br>"
"<a href='/ble'>Switch to BLE</a>"
"</body></html>";


// =====================================================
// HANDLE BLE COMMANDS
// =====================================================
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) {
    std::string cmd = pChar->getValue();

    if (cmd == "wifi_on") {
      wifiMode = true;
    } 
  }
};


// =====================================================
// START BLE
// =====================================================
void startBLE() {
  BLEDevice::init("ESP32_DUAL");
  pServer = BLEDevice::createServer();

  BLEService *service = pServer->createService(SERVICE_UUID);
  pCharacteristic = service->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_WRITE
  );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->addDescriptor(new BLE2902());
  service->start();

  BLEAdvertising *advertising = pServer->getAdvertising();
  advertising->start();

  Serial.println("BLE ON");
}


// =====================================================
// STOP BLE
// =====================================================
void stopBLE() {
  BLEDevice::deinit(true);
  Serial.println("BLE OFF");
}


// =====================================================
// START WIFI + SERVER
// =====================================================
void startWiFi() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  server.on("/", []() {
    server.send(200, "text/html", otaPage);
  });

  server.on("/ble", []() {
    wifiMode = false;  // Trigger balik ke BLE
    server.send(200, "text/plain", "Switching to BLE...");
  });

  server.on("/update", HTTP_POST, []() {
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    delay(500);
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Update.begin();
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      Update.write(upload.buf, upload.currentSize);
    } else if (upload.status == UPLOAD_FILE_END) {
      Update.end(true);
    }
  });

  server.begin();
  Serial.println("WiFi ON, AP Ready");
}


// =====================================================
// STOP WIFI
// =====================================================
void stopWiFi() {
  server.stop();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("WiFi OFF");
}


// =====================================================
// SETUP
// =====================================================
void setup() {
  Serial.begin(115200);
  startBLE();
}


// =====================================================
// LOOP
// =====================================================
void loop() {

  if (wifiMode) {
    stopBLE();
    startWiFi();
    wifiMode = false;  // Hanya pindah satu kali
  }

  if (WiFi.getMode() == WIFI_AP) {
    server.handleClient();
  }
}
