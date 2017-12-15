// #include สองบรรทัดด้านล่างนี้ต้องมีเสมอ 
#include <ESP8266WiFi.h> 
#include <MicroGear.h>

const char* ssid = "abcddd"; // ใส่ชื่อ WiFi SSID แทน SSID 
const char* password = "panayuoat"; // ใส่รหัสผ่าน WiFi แทน PASSWORD

#define APPID "CPCUHWSynLab" // ให้ YOUR_APPID แทนที่ด้วย AppID 
#define KEY "L5NiGlMSwnpT1Gv" // ให้ YOUR_KEY แทนที่ด้วย Key 
#define SECRET "OAhEC66LXmLiBst6tG1nUZxNb" // ให้ YOUR_SECRET แทนที่ด้วย Secret 

#define ALIAS "pieblink" // ตั้งชื่อเล่นให้ device นี้ เป็น pieblink

WiFiClient client;

int timer = 0; 
char state = 0;

MicroGear microgear(client); // ประกาศตัวแปร microgear

// สร้างฟังก์ชั่นที่จะถูกเรียกเมื่อมีข้อความเข้ามา 
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) { 
  Serial.print("Incoming message -- >"); 
  msg[msglen] = '\0'; 
  Serial.println((char *)msg);

// ถ้าข้อความที่เข้ามาเป็น 1 ให้เปิด LED ถ้าเป็น 0 ให้ปิด LED 
if(*(char *)msg == '1'){ 
  digitalWrite(LED_BUILTIN, LOW); // LED on 
  }else{ 
    digitalWrite(LED_BUILTIN, HIGH); // LED off 
    } 
}
// สร้างฟังก์ชั่นที่จะถูกเรียกเมื่อ Microgear เชื่อมต่อกับ NETPIE สาเร็จ 
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) { 
  Serial.println("Connected to NETPIE...");
  // เราอาจจะใช้โอกาสนี้ตั้งชื่อหรือเปลี่ยนชื่อหรืออาจจะทาอะไรบางอย่างเช่น subscribe 
  microgear.setAlias(ALIAS); 
  }
  
void setup() { 
  // ประกาศให้เวลามีข้อความเข้ามาให้ไปทาฟังก์ชั่น onMsghandler() 
  microgear.on(MESSAGE,onMsghandler);


// ประกาศให้เมื่อเชื่อมต่อสาเร็จให้ไปทาฟังก์ชั่น onConnected() 
  microgear.on(CONNECTED,onConnected);
  
  Serial.begin(115200); 
  Serial.println("Starting...");
  
  pinMode(LED_BUILTIN, OUTPUT);
// initiate Wifi 
  if (WiFi.begin(ssid, password)) { 
    while (WiFi.status() != WL_CONNECTED) { 
      delay(500); 
      Serial.print(".");  
      }
  }
  Serial.println("WiFi connected"); 
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
  
  // initialize ตัวแปร microgear 
  microgear.init(KEY,SECRET,ALIAS);
  // เชื่อมต่อไป NETPIE ไปยัง AppID ที่กาหนด 
  microgear.connect(APPID); // ฟังก์ชั่นเชื่อมต่อ NETPIE 
}
void loop() { 
  // เช็คว่า Microgear ยังเชื่อมต่ออยู่หรือไม่ 
  if (microgear.connected()) { // ตรวจสอบการเชื่อมต่อ NETPIE 
    Serial.println("connected"); // พิมพ์แจ้งการเชื่อมต่อ NETPIE สาเร็จ
  // เราต้องเรียก microgear.loop() เป็นระยะ เพื่อรักษาการเชื่อต่อ 
  microgear.loop();
if (timer >= 1000) { 
  Serial.println("Publish..."); // พิมพ์แจ้งการส่งข้อมูล NETPIE

// chat หาตัวเองด้วย state ที่ตรงข้ามกัน 
  if(state==0){ 
    microgear.chat(ALIAS,state); 
    state=1; 
    }else{ 
      microgear.chat(ALIAS,state); 
      state=0; 
    } 
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

