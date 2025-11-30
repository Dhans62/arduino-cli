#!/bin/bash
set -e
# ... (Instalasi Arduino CLI, init, update-index) ...

echo "Installing build dependencies..."
sudo apt update
sudo apt install -y python3 python3-pip git unzip

# A. INSTAL PYSERIAL DULU (WAJIB!)
echo "Installing Python serial module (pyserial)..."
pip install pyserial

# B. BARU INSTAL CORE ESP32 (Sekarang akan berhasil)
echo "Installing ESP32 core v3.x.x"
arduino-cli core install esp32:esp32@3.0.7 

echo "Environment ready."
