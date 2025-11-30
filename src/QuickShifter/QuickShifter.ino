void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.print("ESP32 Arduino Core Version: ");
  Serial.println(ESP.getSdkVersion());
}

void loop() {}
