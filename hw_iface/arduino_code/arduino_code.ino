typedef struct hw_state_t{
    // Sizes of types are different in Arduino! Be careful
    uint8_t ledState;
} hw_state_t;

const int ledPin = 9;

hw_state_t *state;

void setup() {
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);

    state = malloc(sizeof(hw_state_t));
}

void send_confirmation() {
    char* confirm_message = "ok";
    Serial.write(confirm_message);
}

void read_serial_struct() {
    size_t len = sizeof(hw_state_t);
    char *struct_buffer = malloc(len);
    int i = 0;

    while (i < len) {
        if(Serial.available()){
            struct_buffer[i] = Serial.read();
            i++;
        }
    }

    //state = (hw_state_t *) struct_buffer;
    memcpy(state, struct_buffer, len);

    free(struct_buffer);
}

void loop() {
    byte brightness;

    if(Serial.available()){
        brightness = Serial.read();
        if(brightness == 3) {
            send_confirmation();
            read_serial_struct();
            analogWrite(ledPin, state->ledState);
        }else{
            analogWrite(ledPin, brightness);
        }
    }
}