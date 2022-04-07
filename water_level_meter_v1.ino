#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

#define WIRE_25 33
#define WIRE_50 34
#define WIRE_75 35
#define WIRE_100 32
#define RELAY 13
#define RESERV_WIRE 36    //wire sensor pin to check if water in reserviour is still within threshold
//#define BUZZER ??

bool flag = 0;
char what_level;
String pumpState, reservState, checkBoxState, whatImage;

//...............................................change ssid and password to yours
const char* ssid = "hotsoup";
const char* password = "chickenstew";

AsyncWebServer server(80);

//..................................................Fuctions to update States and preprocessor..................................................
//check what level the water is in by reading the analog value on the wires
char waterLevel() {
  uint16_t check_100 = analogRead(WIRE_100);
  Serial.println(check_100);
  uint16_t check_75 = analogRead(WIRE_75);
  Serial.println(check_75);
  uint16_t check_50 = analogRead(WIRE_50);
  Serial.println(check_50);
  uint16_t check_25 = analogRead(WIRE_25);
  Serial.println(check_25);
  //......................................................check to see which is getting voltage from the 3.3v source
  if ((check_100 > 100) && (check_75 > 100) && (check_50 > 100) && (check_25 > 100)) {
    flag = 0;
    what_level = 'a';
    return what_level;
  }
  else if ((check_75 > 100) && (check_50 > 100) && (check_25 > 100) && (check_100 < 100)) {
    what_level = 'b';
    return what_level;
  }
  else if ((check_50 > 100) && (check_25 > 100) && (check_100 < 100) && (check_75 < 100)) {
    what_level = 'c';
    return what_level;
  }
  else if ((check_25 > 50) && (check_100 < 100) && (check_75 < 100) && (check_50 < 100)) {
    //&& (check_100 = 0) && (check_75 = 0) && (check_50 = 0)
    flag = 1;
    what_level = 'd';
    return what_level;
  }
  //check if tank is empty, though I didn't make use of it's 'what_level' in the web page
  else if ((check_100 < 100) && (check_75 < 100) && (check_50 < 100) && (check_25 < 100) && (analogRead(RESERV_WIRE) != 0)) {
    flag = 1;
    what_level = 'e';
  }
}
//............................................................to send pump state to webpage
String pumpUpdate() {
  if (flag) pumpState = "ON";
  else if (!flag) pumpState = "OFF";
  else pumpState = "can't determine";
  return pumpState;
}
//...........................................................send reserviour state to webpage
String reservUpdate() {
  if (analogRead(RESERV_WIRE) > 100) reservState = "Not Empty";
  else if (analogRead(RESERV_WIRE) < 100) reservState = "Empty";
  else reservState = "can't determine";
  return reservState;
}
//.......................................send the flag state to the webpage so the check-box can be updated per xml request-- cool;)
String checkUpdate() {
  if (flag) checkBoxState = "checked";
  else checkBoxState = "";
  return checkBoxState;
}
//..............................................send which GIF image best represent the state of the tank and that of the pump
String updateImage() {
  switch (waterLevel()) {
    case 'a':
      whatImage = "100off";
      break;
    case 'b':
      if (flag) whatImage = "75on";
      else whatImage = "75off";
      break;
    case 'c':
      if (flag) whatImage = "50on";
      else whatImage = "50off";
      break;
    case 'd':
      if (flag) whatImage = "25on";
      else whatImage = "25off";
      break;
    default:
      whatImage = "25off";
  }
  return whatImage;
}
//........................................send placeholders strings through the request->send function in AsyncWebServerRequest
String processor(const String& var) {
  Serial.println(var);
  if (var == "PUMPSTATE") return pumpUpdate();

  if (var == "RESERVSTATE") return reservUpdate();

  if (var == "CHECKFLAGSTATE") return checkUpdate();

  if (var == "IMAGESOURCE") return updateImage();

  return String();
}
//....................................................................................................................................

