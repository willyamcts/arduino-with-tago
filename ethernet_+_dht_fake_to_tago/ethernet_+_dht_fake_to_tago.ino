#include <Ethernet.h>


// Arduino + Shield Ethernet + LED + Sensor de temperatura 

#define LED 7



///////////////////////////////
//       -------------       //
///////////////////////////////

//Definicoes de IP, mascara de rede e gateway

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192,168,15,88);          //Define o endereco IP
IPAddress gateway(192,168,15,1);     //Define o gateway
IPAddress subnet(255, 255, 255, 0); //Define a máscara de rede

// Token Tago.io
String Device_Token = "22cb3b63-387b-4881-8ca7-7e7ab91b96fc";
//int keyIndex = 0;         // your network key Index number (needed only for WEP)


int timeWait = 2000;
unsigned long lastConnectionTime = 0;
int temp = 0;


// server address:
char server[] = "api.tago.io";
EthernetClient client;



void setup() {  
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  delay(1000);
  pinMode(LED, OUTPUT);

  digitalWrite(LED, HIGH);
}



void loop() {

  if( millis() > (lastConnectionTime+timeWait) ) {
    
    
    changeLED();

    temp = random(15, 35);
    Serial.print("Temperatura: "); Serial.println(temp);
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

    Serial.println("\nStarting connection to server...");
    // if you get a connection, report back via serial:
    String PostData = String("[{\"variable\":\"temperature\", \"value\":") + String(temp)+ String(",\"unit\":\"C\"},") + 
      String("{\"variable\":\"led\", \"value\":") + bool(digitalRead(LED)) + String("}]");

    Serial.print("\n\n"); Serial.println(PostData);

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
    client.println(PostData.length());
    client.println();
    client.println(PostData);
    // note the time that the connection was made:
    lastConnectionTime = millis();
    Serial.println("Data sended...");
  }
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}
