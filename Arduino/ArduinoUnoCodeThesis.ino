#include <AltSoftSerial.h>    
#include <SoftwareSerial.h>   
#include <ODriveUART.h>
#include <math.h>

// Arduino connection to Odrive and to ESP-01 (here written as ESP8266)
// pin 8  ← ODrive TX  (AltSoftSerial RX)
// pin 9  → ODrive RX  (AltSoftSerial TX)
// pin 2  ← ESP8266 TX (SoftwareSerial RX)
// pin 3  → ESP8266 RX (SoftwareSerial TX)

//dont forget for the Odrive connection, to connect the 5V and GND pin, check odrive documentation for help. The Arduino needs to know which signal is low or high.


AltSoftSerial odrv(8, 9);    // ODrive pin connection
SoftwareSerial esp(2, 3);    // ESP pin connection
ODriveUART    odrive(odrv);

bool   systemOn       = false;
float  gValue         = 5.0f;      // m/s²
float  integratedRev  = 0.0f;      // fractional revolution for velocity
unsigned long lastVelT = 0;        // for dt
String commandBuf;

const unsigned long TORQUE_DT  = 10;   // ms
const float         mass      = 0.1f;  // kg for apple
const float         leverArm  = 0.3f; // in m for distance elbow axis to the center of the object being held on the hand

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Init ODrive first, if you initiate the ESP, it causes problems. So make sure connection to the Odrive is done and completed
  odrv.begin(19200);
  Serial.println("Waiting for ODrive...");

  //always check in a while loop, this is also advised from the example code shared on the Odrive website
  while (true) {

    //When you're using multiple software/hardware serial ports (like for ODrive and ESP-01), only one can be active at a time.Thats why we use object.listen() method.
    odrv.listen();
    if (odrive.getState() != AXIS_STATE_UNDEFINED) break;
    delay(100);
  }
  Serial.println("Found ODrive");

  // Since we are using the AltSoftSerial , we have to use the command listen , in order to for the arduino to communicate to the Odrive
  odrv.listen();
  
  //this is for clearing any errors
  odrive.clearErrors();

  // set the Odrive on torque mode. being 1
  odrv.listen();
  odrv.println("w axis0.controller.config.control_mode 1");
  delay(100);

  //also set the Odrive in closed loop , in order to be able to read from the encoder. Odrive is used as a link in between Arduino -> encoder.
  odrv.listen();
  while (odrive.getState() != AXIS_STATE_CLOSED_LOOP_CONTROL) {
    odrive.setState(AXIS_STATE_CLOSED_LOOP_CONTROL);
    delay(10);
    odrv.listen();
  }
  Serial.println("→ ODrive in closed-loop torque-control");

  // capture the timestamp when the communcation is set, needed for velocity integration later :
  lastVelT = millis();

  // here now after completing setting up the Odrive, you set the connection to the ESP
  esp.begin(9600);
  Serial.println("Awaiting ESP commands…");
}

// proccessCommand function is to handle “objectHeld”, “systemShutdown”, or new gravity value from Unity
void processCommand(const String &cmdRaw) {
  String cmd = cmdRaw; cmd.trim();
  Serial.print("[ESP] "); Serial.println(cmd);

  if (cmd.equalsIgnoreCase("objectHeld")) {
    // reset our fractional revolutions to zero
    odrv.listen();
    // clear any previous velocity drift
    integratedRev = 0.0f;
    // reset our timer
    lastVelT = millis();
    systemOn = true;
    Serial.println("SYSTEM ON");
  }
  else if (cmd.equalsIgnoreCase("systemShutdown")) {
    systemOn = false;
    odrv.listen(); odrive.setTorque(0);
    Serial.println("SYSTEM OFF");
  }
  else {
    // numeric -> gravity
    float v = cmd.toFloat();
    if (cmd == "0" || v != 0.0f) {
      gValue = v;
      Serial.print("GRAVITY set to "); Serial.println(gValue);
    }
  }
}

// use a buffer to store the commands from the ESP to make the communication more reliable
void updateSerial() {
  while (esp.available()) {
    char c = esp.read();
    if (c == '\n') {
      if (commandBuf.length()) {
        processCommand(commandBuf);
        commandBuf = "";
      }
    }
    else if (c >= 32 && c <= 126) {
      commandBuf += c;
    }
  }
}

void updateTorque() {
  if (!systemOn) {
    odrv.listen();
    odrive.setTorque(0);
    return;
  }

  //switch to ODrive port
  odrv.listen();

  //read velocity & integrate over time to get position
  unsigned long now = millis();
  float dt = (now - lastVelT) / 1000.0f;
  lastVelT = now;

  float vel = odrive.getFeedback().vel;   // revs/sec
  integratedRev += vel * dt;
  if (integratedRev >  0.5f) integratedRev -= 1.0f;
  if (integratedRev < -0.5f) integratedRev += 1.0f;

  // convert revs to angle
  float angle = integratedRev * 2.0f * PI;

  // clamp [0…40degrees] but can be any angle, so 0.785 is max angle in radians where torque becomes 0 Nm
  float angle_check = angle < 0 ? 0 : angle;
  if (angle_check > 0.785f) err = 0.785f;

  // torque calculation
  float       maxT     = mass * gValue * leverArm;
  float       torque      = -maxT * (1 - sin((angle_check / 0.785f) * (PI/2)));

  odrive.setTorque(tau);

  // debug in the serial monitor to see
  Serial.print("revs="); Serial.print(integratedRev, 3);
  Serial.print(" θ=");   Serial.print(angle,        3);
  Serial.print(" err="); Serial.print(err,          3);
  Serial.print(" T=");   Serial.println(torque,          3);
}

void loop() {
  // keep the ODrive port active
  odrv.listen();

  //check the temporary buffer for ESP commands
  updateSerial();

  //The torque is updated every 10 milliseconds to give the Arduino enough time to work properly and avoid sending too many commands too quickly to the motor.

  static unsigned long lastRun = 0;
  unsigned long now = millis();
  if (now - lastRun >= TORQUE_DT) {
    updateTorque();
    lastRun = now;
  }
}