void setup() {
  Serial.begin(115200);
  Serial.print("Welcome");

  pinMode(WIRE_25, INPUT);
  pinMode(WIRE_50, INPUT);
  pinMode(WIRE_75, INPUT);
  pinMode(WIRE_100, INPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(RESERV_WIRE, INPUT);

  digitalWrite(RELAY, LOW);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  //....................................................Connect to Wi-Fi
  //............................wifi try connecting for 5 trials and break out of loop if unsuccesful,
  //.......................................to restart connection, press the reset botton
  WiFi.begin(ssid, password);
  uint8_t i;
  for (i = 0; i < 5 && WiFi.status() != WL_CONNECTED; i++) {
    delay(1000);
    Serial.println("connecting to WiFi.." + i);
    if (i == 4) Serial.print("Couldn't connect to WiFi, press the reset botton to restart and try again");
  }
  //  while (WiFi.status() != WL_CONNECTED) {
  //    delay(1000);
  //    Serial.println("Connecting to WiFi..");
  //  }

  //...................................................Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  //.....................................................Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  //....................................................Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  //.....................................I had to create a seperate a seperate javaScript file, the embedded <script> method kindof broke :(
  server.on("/script", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });

  //.......................................read the query response from the /update? url and switch the pump state, high if "1", and low if "0"
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam("pumpstate")) {
      Serial.print("Pump is switched:");
      if (request->getParam("pumpstate")->value() == "1") {
        digitalWrite(RELAY, HIGH);
        flag = 1;
        Serial.print("On");
      }
      else if (request->getParam("pumpstate")->value() == "0") {
        digitalWrite(RELAY, LOW);
        flag = 0;
        Serial.print("Off");
      }
    }
    request->send(200, "text/plain", "OK");
  });

  //............................................................Route to load GIF image files
  server.on("/100off", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/100off.gif", "image/GIF");
  });
  server.on("/75off", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/75off.gif", "image/GIF");
  });
  server.on("/75on", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/75on.gif", "image/GIF");
  });
  server.on("/50off", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/50off.gif", "image/GIF");
  });
  server.on("/50on", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/50on.gif", "image/GIF");
  });
  server.on("/25off", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/25off.gif", "image/GIF");
  });
  server.on("/25on", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/25on.gif", "image/GIF");
  });
  //...........................................................................................................

  // ..........................................XML response to change the gif image source, pump state and reserv state
  //.........................................I think at this point the placeholder method used above will become obsolute,
  //...............................................nevertheless it should useful for the first loading of the page
  server.on("/updateimage", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", updateImage().c_str());
  });
  server.on("/pump", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", pumpUpdate().c_str());
  });
  server.on("/reserv", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", reservUpdate().c_str());
  });
  //  server.on("/checkbox", HTTP_GET, [](AsyncWebServerRequest *request){
  //    request->send(200, "text/plain", checkUpdate().c_str());
  //  });

  //..........................................Start server
  server.begin();
}

void loop() {
  //............................................check if the reserviour is empty and put off the pump relay,
  //...............................................this overides any other function that toggles flag/pump state
  if (analogRead(RESERV_WIRE) < 100) {
    Serial.println(analogRead(RESERV_WIRE));
    flag = 0;
    digitalWrite(RELAY, LOW);
    Serial.println("reservour is empty");
  }

  else {
    //call the waterlevel function and check what is returned
    //case'd' toggles the pump relay HIGH and case'a' toggles the pump relay LOW
    waterLevel();

    switch (waterLevel()) {
      case 'a':
        //output the water percent is 100
        Serial.println("water level is 100%");
        digitalWrite(RELAY, LOW);
        break;
      case 'b':
        //output the water percent is 75
        Serial.println("water level is 75%");
        break;
      case 'c':
        //output the water percent is 50
        Serial.println("water level is 50%");
        break;
      case 'd':
        //output water percent is 25
        Serial.println("water level is 25%");
        if (flag == 1) {
          digitalWrite(RELAY, HIGH);
        }
        break;
    }
  }

  Serial.print("flag is: ");
  Serial.println(flag);
  delay(1000);
}
