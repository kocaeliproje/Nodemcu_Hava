#include <dht11.h> // dht11 kütüphanesini ekliyoruz.
#define DHT11PIN D5 // DHT11PIN olarak Dijital 2"yi belirliyoruz.
#include <Adafruit_GFX.h>      // include adafruit graphics library
#include <Adafruit_PCD8544.h>  // include adafruit PCD8544 (Nokia 5110) library
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//ağ bilgilerinizi girin
const char* ssid = "MMMMMM";
const char* password = "0000000";

// Nokia 5110 LCD module connections (CLK, DIN, D/C, CS, RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(D4, D3, D2, D1, D0);

//#define DHTPIN  D5           // DHT11 data pin is connected to NodeMCU pin D5

dht11 DHT11;

void setup()
{
  Serial.begin(115200); // Seri iletişimi başlatıyoruz.
  Serial.println("Başlatılıyor");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Bağlantı hatası, yeniden başlıyor!!!");
    delay(5000); //5sn bekle
    ESP.restart();//sistemi yeniden başlat
  }

    //iletişim için gerekli port
  // standart olarak  8266 portu gelir
  // ArduinoOTA.setPort(8266); //komutu ile değiştirilebilir

  // Sunucu ismi olarak  esp8266-[ChipID] gelir
  // ArduinoOTA.setHostname("myesp8266"); //komutu ile değiştirilebilir

  // Standart olarak şifre sormaz ama sorması istenirse
  // ArduinoOTA.setPassword("admin"); //komutu ile şifre belirlenebilir

  // Aynı zamanda bu şifre MD5 kriptolama algoritması ile kriptolanabilir.
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

//////////////////////////////////////////////////////////////////////////////////////
//daha sonra yeniden OTA kod atabilmek için gerekli OTA tanımlamalarını başlatmalıyız
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOT: FS kullandıysanız FS.end()komutu ile dosyayı kapatmalısınız
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Hata[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Yetki Hatası");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Baslatma Basarisiz");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Baglanti Basarisiz");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Veri Alinamadi");
    else if (error == OTA_END_ERROR) Serial.println("Sonlandirma Basarisiz");
  });
  ArduinoOTA.begin();//OTA'yi başlatıyor
  Serial.println("Sistem Hazir");
  Serial.print("IP Adresiniz: ");
  Serial.println(WiFi.localIP());

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  display.begin();
    
  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(50);

  display.clearDisplay();   // clear the screen and buffer

  display.drawFastHLine(0, 23, display.width(), BLACK);

  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(6, 0);
  display.print("SICAKLIK:");
  display.setCursor(15, 28);
  display.print("NEM:");
  display.display();

   }

void loop()
{
   ArduinoOTA.handle();//sonraki kod atma işlemleri için kullanmamız gerekiyor
  // Bir satır boşluk bırakıyoruz serial monitörde.
  Serial.println();
  display.begin();
  // Sensörün okunup okunmadığını konrol ediyoruz. 
  // chk 0 ise sorunsuz okunuyor demektir. Sorun yaşarsanız
  // chk değerini serial monitörde yazdırıp kontrol edebilirsiniz.
  int chk = DHT11.read(DHT11PIN);
  int Humi = DHT11.humidity *10;
  int Temp = DHT11.temperature *10;
  // Sensörden gelen verileri serial monitörde yazdırıyoruz.
  Serial.print("Nem (%): ");
  Serial.println((float)DHT11.humidity, 2);

  Serial.print("Sicaklik (Celcius): ");
  Serial.println((float)DHT11.temperature, 2);

Serial.println(chk);
Serial.println(Humi);
Serial.println(Temp);

// print temperature (in °C)
  display.setCursor(18, 12);
  //display.printf(DHT11.humidity);
    display.printf(" %02u.%1u C", (Temp/10)%100, Temp  % 10);
  // print degree symbol ( ° )
  display.drawRect(50, 12, 3, 3, BLACK);

// print humidity (in %)
  display.setCursor(24, 40);
  display.printf("%02u.%1u %%", (Humi/10)%100, Humi % 10);

  

  // 2 saniye bekliyoruz. 2 saniyede bir veriler ekrana yazdırılacak.
  delay(2000);

}
