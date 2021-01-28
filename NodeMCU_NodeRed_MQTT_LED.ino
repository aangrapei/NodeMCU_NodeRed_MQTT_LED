/*
 * ==============================NODEMCU MQTT NODE-RED=================================
 * 
 * ===================================REMOTE LED=======================================
 * 
 * ==================================AANG RAPE'I=======================================
 * 
 * ==================================27-01-2021========================================
 * 
 * ============================= KONEKSI PIN DAN LED ==============
`* ANODA LED 1 KE PIN D5 -- DISERI DENGAN RESISTOR 330 OHM -- KATODA LED 1 KE GROUND
`* ANODA LED 2 KE PIN D6 -- DISERI DENGAN RESISTOR 330 OHM -- KATODA LED 2 KE GROUND
`* ANODA LED 3 KE PIN D7 -- DISERI DENGAN RESISTOR 330 OHM -- KATODA LED 3 KE GROUND
 */
#include <ArduinoJson.h> //Library JSON
#include <ESP8266WiFi.h> //Library  ESP8266
#include <PubSubClient.h> //Library MQTT

//Setup WIFI
const char*   ssid        = "MODEM"; //Nama SSID Wifi yang akan diakses!
const char*   pass        = "tarungjaya"; //Password Wifi

//Setup MQTT broker
const char*   mqtt_server = "broker.hivemq.com"; //Server MQTT/Broker
const int     mqtt_port   = 1883; //MQTT Port
const char*   mqttuser    = ""; //MQTT Username 
const char*   mqttpass    = ""; //MQTT Password
String        clientId    = "ESP8266Client-"; //MQTT ClientID

//Setup pin lampu
const int lamp1 = D5;
const int lamp2 = D6;
const int lamp3 = D7;

//Variabel status lampu
int stat_lamp1 = 0;
int stat_lamp2 = 0;
int stat_lamp3 = 0;

WiFiClient espclient;
PubSubClient client(espclient);

//Fungsi Menyambungkan ke Wifi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//Fungsi Menerima Pesan dari MQTT Broker
void callback(String topic, byte* payload, unsigned int length) {
  Serial.print("message arrived[");
  Serial.print(topic);
  Serial.println("]");
  String msgg;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    msgg += (char)payload[i];
  }

//Mengelola Pesan Masuk
if (topic == "ke_alat"){ //Jika topik yang datang bernama "pub_things"
    if (msgg == "lamp1") {    //jika pesan yang masuk "lamp1"
      if(stat_lamp1 == 0){    //jika set status lampu = 0
      digitalWrite(lamp1, HIGH); //nyalakan lampu
      Serial.print("\n LED 1 Menyala \n "); //tampilkan di serial monitor LED 1 Menyala
      stat_lamp1 = 1;  //set status lampu = 1
      }else{
      digitalWrite(lamp1, LOW);
      Serial.print("\n LED 1 Mati \n ");
      stat_lamp1 = 0;
      }
    }else if (msgg == "lamp2") {
      if(stat_lamp2 == 0){
      digitalWrite(lamp2, HIGH);
      Serial.print("\n LED 2 Menyala \n ");
      stat_lamp2 = 1;
      }else{
        digitalWrite(lamp2, LOW);
      Serial.print("\n LED 2 Mati \n ");
      stat_lamp2 = 0;
      }
    }else  if (msgg == "lamp3") {
      if(stat_lamp3 == 0){
      digitalWrite(lamp3, HIGH);
      Serial.print("\n LED 3 Menyala \n ");
      stat_lamp3 = 1;
      }else{
      digitalWrite(lamp3, LOW);
      Serial.print("\n LED 3 Mati \n ");
      stat_lamp3 = 0;
      }
    }
  }

  
}



//Fungsi Menyambungkan Ulang ke MQTT Broker
void reconnect() {
  while (!client.connected()) {
    clientId += String(random(0xffff), HEX);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(mqtt_server);
    if (client.connect(clientId.c_str(), mqttuser, mqttpass )) {
      Serial.println("connected");
      //client.publish("dari_alat", "hello world");//kirim pesan ke MQTT broker
      client.subscribe("ke_alat"); //Terima pesan dari MQTT broker
    } else {
      Serial.print("failed with state, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

//Fungsi ini dijalankan sekali ketika NodeMCU mulai start
void setup() {
  Serial.begin(115200);
  pinMode(lamp1, OUTPUT); //Set pin lampu 1 sebagai OUTPUT
  pinMode(lamp2, OUTPUT); //Set pin lampu 2 sebagai OUTPUT
  pinMode(lamp3, OUTPUT); //Set pin lampu 3 sebagai OUTPUT
  digitalWrite(lamp1, LOW); //matikan lampu 1
  digitalWrite(lamp2, LOW); //matikan lampu 2
  digitalWrite(lamp3, LOW); //matikan lampu 3
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
}

//Fungsi ini dijalakan berulang-ulang selama NodeMCU menyala
void loop(){
   
 if (!client.connected()) { //menyambungkan kembali jika terputus dengan MQTT Broker
    reconnect();
 }
 
 if(!client.loop()){ //menyambungkan kembali menerima pesan jika terputus dengan MQTT Broker
    client.connect("AangESP8266Client",  mqttuser, mqttpass );
 }

 //Menyiapkan Data JSON
DynamicJsonDocument doc(100);

//Set Data JSON
doc["L1"] = stat_lamp1;
doc["L2"] = stat_lamp2;
doc["L3"] = stat_lamp3;


char buffer[256];
size_t n = serializeJson(doc, buffer);
client.publish("dari_alat", buffer, n); //Publish/kirim pesan ke MQTT broker
delay(200);

}
