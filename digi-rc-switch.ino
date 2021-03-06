#include <RCSwitch.h>
#include <DigiUSB.h>

RCSwitch mySwitch = RCSwitch();
byte in = 0;
byte byte1 = 0;
byte byte2 = 0;
byte byte3 = 0;
byte byte4 = 0;
int next = 0;
long code = 0;
long last_time = 0;

// The debug print code interferes with the stability
// of the usb thing, the attiny can't print out reliably.
#define DEBUG 0

// The rc-switch library requires a long int, but we can only
// read data out of the usb 1 bytes at a time, so we must
// compose our own long to send to rc-switch
long composeLong(byte one, byte two, byte three, byte four) {
  return (long) four << 24 | (long) three << 16 | (long) two << 8 | (long) one;
}

void setup() {
  DigiUSB.begin();
  mySwitch.enableTransmit(1);
  mySwitch.setPulseLength(192);
  last_time = millis();
}

void loop() {
  DigiUSB.refresh();
  if (DigiUSB.available() > 0) {
    if (last_time <= millis() - 1000) {
#ifdef DEBUG
      DigiUSB.println("Start");
#endif
      next = 1;
      last_time = millis();
    }

    in = DigiUSB.read();
    if (next == 1) {
      byte1 = in;
#ifdef DEBUG
      DigiUSB.print(F("byte1 "));
      DigiUSB.println(in, HEX);
#endif
      next = 2;
    }
    else if (next == 2) {
      byte2 = in;
#ifdef DEBUG
      DigiUSB.print(F("byte2 "));
      DigiUSB.println(in, HEX);
#endif
      next = 3;
    }
    else if (next == 3) {
      byte3 = in;
#ifdef DEBUG
      DigiUSB.print(F("byte3 "));
      DigiUSB.println(in, HEX);
#endif
      DigiUSB.print("byte3 ");
      DigiUSB.println(in, HEX);
      next = 4;
    }
    else if (next == 4) {
      byte4 = in;
#ifdef DEBUG
      DigiUSB.print(F("byte4 "));
      DigiUSB.println(in, HEX);
#endif
      code = composeLong(byte1, byte2, byte3, byte4);
#ifdef DEBUG
      DigiUSB.print(F("Sending Code:"));
      DigiUSB.print(code);
      DigiUSB.print(F(" (hex: "));
      DigiUSB.print(code, HEX);
      DigiUSB.print(F(")..."));
#endif
      DigiUSB.refresh();
      mySwitch.send(code, 24);
      DigiUSB.refresh();
#ifdef DEBUG
      DigiUSB.println(F("Done"));
      DigiUSB.println("");
#endif
      next = 0;
#ifdef DEBUG
    } else {
      DigiUSB.print(F("Else Got:"));
      DigiUSB.println(in, HEX);
#endif
    }
  }
}
