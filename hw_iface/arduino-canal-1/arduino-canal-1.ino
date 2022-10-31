#include <Wire.h>

const int CHANNEL_LEN = 10;

typedef enum ship_type_t {
    NO_SHIP,
    NORMAL,
    PESQUERO,
    PATRULLA
} ship_type_t;

const int ledN = 5;

int rPins[ledN] = {2,5,8,11,14};
int gPins[ledN] = {3,6,9,12,15};
int bPins[ledN] = {4,7,10,13,16};

void process_I2C_incoming_message(int bytes_to_read){
    Serial.println("Receiving message!");
    Serial.print(Wire.available());
    Serial.println(" bytes to read");
    Serial.println();
    char * msg_buffer = malloc(516);
    int i = 0;
    while(Wire.available()){
        msg_buffer[i] = Wire.read();
        Serial.print(msg_buffer[i] - 0);
        Serial.print(" ");
        i++;
        if ((i+1)%16==0)
            Serial.println("");
    }

    Serial.println("Finished receiving message");
    Serial.println(msg_buffer);

    ship_type_t *ship_arr = (ship_type_t *)msg_buffer;
    for(i = 0;i<ledN;i++){
        turn_on_led_n(i, ship_arr[i]);
    }
    Serial.println("Finished parsing message");

}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Wire.begin(0x1);

    Wire.onReceive(process_I2C_incoming_message);

    for(int i = 0;i < ledN;i++){
        pinMode(rPins[i], OUTPUT);
        pinMode(gPins[i], OUTPUT);
        pinMode(bPins[i], OUTPUT);
    }

    Serial.println("Setup complete!");
}

void turn_on_led_n(int n, ship_type_t shipType){
    Serial.println("Trying to turn on led " + String(n) + " of type: ");
    Serial.print(shipType);
    switch (shipType) {
        case NO_SHIP:
            digitalWrite(rPins[n], LOW);
            digitalWrite(gPins[n], LOW);
            digitalWrite(bPins[n], LOW);
            break;
        case NORMAL:
            digitalWrite(rPins[n], LOW);
            digitalWrite(gPins[n], HIGH);
            digitalWrite(bPins[n], LOW);
            break;
        case PESQUERO:
            digitalWrite(rPins[n], LOW);
            digitalWrite(gPins[n], LOW);
            digitalWrite(bPins[n], HIGH);
            break;
        case PATRULLA:
            digitalWrite(rPins[n], HIGH);
            digitalWrite(gPins[n], LOW);
            digitalWrite(bPins[n], LOW);
            break;
    }
}

void loop() {
    // put your main code here, to run repeatedly:

}
