/**********************************************************
 *      Nesnelerin İnterneti ve Uygulamaları Dersi
 *         Proje Uygulaması (Akıllı Çöp Kutusu)
 *              Yazar : Nihad Asgarov
 *********************************************************/

/* Kütüphane Dosyaları */
#define BLYNK_TEMPLATE_ID "TMPL6h7UNiRJx"
#define BLYNK_TEMPLATE_NAME "Iot SmartDustBin"
#define BLYNK_AUTH_TOKEN "aLgifZsuoX81T08ZDJH_xrlIi1v5ozw_"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <ThingSpeak.h>
#include <ThingESP.h>

/* ThingESP Kurulumu */
ThingESP8266 thing("nihadasgarov", "SmartDustBin", "nihadasgarov");

/* Kablosuz Bağlantı Bilgileri */
char auth[] = "aLgifZsuoX81T08ZDJH_xrlIi1v5ozw_";
char ssid[] = "Asgarov";
char pass[] = "nihat12345";

const char* WLAN_SSID = "Asgarov";
const char* WLAN_PASSWORD = "nihat12345";

BlynkTimer timer;
Servo servo;

/* ThingSpeak Kurulumu */
const char* thingSpeakHost = "api.thingspeak.com";
unsigned long Channel_ID = 2388538 ;
const char * WriteAPIKey = "ZCIBIAM4EKJHJDF8";
const int field_no = 1;

// ---------- Çöp Kutusu Değişkenleri ve Pin Tanımları ------------//
int trigPin1 = D6; 
int echoPin1 = D5;  
int buzzer = D4 ;
long sure1;
long mesafe1;
int seviye=0;
int yukseklik=0;
int trigPin2 = D7; 
int echoPin2 = D8;
long sure2;
long mesafe2;

/*** ESP8266 WiFi Kurulum Fonksiyonu ***/
WiFiClient client;
void WiFi_Setup()
{
  if (WiFi.status() != WL_CONNECTED)
  {
      Serial.print("Kablosuz  Agina Baglaniyor");
      WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
      while (WiFi.status()  !=  WL_CONNECTED) {
          delay(1000);
          Serial.print(".");
      }
      Serial.println();
      Serial.print(WLAN_SSID);  
      Serial.println(" Kablosuz  Aga Baglandi");
  }
}

void acikkapali()
{
  ultrasonic2();

  if(mesafe2<50)
  {
    Serial.println("Kapak açıldı...");
    uploadData();
    servo.write(180);

    for(int i=0; i<10; i++)
    {
      Blynk.virtualWrite(V2, 90);
      ultrasonic1();
      delay(100);
    }
    Serial.println("Kapak kapandı...");
    servo.write(0);
    Blynk.virtualWrite(V2, 0);
  }
  else 
  {
    ultrasonic1(); 
    delay(100);
  }
}

void ultrasonic1()
{
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  sure1 = pulseIn(echoPin1, HIGH);
  mesafe1 = sure1 * 0.034 / 2;
  seviye =map(mesafe1, 26, 0, 0,100);
  seviye = constrain(seviye, 0, 100);
  String doluluk = "Doluluk: " + String(seviye) + " %";
  String response = HandleResponse("doluluk");
  Blynk.virtualWrite(V0, mesafe1);
  Blynk.virtualWrite(V1, seviye);

  if (seviye>70)
  {
    Serial.println("Çöp kutunuz dolmak üzere...");
    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);
    delay(50);
  }
  else
  {
    digitalWrite(buzzer, LOW);
  }
}

void ultrasonic2()
{
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  sure2 = pulseIn(echoPin2, HIGH);
  mesafe2 = sure2 * 0.034 / 2;
}

void setup()
{
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1,INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2,INPUT);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  Serial.begin(9600); 
  servo.attach(D2);
  servo.write(0);
  Blynk.begin(auth, ssid, pass);
  delay(1000);
  timer.setInterval(1000L, acikkapali);
  thing.SetWiFi("Asgarov", "nihat12345");

  thing.initDevice();
}

/* ThingSpeak Field Yazma İşlemi **/
void uploadData()
{
  WiFi_Setup();
  int data;
  data = ThingSpeak.writeField(Channel_ID, field_no, mesafe2, WriteAPIKey);
}
/* ThingESP Whatsapp mesaj gönderme */
String HandleResponse(String query)
{
  
  if (query == "doluluk") {
    return "Doluluk: " + String(seviye) + " %";
  } else {
    return "Your query was invalid..";
  }

}


void loop() 
{
  WiFi_Setup();
  Blynk.run();
  timer.run();
  thing.Handle();
}