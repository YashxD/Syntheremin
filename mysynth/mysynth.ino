// Arduino synth library absolute frequency example

//Hardware cHIGHnectiHIGHs:

//                    +10�F
//PIN 11 ---[ 1k ]--+---||--->> Audio out
//                  |
//                 === 10nF
//                  |
//                 GND

#include <StandardCplusplus.h>
#include <synth.h>
// IMPORT POWER LIBRARY
#include <vector>
#define trig 13
#define echo 12
const int pitchpin = A0; // Pitch Pot (analog)
const int tonelengthpin = A1; // Tone Length Pot (analog)
const int modpin = A2; // Mod pot (analog)
const int sequenceinterval = A3; // Sequence Interval Pot (analog)
// Waveform switches
const int switch1 = 2; // sine
const int switch2 = 3; // triangle
const int switch3 = 4; // square 
const int switch4 = 5; // saw
const int switch5 = 6; // ramp 
const int switch6 = 7; // noise
//pinMode(switch1, INPUT);
//pinMode(switch2, INPUT);
//pinMode(switch3, INPUT);
//pinMode(switch4, INPUT);
//pinMode(switch5, INPUT);
//pinMode(switch6, INPUT);
//Envelopes
const int envelopeswitch = 8; // 4-way envelope switch (or potentially another pot)
const int LEDPin = 9;
int onsize = 0; // length of the vector of switch modes (HIGH OR LOW)
synth edgar;    //-Make a synth

void update() {
  // use 6 arduino toggle switches to choose SINE,TRIANGLE,SQUARE,SAW,RAMP,NOISE
  // map values of int, pitch, tonelength of AnalogRead from 0 - 1023 to 0 - 127? divide them by 8.06 and round 
  int pitch = round(analogRead(pitchpin)/8.06);
//  Serial.println(pitch);
  int tonelength = round(analogRead(tonelengthpin)/8.06);
  int mod = round(analogRead(modpin)/8.06);
//  Serial.println(tonelength);
  //setupVoice( voice[0-3] , waveform[SINE,TRIANGLE,SQUARE,SAW,RAMP,NOISE] , pitch[0-127], envelope[ENVELOPE0-ENVELOPE3], length[0-127], mod[0-127, 64=no mod])

  int switch1state = digitalRead(switch1);
  int switch2state = digitalRead(switch2);
  int switch3state = digitalRead(switch3);
  int switch4state = digitalRead(switch4);
  int switch5state = digitalRead(switch5);
  int switch6state = digitalRead(switch6);
  Serial.println(digitalRead(switch6));
  int envelope = analogRead(envelopeswitch); // not sure what the output of the 4-way switch will be. if cant figure out, use a pot for envelope and threshold it. For now assuming it will output 1,2,3,or 4
  int envel = ENVELOPE0; // set default envelope
  switch (envelope) {
    case 1:
      envel = ENVELOPE0;
    case 2:
      envel = ENVELOPE1;
    case 3:
      envel = ENVELOPE2;
    case 4:
      envel = ENVELOPE3;
  }

  int switchstates[] = {switch1state, switch2state, switch3state, switch4state, switch5state, switch6state};
  std::vector< int > onswitches;
  for (int i = 0; i < 6; i++) {
//    Serial.println(i);
    Serial.println(switchstates[i]); // check that at least one of them is "HIGH"
    if (switchstates[i] == 1) {
      onswitches.push_back(i);
    }
  }
  Serial.println("-------------------------");
  onsize = sizeof(onswitches) / sizeof(int);
  for (int i = 0; i < onsize + 1; i++) {
//    Serial.println(onswitches[1]);
    edgar.setupVoice(i, onswitches[i], pitch, envel, tonelength, mod); // second parameter: 0,1,2,3,4,5 = SINE,TRIANGLE,SQUARE,SAW,RAMP,NOISE
  }
}
void setup() {
  edgar.begin(); //-Start it up, with audio output from pin 11
  update();
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, HIGH);
  Serial.begin(115200);
  // ADD CALL TO POWER LIBRARY FOR TIMEOUT DURATION (MAKE IT SOMETHING LIKE 3 HRS)
}
void loop()
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  char duration = pulseIn(echo, HIGH);
  int distance = microsecondsToCentimeters(duration);
  // tone(8, distance, 40); pin 8 is the piezo out
  int sequenceaddition = analogRead(sequenceinterval);
//  Serial.println(onsize); // check that it isn't zero. if it is, onsize didn't get updated when update() was called in setup(). 
  for (int i = 0; i < onsize + 1; i++) {
    edgar.setFrequency(i, distance + (sequenceaddition * i));
    edgar.trigger(i);
  }
  //  if (millis() % 1000 == 0) {
//      update(); // update switch configurations to see if they've been changed, every second
//    }
  update();
   
  // ADD CALL TO THE POWER LIBRARY TO CHECK IF TIMEOUT
}
long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}
