#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Wi-Fi";
const char* password = "Eabd1234@@";

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("C");
  }
  Serial.println("WIFI");
}

void loop() {
  int i = 0;
  
  // Wait for the trigger string from the STM32
  while (!Serial.available() || Serial.readStringUntil('\n') != "UPDATE_TRIGGER") {
    // Wait until the trigger is received
      Serial.println("WIFI");
         delay(1000);
  }
  
  /* Need to read file from web to check version */
  String version = "";
  while (version == "") {
    version = Serial.readString();
    version.trim(); // Trim leading and trailing whitespaces
  }
  String url2 = "http://amres.freevar.com/Update.txt";
  String Check;
  
  // Send HTTP GET request
  WiFiClient client2;
  HTTPClient http2;
  http2.begin(client2, url2);
  int httpCode2 = http2.GET();
  
  if (httpCode2 == HTTP_CODE_OK) {
    // Get the response payload
    Check = http2.getString();
    Check.trim(); // Trim leading and trailing whitespaces
  }
  
  if (version.equals(Check))
  {
    // Tell STM that there is no update 
    Serial.println("NO");
  }
  else {
    // Tell STM that there is an update available 
    Serial.println("AV");
    Serial.println(Check); 
    // GET Connect to HEX FILE 
    String url = "http://amres.freevar.com/value.txt";
    
    // Send HTTP GET request
    WiFiClient client;
    HTTPClient http;
    http.begin(client, url);
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
      // Get the response payload
      String payload = http.getString();
      
      //Serial.println("File content:");
      while (payload[i] != '\0') {
        while (!Serial.available() || Serial.readStringUntil('\n') != "ok") ;
        
        while (payload[i] != '\n') {
          Serial.println(payload[i]);
          i++;
        }
        
        if (payload[i] == '\n') {
          Serial.println("DN");
          i++;
        }
      }
      
      if (payload[i] == '\0') {
        Serial.println("done");
      }
    }
    
    http.end();

  }
}
