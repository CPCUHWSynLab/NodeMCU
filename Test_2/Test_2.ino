#include <ESP8266WiFi.h>
#include <MicroGear.h>

const char* ssid     = "58304062";
const char* password = "panayuoat";

#define APPID   "CPCUHWSynLab"
#define KEY     "L5NiGlMSwnpT1Gv"
#define SECRET  "OAhEC66LXmLiBst6tG1nUZxNb" 

#define ALIAS   "pieled"



WiFiClient client;

int timer = 0;
char str[32];
//char str2[32];

char state = 0;
char stateOutdated = 0;
char buff[16];

//#define DHTTYPE DHT11          //Define sensor type
//#define DHTPIN D4              // Define sensor pin
//DHT dht(DHTPIN, DHTTYPE, 15);   //Initialize DHT sensor 

int humid = 0;
int temp;

MicroGear microgear(client);

void sendState(){
  if (state==0)
    microgear.publish("/pieled/state","0");
  else
    microgear.publish("/pieled/state","1");
  Serial.println("send state..");
  stateOutdated = 0;
}

void updateIO(){
  if (state >= 1) {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else {
    state = 0;
    digitalWrite(LED_BUILTIN, HIGH);
  }
}


void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    char m = *(char *)msg;
  
    Serial.print("Incoming message -->");
    msg[msglen] = '\0';
    Serial.println((char *)msg);

    if (m == '0' || m == '1') {
      state = m=='0'?0:1;
      updateIO();
    }
     if (m == '0' || m == '1' || m == '?') stateOutdated = 1;
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    microgear.setAlias(ALIAS);
    stateOutdated = 1;
}

void setup(){
//dht.begin();

    microgear.on(MESSAGE,onMsghandler);
    microgear.on(CONNECTED,onConnected);

    Serial.begin(115200);
    Serial.println("Starting...");

    pinMode(LED_BUILTIN, OUTPUT);

    if (WiFi.begin(ssid, password)) {
   while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
    }

    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());


    microgear.on(MESSAGE,onMsghandler);
    microgear.on(CONNECTED,onConnected);
    microgear.init(KEY,SECRET,ALIAS);
    microgear.connect(APPID);
}

void loop(){
    if (microgear.connected()) {
        if (stateOutdated) sendState();
        Serial.println("connected");
        microgear.loop();

        if (timer >= 1000) {
            humid = humid+1;
    //temp = dht.readTemperature();

            String data = "{\"humidity\":"; 
            data += humid ; 
            //data += ", \"temp\":"; 
            //data += temp ; 
            data += "}";
    
            sprintf(str,"%d,%d",humid,humid*2);
            //sprintf(str2,"%d",humid);
            microgear.writeFeed("CPCUSoilhumidity",data ,
            "o9dYEQAyVBqDmWn1SORwfFTq7afHhYjd");
            Serial.println(str);

            Serial.print("Sending -->");
            microgear.publish("/dht",str);

            timer = 0;
        } 
    else timer += 100;
    }
    else {
        Serial.println("connection lost, reconnect...");
        if (timer >= 5000) {
            microgear.connect(APPID);
            timer = 0;
        }
        else timer += 100;
    }
    delay(100);
}
