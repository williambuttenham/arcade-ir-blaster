#include <Servo.h>
#include <IRremote.h>

Servo joystickServo;
IRsend irsend;

const int upPin = 7;
const int downPin = 8;
const int servoPin = 9;
// const int irLedPin = 3;

int upState = 0;
int downState = 0;
int pos = 0;

void setup()
{
    joystickServo.attach(servoPin);
    pinMode(upPin, INPUT_PULLUP);
    pinMode(downPin, INPUT_PULLUP);
    irsend.sendSony(0xa90, 12); //power
    delay(40);
    irsend.sendSony(0xa90, 12); //input
    delay(40);
}

void loop()
{
    upState = digitalRead(upPin);
    downState = digitalRead(downPin);

    if (upState == HIGH && downState == HIGH)
    {
        joystickChange();
        delay(1000);
    }
    else if (upState == HIGH)
    {
        irVolUp();
    }
    else if (downState == HIGH)
    {
        irVolDown();
    }
}

void joystickChange()
{
    if (pos == 0)
    {
        joystickServo.write(180);
        delay(100);
    }
    else
    {
        joystickServo.write(0);
        delay(100);
    }
}

void irVolUp()
{
    irsend.sendSony(0xa90, 12);
    delay(40);
}

void irVolDown()
{
    irsend.sendSony(0xa90, 12);
    delay(40);
}
