#include <Servo.h>

const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int PIR_PIN = 2;
const int SERVO_PIN = 3;
const int BUZZER_PIN = 5;

Servo gateServo;

enum GateState { CLOSED, OPENING, OPEN, CLOSING };
GateState currentState = CLOSED;
unsigned long openTimer = 0;
const unsigned long HOLD_OPEN_DURATION = 5000; // 5 seconds

float getDistanceCM() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    if (duration == 0) return 999.0;
    return (duration * 0.0343) / 2.0;
}

void setup() {
    Serial.begin(9600);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(PIR_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    gateServo.attach(SERVO_PIN);
    gateServo.write(0); // 0 degrees = Closed
}

void loop() {
    float distance = getDistanceCM();
    int motion = digitalRead(PIR_PIN);

    switch(currentState) {
        case CLOSED:
            if (distance < 25.0 || motion == HIGH) {
                tone(BUZZER_PIN, 1000, 200);
                gateServo.write(90); // Open
                currentState = OPEN;
                openTimer = millis();
            }
            break;
        case OPEN:
            if (millis() - openTimer > HOLD_OPEN_DURATION) {
                if (distance > 30.0) { // Safety clearance check
                    gateServo.write(0); // Close
                    currentState = CLOSED;
                } else {
                    openTimer = millis(); // Reset timer if obstacle present
                }
            }
            break;
        default:
            break;
    }
    delay(100);
}
