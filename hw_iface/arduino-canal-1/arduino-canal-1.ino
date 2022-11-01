#include <Wire.h>

const int CHANNEL_LEN = 10;

typedef enum ship_type_t {
    NO_SHIP,
    NORMAL,
    PESQUERO,
    PATRULLA
} ship_type_t;

ship_type_t canal_arr[5] = {NO_SHIP, NO_SHIP, NO_SHIP, NO_SHIP, NO_SHIP};

const int ledN = 5;

int rPins[ledN] = {2,5,8,11,14};
int gPins[ledN] = {3,6,9,12,15};
int bPins[ledN] = {4,7,10,13,16};

void add_boat_to_pos(ship_type_t boat_type, int pos){
    canal_arr[pos] = boat_type;
    turn_on_led_n(pos, boat_type);
}

void flush_i2c(){
    Serial.println("Flushing I2C channel");
    Serial.print("  Flushing ");
    Serial.print(Wire.available());
    Serial.println(" bytes");
    while(Wire.available()){
        Wire.read();
    }
}
void move_pos(int from_pos, int to_pos) {
    Serial.print("Moving ship of type ");
    Serial.print(canal_arr[from_pos]);
    Serial.print(" to pos: ");
    Serial.print(to_pos);

    canal_arr[to_pos] = canal_arr[from_pos];

    turn_on_led_n(from_pos, NO_SHIP);
    turn_on_led_n(to_pos, canal_arr[to_pos]);

}

void process_I2C_incoming_message(int bytes_to_read){
    Serial.println("Receiving message!");
    Serial.print(Wire.available());
    Serial.println(" bytes to read");

    int code = Wire.read();
    ship_type_t boat_type;
    int pos, from_pos, to_pos;


    switch (code) {
        case 1:
            Serial.println("Executing code 1");
            boat_type = Wire.read();
            pos = Wire.read();
            add_boat_to_pos(boat_type, pos);
            break;
        case 2:
            Serial.println("Executing code 2");
            from_pos = Wire.read();
            to_pos = Wire.read();
            move_pos(from_pos, to_pos);
        default:
            delay(10);
            break;
    }
    flush_i2c();

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
    Serial.println("");
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
