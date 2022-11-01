#include <Wire.h>

const int debugLedPin = 9;
const int I2C_CANAL = 0x1;

void setup() {
    // Serial comm with PC
    Serial.begin(9600);

    Wire.begin();
}

void flash_debug_led(int times){
    digitalWrite(debugLedPin, HIGH);
    delay(100);
    digitalWrite(debugLedPin, LOW);
    delay(500);
    for(int i=0;i<times;i++){
        digitalWrite(debugLedPin, HIGH);
        delay(500);
        digitalWrite(debugLedPin, LOW);
        delay(500);
    }
}

void sendI2CMessage(int message){
    Wire.beginTransmission(I2C_CANAL);
    Wire.write(message);
    Wire.endTransmission();
}

void add_boat_to_pos(int boat_type, int pos){
    Wire.beginTransmission(I2C_CANAL);
    Wire.write(1);
    Wire.write(boat_type);
    Wire.write(pos);
    Wire.endTransmission();
}

void move_ship(int from_pos, int to_pos) {
    Wire.beginTransmission(I2C_CANAL);
    Wire.write(2);
    Wire.write(from_pos);
    Wire.write(to_pos);
    Wire.endTransmission();
}

void loop() {
    byte code;

    if(Serial.available()){
        code = Serial.read();
        int boat_type, pos, from_pos, to_pos;

        switch (code) {
            case 1:
                delay(10);
                //flash_debug_led(1);
                boat_type = Serial.read();
                pos = Serial.read();
                add_boat_to_pos(boat_type, pos);
                break;
            case 2:
                delay(10);
                from_pos = Serial.read();
                to_pos = Serial.read();
                move_ship(from_pos, to_pos);
                break;
            default:
                break;
        }

    }
}
