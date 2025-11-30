#!/bin/bash
set -e

echo "Installing Arduino CLI..."
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
sudo mv bin/arduino-cli /usr/local/bin/
rm -rf bin

echo "Initializing Arduino CLI..."
arduino-cli config init
arduino-cli config set board_manager.additional_urls https://espressif.github.io/arduino-esp32/package_esp32_index.json

echo "Updating core index..."
arduino-cli core update-index

echo "Installing ESP32 core v3.x.x"
arduino-cli core install esp32:esp32@3.0.7

echo "Installing build dependencies..."
sudo apt update
sudo apt install -y python3 python3-pip git unzip

echo "Installing build dependencies..."
sudo apt update
sudo apt install -y python3 python3-pip git unzip

# BARIS KRITIS YANG HILANG:
echo "Installing Python serial module (pyserial)..."
pip install pyserial

echo "Environment ready."
