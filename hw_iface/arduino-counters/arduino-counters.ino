#include <Wire.h>

typedef enum display1_led_t {
        A_1=2,
        B_1,
        C_1,
        D_1,
        E_1,
        F_1,
        G_1,
        EMPTY_1=22
} display1_led_t;

typedef enum display2_led_t {
    A_2=9,
    B_2,
    C_2,
    D_2,
    E_2,
    F_2,
    G_2,
    EMPTY_2=22
} display2_led_t;

const display1_led_t display1_leds[7] = {
        A_1,B_1,C_1,D_1,E_1,F_1,G_1
};
const display2_led_t display2_leds[7] = {
        A_2,B_2,C_2,D_2,E_2,F_2,G_2
};

typedef enum display_number_t {
    LEFT,
    RIGHT
} display_number_t;

const int NUMBERS_IN_DIGIT = 10;
const int N_SEGMENTS = 7;

const display1_led_t number_display_1
        [NUMBERS_IN_DIGIT][N_SEGMENTS] = {
        {A_1, B_1,  C_1,  D_1,      E_1,      F_1,      EMPTY_1}, // 0
        {B_1, C_1,  EMPTY_1,  EMPTY_1,  EMPTY_1,  EMPTY_1,  EMPTY_1}, // 1
        {A_1, B_1,  G_1,      D_1,      E_1,      EMPTY_1,  EMPTY_1}, // 2
        {A_1, B_1,  G_1,      C_1,      D_1,      EMPTY_1,  EMPTY_1}, // 3
        {B_1, C_1,  G_1,      F_1,      EMPTY_1,  EMPTY_1,  EMPTY_1}, // 4
        {A_1, F_1,  G_1,      C_1,      D_1,      EMPTY_1,  EMPTY_1}, // 5
        {A_1, C_1,  D_1,      E_1,      F_1,      G_1,      EMPTY_1}, // 6
        {A_1, B_1,  C_1,      G_1,      EMPTY_1,  EMPTY_1,  EMPTY_1}, // 7
        {A_1, B_1,  C_1,      D_1,      E_1,      F_1,      G_1}, // 8
        {A_1, B_1,  C_1,      F_1,      G_1,      EMPTY_1,  EMPTY_1}, // 9
};

const display2_led_t number_display_2
[NUMBERS_IN_DIGIT][N_SEGMENTS] = {
        {A_2, B_2,  C_2,  D_2,      E_2,      F_2,      EMPTY_2}, // 0
        {B_2, C_2,  EMPTY_2,  EMPTY_2,  EMPTY_2,  EMPTY_2,  EMPTY_2}, // 1
        {A_2, B_2,  G_2,      D_2,      E_2,      EMPTY_2,  EMPTY_2}, // 2
        {A_2, B_2,  G_2,      C_2,      D_2,      EMPTY_2,  EMPTY_2}, // 3
        {B_2, C_2,  G_2,      F_2,      EMPTY_2,  EMPTY_2,  EMPTY_2}, // 4
        {A_2, F_2,  G_2,      C_2,      D_2,      EMPTY_2,  EMPTY_2}, // 5
        {A_2, C_2,  D_2,      E_2,      F_2,      G_2,      EMPTY_2}, // 6
        {A_2, B_2,  C_2,      G_2,      EMPTY_2,  EMPTY_2,  EMPTY_2}, // 7
        {A_2, B_2,  C_2,      D_2,      E_2,      F_2,      G_2}, // 8
        {A_2, B_2,  C_2,      F_2,      G_2,      EMPTY_2,  EMPTY_2}, // 9
};

void setup(){
    Serial.begin(115200);
    Wire.begin(0x2);
    Wire.onReceive(process_I2C_incoming_message);

    for(int i = 0;i<7;i++){
        pinMode(display1_leds[i], OUTPUT);
        pinMode(display2_leds[i], OUTPUT);
    }
}

void flush_digits(){
    for(int j = 0;j < NUMBERS_IN_DIGIT; j ++){
        for(int i = 0;i<N_SEGMENTS;i++){
            digitalWrite(number_display_1[j][i], LOW);
            digitalWrite(number_display_2[j][i], LOW);
        }
    }
}


void draw_digit(int number, display_number_t display){
    if (display == RIGHT){
        for(int i = 0;i<N_SEGMENTS;i++){
            digitalWrite(number_display_1[number][i], HIGH);
        }
    }else{
        for(int i = 0;i<N_SEGMENTS;i++){
            digitalWrite(number_display_2[number][i], HIGH);
        }
    }
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

void process_I2C_incoming_message(int bytes_to_read){
    Serial.print("Receiving message!");
    Serial.print(Wire.available());
    Serial.println(" bytes to read");

    int code = Wire.read();
    int quantity;

    switch (code) {
        case 3:
            Serial.println("Executing code 3");
            quantity = Wire.read();
            draw_digit(quantity, LEFT);
            break;
        case 4:
            Serial.println("Executing code 4");
            quantity = Wire.read();
            draw_digit(quantity, RIGHT);
            break;
        default:
            delay(10);
            break;
    }
    flush_i2c();

}

void loop(){

}
