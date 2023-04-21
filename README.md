# Inventory-Management-RFID

RFID Tag Counter with MQTT and ThingsBoard:

This is an Arduino sketch that uses the MFRC522 RFID module to count the number of times each tag is detected and publish the data to the ThingsBoard IoT platform via MQTT.

Dependencies:

This sketch depends on the following libraries:

SPI (included with Arduino IDE)

MFRC522 (available from https://github.com/miguelbalboa/rfid)

WiFi (included with Arduino IDE)

PubSubClient (available from https://github.com/knolleary/pubsubclient)


Installation:

Install the above libraries if you haven't already.

Download and extract the sketch files.

Open the sketch file (RFID_Tag_Counter.ino) in the Arduino IDE.

Modify the following lines to match your WiFi and MQTT configuration:

Upload the sketch to your Arduino board.

Connect the MFRC522 module to your Arduino board according to the pin layout specified in the sketch file.

Power up the Arduino board and the MFRC522 module.

Open the serial monitor in the Arduino IDE to view the output.

Usage:

When a new RFID tag is detected, the sketch will print its UID and the number of times it has been detected since the last different tag was detected. The counts for each tag are published to ThingsBoard via MQTT every time a new tag is detected.

License:

This sketch is released under the MIT License. See LICENSE for more information.
