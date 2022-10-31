#include <Wire.h>

const int CHANNEL_LEN = 10;
const int debugLedPin = 9;

typedef enum direction_t {
    RIGHT,
    LEFT
} direction_t;

typedef enum ship_type_t {
    NO_SHIP,
    NORMAL,
    PESQUERO,
    PATRULLA
} ship_type_t;

typedef struct iface_state_t{
    ship_type_t channel_representation[CHANNEL_LEN];
    uint8_t ships_at_left;
    uint8_t ships_at_right;
    direction_t channel_direction;
} iface_state_t;

const int I2C_canal_1 = 0x1;
const int I2C_canal_2 = 0x2;
const int I2C_contadores = 0x3;

iface_state_t *state;
String state_message;


void setup() {
    // Initializing essential variables
    state = malloc(sizeof(iface_state_t));

    // Serial comm with PC
    Serial.begin(9600);
    pinMode(debugLedPin, OUTPUT);

    Wire.begin();
}

void send_confirmation() {
    char* confirm_message = "ok";
    Serial.write(confirm_message);
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

void read_serial_struct() {
    int len = sizeof(iface_state_t);
    char *struct_buffer = malloc(len);

    // Vamos a leer un string de la forma
    // 1 2 3 0 0 0 0 0 0 0\n
    // 00 99\n
    // 0\0

    state_message = Serial.readString();
    flash_debug_led(1);

    //memcpy(state, struct_buffer, len);
//    size_t len = 72;//sizeof(iface_state_t);
//    char *struct_buffer = malloc(len);
//    int i = 0;
//    delay(100);
//
//    while(Serial.available() != len){
//        delay(10);
//    }
//
//    flash_debug_led(1);
//
//    while(Serial.available()){
//        struct_buffer[i] = Serial.read();
//        i++;
//    }
//
//
//    memcpy(state, struct_buffer, len);
//
//
//    free(struct_buffer);
}

void sendI2CMessage(int target_device, void* message, int length){
    flash_debug_led(3);
    char msg_buffer[516];

    state_message.toCharArray(msg_buffer, state_message.length());

    Wire.beginTransmission(target_device);
    Wire.write(msg_buffer);
//    for(int i = 0; i < length; i++) {
//        Wire.write(msg_buffer[i]);
//    }
    Wire.endTransmission();
    flash_debug_led(3);
}

void send_control_messages_to_slaves() {
    sendI2CMessage(
            I2C_canal_1,
            state->channel_representation,
            sizeof(ship_type_t) * CHANNEL_LEN);
}

void loop() {
    byte brightness;

    if(Serial.available()){
        brightness = Serial.read();
        if(brightness == 170) {
            send_confirmation();
            read_serial_struct();
            send_control_messages_to_slaves();
            //analogWrite(ledPin, state->ledState);
        }
    }
}
