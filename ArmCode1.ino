#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Servo.h>

// Wi-Fi configuration
const char* ssid = "AndroidAP_6128";
const char* password = "112112112";
WiFiUDP udp;
unsigned int localPort = 4210;

// Servo configuration
Servo servoBase, servoArm, servoUp, servoGrip;
int b = D1, a = D3, u = D5, g = D7;

void setup() {
  // Servo setup
  servoBase.attach(b, 500, 2500);
  servoArm.attach(a, 500, 2500);
  servoUp.attach(u, 500, 2500);
  servoGrip.attach(g, 500, 2500);
  
  // Initial servo positions
  servoBase.write(90);
  servoArm.write(90);
  servoUp.write(90);
  servoGrip.write(0);

  // Wi-Fi setup
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  udp.begin(localPort);
}

void loop() {
  // Check for UDP packet
  char incomingPacket[255];
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, 255);
    if (len > 0) incomingPacket[len] = 0;
    String command = String(incomingPacket);

    if (command == "RIGHT") {
      rightPut();
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.print("done");
      udp.endPacket();
    } else if (command == "LEFT") {
      leftPut();
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.print("done");
      udp.endPacket();
    }
  }
}
void rightPut() {
  // Move arm down (reversed direction)
  servoArm.write(135); 
  delay(1000);

  // Move gripper down
  servoUp.write(60); 
  delay(1000);

  // Close grip
  servoGrip.write(0);//Catch Object 
  delay(1000);

  // Move gripper up
  servoUp.write(90); 
  delay(1000);

  // Move arm up (reversed direction)
  servoArm.write(90); 
  delay(1000);

  // Rotate base to the right
  servoBase.write(135); 
  delay(1000);

  // Move arm down to drop position (reversed direction)
  servoArm.write(135); 
  delay(1000);

  // Move gripper down
  servoUp.write(60); 
  delay(1000);

  // Open grip to release
  servoGrip.write(45); //Relese Object 
  delay(1000);

  // Return gripper to center position (90 degrees)
  servoUp.write(90); 
  delay(1500);

  // Close grip
  servoGrip.write(0); 
  delay(1100);

  // Return arm to center position (up, 90 degrees)
  servoArm.write(90); 
  delay(2100);
  // Return base to center position (90 degrees)
  servoBase.write(90); 
  delay(1100);
}

void leftPut() {
  // Move arm down (reversed direction)
  servoArm.write(135); 
  delay(1000);

  // Move gripper down
  servoUp.write(60); 
  delay(1000);

  // Close grip
  servoGrip.write(0); 
  delay(1000);

  // Move gripper up
  servoUp.write(90); 
  delay(1000);

  // Move arm up (reversed direction)
  servoArm.write(90); 
  delay(1000);

  // Rotate base to the left
  servoBase.write(45); 
  delay(1000);

  // Move arm down to drop position (reversed direction)
  servoArm.write(135); 
  delay(1000);

  // Move gripper down
  servoUp.write(60); 
  delay(1000);

  // Open grip to release
  servoGrip.write(45); 
  delay(1100);

  // Return gripper to center position (90 degrees)
  servoUp.write(90); 
  delay(1100);

  // Close grip
  servoGrip.write(0); 
  delay(1100);

  // Return arm to center position (up, 90 degrees)
  servoArm.write(90); 
  delay(2100);
  // Return base to center position (90 degrees)
  servoBase.write(90); 
  delay(1100);
}
