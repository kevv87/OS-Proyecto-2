
const int ledPin = 9;

void setup(){
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
}

void send_confirmation(){
    char* confirm_message = "ok";
    Serial.write(confirm_message);
}

void loop(){
    byte brightness;

    if(Serial.available()){
        brightness = Serial.read();
        if(brightness == 0){
            send_confirmation();
        }
        analogWrite(ledPin, brightness);
    }
}