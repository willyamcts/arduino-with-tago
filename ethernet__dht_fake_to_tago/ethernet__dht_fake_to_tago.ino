/*
 * Based:
 *    - https://docs.tago.io/en/articles/6-arduino
 *    
 *    // Arduino + Shield Ethernet + LED + Sensor de temperatura (fake)
 */

#include <Ethernet.h>

#define LED 7

//Definicoes de IP, mascara de rede e gateway
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 15, 177);

// Token Tago.io
String Device_Token = "f3832aba-b9b3-4367-915c-290cea3b3b94";
//int keyIndex = 0;         // your network key Index number (needed only for WEP)

// Tempo para atualizar os dados no Tago.io
int timeWait = 60000*5;


///////////////////////////////////////////////
//          Code Static Bellow...            //
///////////////////////////////////////////////

EthernetClient client;

// server address:
char server[] = "api.tago.io";

unsigned long lastConnectionTime = 0;
int temp = 0;



void setup() {  
  Serial.begin(9600);

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  
  pinMode(LED, OUTPUT);

  digitalWrite(LED, HIGH);
}



void loop() {
  
  if( millis() < 5000 || millis() > (lastConnectionTime+timeWait) ) {
    
    Serial.print(millis()); 
    changeLED();

    temp = random(15, 40);
    Serial.print("\n\nTemperatura: "); Serial.println(temp);
    Serial.print("LED: "); Serial.println(digitalRead(LED));

    httpRequest();
  }
  
}





void changeLED() {
  
  if ( digitalRead(LED) == HIGH ) {
    digitalWrite(LED, LOW);
  } else {
    digitalWrite(LED, HIGH);
  }
}



// this method makes a HTTP connection to the server:
void httpRequest() {
    // close any connection before send a new request.
    // This will free the socket on the WiFi shield
    client.stop();

    // if you get a connection, report back via serial:
    String postData = String("[{\"variable\":\"temperature\", \"value\":") + String(temp)+ String(",\"unit\":\"C\"},") + 
      String("{\"variable\":\"led\", \"value\":") + bool(digitalRead(LED)) + String("}]");

    Serial.println(postData);
    
    Serial.println("\n\tStarted connection to server...");

    /*
    {
      "variable": "temperature",
      "value": 26,
      "unit": "C"
    }
     */

  /*
   {
    {
      "variable": "temperature",
      "value": 26,
      "unit": "C"
    },
    
    {
      "variable": "led",
      "value": 0
    }
    }
     */

    
    String Dev_token = String("Device-Token: ")+ String(Device_Token);
    
    if (client.connect(server,80)) {                      // we will use non-secured connnection (HTTP) for tests
      Serial.println("connected to server");
      // Make a HTTP request:
      client.println("POST /data? HTTP/1.1");
      client.println("Host: api.tago.io");
      client.println("_ssl: false");                        // for non-secured connection, use this option "_ssl: false"
      client.println(Dev_token);
      client.println("Content-Type: application/json");
      client.print("Content-Length: ");
      client.println(postData.length());
      client.println();
      client.println(postData);
      // note the time that the connection was made:
      lastConnectionTime = millis();
      Serial.println("Data sended...");
      Serial.println(" - - - - - - / / - - - - - - ");
  }
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println(" - - - - - - / / - - - - - - ");
  }
}
