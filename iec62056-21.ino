unsigned long last;

#define INPUTPIN A0
#define THRESHOLD 10

#define FASTADC 1

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void setup() {
  Serial.begin (115200);
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println ("Starting ...");
  pinMode (INPUTPIN, INPUT);
  last = micros();
 
#if FASTADC
  // set prescale to 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
#endif
}

void getSync() {
  // find a low level lasting at least 150ms
  unsigned long sync = 0;
  do {
    if (analogRead(INPUTPIN) < THRESHOLD) sync = sync + 1;
    else sync = 0;
  } while (sync < 10000);
}

void getRisingEdge() {
  // wait for rising edge
  do {
    delayMicroseconds (3);
  } while (analogRead(INPUTPIN) < THRESHOLD);
  delayMicroseconds (104/2); // wait half bit duration
}

unsigned char getByte() {
  // sample 7 bits at 9600 baud / 104.17 us
  unsigned char c = 0;
  unsigned char power = 1;
  for (int b = 0; b < 7; ++b) {
    delayMicroseconds (90);
    if (analogRead(INPUTPIN) > THRESHOLD) c = c + power;
    power = 2 * power;
  }

  delayMicroseconds (104 + 104/2); // skip parity bit
  return c;
}

void loop() {
  // put your main code here, to run repeatedly
  getSync();
  unsigned char c;
  do {
    getRisingEdge();
    c = getByte();
    if (c != 0) {
      c = 127 - c;

      Serial.print((char) c);
/*
      Serial.print("(");
      if (c < 16) Serial.print("0");
      Serial.print(c, HEX);
      Serial.print (") ");
*/
    }
  } while (c != 0);
}
