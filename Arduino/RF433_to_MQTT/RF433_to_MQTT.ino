#include <WiFi.h>
#include <ESP32Ping.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>
#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h> // Not actually used but needed to compile
#endif

RH_ASK radio(2000, 4, 5, 0);

const char *ssid = "******";
const char *password = "******";

const char *mqtt_server =  "farmer.cloudmqtt.com"; 
const int   mqtt_port =  10614; 
const char *mqttUser = "*******";
const char *mqttPassword = "********";
const char *mqttClientID = "esp32-PIR-bridge";
char *topic = "motion/zones";
const int buf_size = 12;
const char *msg_pattern = "{'zone': ";
const int ptr_size = strlen(msg_pattern);

long        lastReconnectAttempt = 0;
 
WiFiClient   espClient;
PubSubClient mqttClient(espClient);

void setup() {
  Serial.begin(115200);

  Serial.println("** Starting up **");
  setup_wifi();
 
  Serial.println("** Connect to mqtt server **");
  mqttClient.setServer(mqtt_server, mqtt_port);
  
  lastReconnectAttempt = 0;
  /* End ESP8266 Stuff */

  if (!radio.init()) {
      Serial.println("** RF init failed **");
    } else{
      Serial.println("** RF init passed **");
  }
  
  Serial.println("** Setup done **");
 
}
 
void loop() {

  uint8_t receive_buffer[buf_size];
  uint8_t buflen = sizeof(receive_buffer);
  
  if (radio.recv(receive_buffer, &buflen))
  {
    Serial.print("** Got RF Message: "); Serial.print((char*)receive_buffer); Serial.println(" **");
    if (isCorrectMessage((char *) receive_buffer)){
       Serial.println("** Sending message to MQTT ");
       sendToMQTT((char *) receive_buffer);
    }
  }

}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println(); 
  Serial.print("** Connecting to - "); Serial.print(ssid); Serial.println(" **");
  
//  WiFi.setSleepMode(WIFI_NONE_SLEEP); // Suggested to help with led flickering issue
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("** WiFi connected **");
  Serial.print("** IP address: "); Serial.print(WiFi.localIP()); Serial.println(" **");
}

boolean reconnect() {
  Serial.println("** Reconnecting to mqtt....");
  mqttClient.connect(mqttClientID, mqttUser, mqttPassword );
  return mqttClient.connected();
}


bool isCorrectMessage(char *message ){
  Serial.print("** Checking message '");Serial.print(message); Serial.println("' for correctness");
  for (int i = 0; i < ptr_size; i++ ){
    Serial.print("** Check if '"); Serial.print(message[i]); Serial.print("' != '"); Serial.print(msg_pattern[i]); Serial.println("' **");
    if (message[i] != msg_pattern[i]){
      return false;
    }  
  }

  Serial.print("** Check if '"); Serial.print(message[buf_size - 1]); Serial.print("' != "); Serial.println("'}' **");
  if (message[buf_size - 1] != '}'){
    return false;
  }
  
  Serial.println("** Message is correct **");
  return true;
}

void sendToMQTT(char *message){
   char messageToSend[buf_size+1];
   strncpy( messageToSend, message, buf_size );
   messageToSend[buf_size] = '\0';
   if (!mqttClient.connected()) {
    Serial.println("** mqtt NOT connected **");
    reconnect();
  }
  
  if ( mqttClient.connected() ) {
    Serial.print("** Publishing '"); Serial.print(messageToSend); Serial.println("' to MQTT **");
    mqttClient.publish(topic, messageToSend);
    mqttClient.loop();
  } else {
    Serial.println("** Not Connected to MQTT **");
    Serial.print("** State after reconnection: ");
    Serial.println(mqttClient.state());
  }
}
