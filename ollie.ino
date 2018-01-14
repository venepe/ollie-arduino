#include <ESP8266WiFi.h>;
#include <Servo.h>
#include <WebSocketsClient.h>
#define SERVO_PIN        D1
#define RECV_HALF_PIN    D2
#define RECV_FULL_PIN    D3
#define SERVO_DEFAULT    90
#define SERVO_STOP       90
#define SERVO_DOWN       60
#define SERVO_UP         120
#define UP               0
#define DOWN             1
//CHANGE TO THE IP ADDRESS OF THE OLLIE NODE SERVER
#define IP_ADDRESS       "YOUR_IP_ADDRESS"
#define PORT             8080

Servo servo;
WebSocketsClient webSocket;
int requestedDirection = -1;

void setup() {
  Serial.begin(9600);
  //Set pin type
  pinMode(RECV_HALF_PIN, INPUT);
  pinMode(RECV_FULL_PIN, INPUT);
  pinMode(SERVO_PIN, OUTPUT);

  servo.attach(SERVO_PIN);
  servo.write(SERVO_DEFAULT);

  delay(10);

  WiFi.mode(WIFI_STA);
  delay(500);

  WiFi.beginSmartConfig();

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    Serial.println(WiFi.smartConfigDone());
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  webSocket.begin(IP_ADDRESS, PORT, "/");
  webSocket.onEvent(webSocketEvent);
}

int value = 0;

void loop() {
  webSocket.loop();
  int halfProx = digitalRead(RECV_HALF_PIN);
  int fullProx = digitalRead(RECV_FULL_PIN);
  if (halfProx == LOW && requestedDirection == DOWN) {
    servo.write(SERVO_STOP);
  } else if (fullProx == LOW && requestedDirection == UP) {
    servo.write(SERVO_STOP);
  }
  delay(200);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
      Serial.printf("[WSc] Connected to url: %s\n", payload);

      // send message to server when Connected
      webSocket.sendTXT("Connected");
      webSocket.sendTXT("register:1");
    }
      break;
    case WStype_TEXT: {
      Serial.printf("[WSc] get text: %s\n", payload);
      String str = (char*)payload;

      if (str.equals("up")) {
        Serial.printf("Up");
        requestedDirection = UP;
        servo.write(SERVO_UP);
      } else if (str.equals("down")) {
        Serial.printf("Down");
        requestedDirection = DOWN;
        servo.write(SERVO_DOWN);
      } else if (str.equals("stop")) {
        Serial.printf("Stop");
        servo.write(SERVO_STOP);
      }
    }
      break;
    case WStype_BIN: {
      Serial.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);
    }
      break;
  }

}
