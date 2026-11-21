#include <Wire.h>
#define SDA 32      
#define SCL 33     

#include <WiFi.h>
#include <WiFiClientSecure.h>
#define ssid "Waheeb"//"Wejmail"//"Abed1988"// "ASUS VPN"// inser WIFI name
#define password "wa1503ej"//"abed1988"// "AM2002ej@" // WIFI Pass

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"

#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
String chatId = "1128494884";// Waheeb Habiba
String BOTtoken = "8563778655:AAHiZX333yKfK3A718KGSyuddvLL1d8YofM";
bool Firsttime = true;

WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);

#include "RTClib.h"
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int Hour,Day,InitHour,Minutes,InitMinute,InitMinute1,LastFeedHour=-1;

#include "PCF8574.h"              
#define PCF8574_ADDR 0x38
PCF8574 pcf8574(PCF8574_ADDR, SDA, SCL);

#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads;  

#include <ESP32Servo.h> 
Servo FeedServo;
#define FeedServoPin 15  

//CAMERA_MODEL_WROVER_KIT  notice that -1 means not connected

#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     21
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       19
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       5
#define Y2_GPIO_NUM       4
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define LED 2
#define LASER 14
#define ECHO 13
float Distance, Duration;
String Text;
int WaterLevel,WaterPlate, FoodPlate,WateringTime=2000,FeedingTime=2000;
//int botRequestDelay = 1000;   // mean time between scan messages
//long lastTimeBotRan;     // last time messages' scan has been done

void handleNewMessages(int numNewMessages);
String sendPhotoTelegram();

//========================= setup ==================

void setup()
{
  Serial.begin(115200);
  Wire.begin(SDA, SCL);
  pinMode(LED, OUTPUT);
  pinMode(LASER, INPUT);
  pinMode(ECHO, INPUT);

  pcf8574.begin();
  digitalWrite(LED, 1);
  pcf8574.pinMode(0, OUTPUT);   //Relay
  pcf8574.digitalWrite(0,0);
  delay(1000);
  pcf8574.digitalWrite(0,1);

  FeedServo.attach(FeedServoPin); 

  ads.begin();
  Serial.println("Getting single-ended readings from AIN0 ... ");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 0.1875mV/ADS1115)");
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  
  rtc.begin();
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //rtc.adjust(DateTime(2026, 05, 29, 20, 55, 0)); //(y,m,d,h,m,s);
  delay(1000);
  DateTime now = rtc.now();
  InitHour = (now.hour());
  Minutes = (now.minute());
  InitMinute = Minutes;
  InitMinute1 = Minutes;
  Day = (now.day());
  delay(1000); 
  
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);// to prevent esp restart on voltage drop

  Serial.print("Connect to:  ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  
  // configure ESP WIFI as Station Mode, means ESP behaves as mobile.
  //client wich connect to exist WIFI. this mode let ESP to surf in the internet. 
  //other modes WIFI_AP (Access Point Mode) - ESP becomes a router and WIFI_AP_STA ESP work in both modes
  
  WiFi.begin(ssid, password);
  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  //clientTCP.setInsecure();
  clientTCP.setTimeout(5000);

  // this line comes to solve secuirity problems,
  // it let the ESP knows that the communication is with the real Telegram server and not hacker. 
  // in other ward this line defines an id for the WIFI/TCP ( SSL Certificate) of the telegram
  //this line inject into the ESP the its cerificate ID which is the "TELEGRAM_CERTIFICATE_ROOT" is long String
  // when ESP tries to connect to the Telegram " api.telegram.org" it send the certificate ID, ESP compares
  // it to the sent by "TELEGRAM_CERTIFICATE_ROOT" and if its the same it establish the connection.
  
  while (WiFi.status() != WL_CONNECTED)
  
  // indicates if the ESP connected to the WIFI or not the phrase 
  //"WL_CONNECTED" means it was connected
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi connected.");
  Serial.print(WiFi.localIP());
/////=======================================
//  Serial.print("Gateway: ");
//  Serial.println(WiFi.gatewayIP());
//  
//  Serial.print("DNS: ");
//  Serial.println(WiFi.dnsIP());
// ////=================================== 
  // prints on the serial monitor the IP allocted for the ESP
  
  delay(1000);
  camera_config_t config;
  
  // this line define in "definition template' ( struct )which contains all the infomation the the cam
  // needs to work proerly, such as to which pins it connected , pucture quality' and what format...etc. 
  // "camera_config_t" is a data type the saved in the inlude of the esp-cam, and config is the 
  // name we choosed and will be filled with the needed params for proper work of the cam.

  // the below 2 lines configur the timer and pulse channels
  
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  
  // the cam send the pixels in parallel ( 8 bits each time -BYTE )below shows which bit connected to
  //what, pin_d0 is the  LSB where the pin_d7 is the MSB, notice that each GPIO_NUM defined to be 
  // connected to ESP pin - see line 47 and above -.
  
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;

  config.pin_xclk = XCLK_GPIO_NUM;// Main External Clock which ESP gives to  cam operation 
  config.pin_pclk = PCLK_GPIO_NUM;// indicates to the ESP the new pixels are available , read it 
  config.pin_vsync = VSYNC_GPIO_NUM; // indicates start and end of full single frame (full pic)
  config.pin_href = HREF_GPIO_NUM;// indicates start and end of sinle line in the pic.
  config.pin_sscb_sda = SIOD_GPIO_NUM;//serial data pin of the cam 
  config.pin_sscb_scl = SIOC_GPIO_NUM;// serial clock pin of the cam
  config.pin_pwdn = PWDN_GPIO_NUM;// enables power off the cam ( since it gets -1 mean not in use line 42 )
  config.pin_reset = RESET_GPIO_NUM;// enables rest to the cam ( since it gets -1 mean not in use  line 43)
  config.xclk_freq_hz = 20000000;// sets the frequency of the primery clock XCLC line 175
  config.pixel_format = PIXFORMAT_JPEG;//defines what format the cam will provide -jpeg file in our case-.

  //init with high specs to pre-allocate larger buffers
  // the below code block is responsible for memory mange and image quality adjust. 
  // in case we have external memory the cam generates images with high quality (1600x1200 pixels)
  //therwise it generates images with less quality ( 800x600 pixels). 
  if (psramFound())
  {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 2;// 2 buffers are allocated simultanuosly for the image. 
  }
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;// 1 buffers are allocated for the image.
  }

  // all the defined parametes in the struct are sent to the cam for initiolization
   
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.println("Camera init failed");
    delay(1000);
    ESP.restart();
  }

  // Drop down frame size for higher initial frame rate
  // sensor_t us strust which contains menu for all function used by the cam sensor, 
  //defined in cam library such as (brightness, contrast, resolution, image inversion, etc.) 
  //*s is variable name , * indicates that in contans an address (pointer) of cam sensor.
   
  sensor_t * s = esp_camera_sensor_get(); // returns the address of cam sensor to S
  
  s->set_framesize(s, FRAMESIZE_CIF);  // UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA
  
  //-> is the way used in C language to chnage/operate Attribute in the pointer s;in our case we changed
  // the fomat to CIF ( 352x288 pixels) less quality than was definedn above , this done on going.
     
}

///========================================= main ===============================

void loop()
{
  
  //////////  need to move to setup oppening message //////////////  habiba and raheeq
  if ( Firsttime==true)
  {
    sendTextMessage("Habiba & Raheeq Project\n\nCam & Telegram");
    Firsttime = false;
  }
  
  DateTime now = rtc.now();
   
  //defines new variable named now and belong to the class DateTime which
  // defined in RTClib.h  
  
  Minutes = (now.minute()); // RTClib function for minute seperation.
  Hour=(now.hour());// // RTClib function for hour seperation.
  Serial.print(now.hour());
  Serial.print(":");
  Serial.println(now.minute());
  delay(1000);

  // below commands are used for the US which measure distance, 
  //calculation based on 343m/s sound waves volocity 
  
  Duration = pulseIn(ECHO,1);
  
 //The pulseIn function returns the travel time (in microseconds) from when the wave is emitted 
 //until it returns.
  Distance = Duration/58.2;// equasion to calculate the distance
  Serial.print("Distance: ");
  Serial.println(Distance);
  Serial.print("initminute: ");
  Serial.println(InitMinute);
  Serial.print("minute: ");
  Serial.println(now.minute());
  if ((Distance <=30 )&&  (Minutes >= InitMinute +5))
  {
    Serial.print ("currenttime= ");
    Serial.println (Hour);
    sendPhotoTelegram();// methode/function to send an image to the telegram bot
    InitHour = (now.hour());
    InitMinute = (now.minute());
  }
  
  WaterPlate = ads.readADC_SingleEnded(0);// reading analog value of the FSR ; connected to port 0
  float DWaterPlate = ads.computeVolts(WaterPlate); 
  Serial.print("WaterPlate = ");
  Serial.println(WaterPlate);
  Serial.println("-----------------------------------------------------------");

  if (WaterPlate<=11900)//check it out
  {
    pcf8574.digitalWrite(0,0);// Turn Relay On
    delay(WateringTime);
    pcf8574.digitalWrite(0,1);//Turn Relay Off
  }
  now = rtc.now();
  Hour = (now.hour());
  Day = (now.day());
  FoodPlate = ads.readADC_SingleEnded(1);
  float DFoodPlate = ads.computeVolts(FoodPlate); 
  Serial.println("-----------------------------------------------------------");
  Serial.print("FoodPlate: "); 
  Serial.print(FoodPlate); 
  if ((Hour == 8 || Hour==20)&&FoodPlate<=21600 && LastFeedHour!=-1)
  {
    FeedServo.write(90);
    delay(FeedingTime);
    FeedServo.write(0);
    LastFeedHour = Hour;
  }
  WaterLevel = ads.readADC_SingleEnded(2);
  float DWaterLevel = ads.computeVolts(WaterLevel); // no water high values and vice versa
  Serial.println("-----------------------------------------------------------");
  Serial.print("WaterLevel = "); 
  Serial.print(WaterLevel); 
  Serial.print("  "); 
  Serial.print(DWaterLevel); 
  Serial.println("V");
  Serial.print("Lazer = ");
  Serial.println(digitalRead(LASER));
  Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++");
  Minutes=(now.minute());
  if (digitalRead(LASER) == 0 && (Minutes >= InitMinute1+15))
    { 
//      if  (Minutes >= InitMinute1+15)
//      {
        sendTextMessage("Warning: Please fill food tank!");//sendTextMessage(String message)
        InitMinute1=(now.minute());
//      }
    }
  if (WaterLevel >= 24000 && (Minutes >= InitMinute+15))
    {
//      if  (Minutes >= InitMinute1+15)
//      {
        sendTextMessage("Warning: Please fill water tank!");
        InitMinute=(now.minute());
//     }
    }
  }
String sendPhotoTelegram()
{
  const char* myDomain = "api.telegram.org";
  String getAll = "";
  String getBody = "";

  camera_fb_t * fb = NULL;

// discard only one old frame
  fb = esp_camera_fb_get();
  if (fb) 
  {
    esp_camera_fb_return(fb);
    delay(200);
  }

// take new frame
  fb = esp_camera_fb_get();
  if (!fb) 
  {
    Serial.println("Camera capture failed!");
    return "Camera capture failed";
  }
  Serial.print("Connect to: ");
  Serial.println(String(myDomain));
  if (clientTCP.connect(myDomain, 443))
  {
    Serial.print("Connection successful");
    String head = "--Habiba_and_Raheeq_final_project\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + chatId + "\r\n--Habiba_and_Raheeq_final_project\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--Habiba_and_Raheeq_final_project--\r\n";
    uint16_t imageLen = fb->len;
    uint16_t extraLen = head.length() + tail.length();
    uint16_t totalLen = imageLen + extraLen;

    clientTCP.println("POST /bot" + BOTtoken + "/sendPhoto HTTP/1.1");
    clientTCP.println("Host: " + String(myDomain));
    clientTCP.println("Content-Length: " + String(totalLen));
    clientTCP.println("Content-Type: multipart/form-data; boundary=Habiba_and_Raheeq_final_project");
    clientTCP.println();
    clientTCP.print(head);
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n = 0; n < fbLen; n = n + 1024)
    {
      if (n + 1024 < fbLen) 
      {
        clientTCP.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen % 1024 > 0)
      {
        size_t remainder = fbLen % 1024;
        clientTCP.write(fbBuf, remainder);
      }
    }
    clientTCP.print(tail);// send the tail as defined above to end connection
    
    esp_camera_fb_return(fb);
    
    // cleans the buffer frame of the Image , 
    //ready for the next one to be allocated on the same place

    int waitTime = 10000;   // timeout 10 seconds
    long startTimer = millis();
    boolean state = false;
    while ((startTimer + waitTime) > millis())
    {
      Serial.print(".");
      delay(100);

      // below while keeps reading the response till we get the first time "/n" which means that the header 
      //was ended ( header contains date,server type ...etc)
      // onc "/n" was recieved means the body of the resonce is below, then start collect the reponse to
      // the string named getbody, while making sure to drom the lette "/r" from  it.while will be ended
      // if time exceeds 10 seconds of reponce was revieved. 
      
      while (clientTCP.available()) 
      {
        char c = clientTCP.read();
        if (state == true) 
          getBody += String(c);
        if (c == '\n') // check if "/n was recieved and nothing else
        {
          if (getAll.length() == 0) 
            state = true;
          getAll = "";
        }
        else if (c != '\r')
          getAll += String(c);
        startTimer = millis();
      }
      if (getBody.length() > 0)
        break;
    }
    clientTCP.stop();
    Serial.println(getBody);
  }
  else
  {
    getBody = "Connected to api.telegram.org failed.";
    Serial.print("Connect to telegram failed");
  }
  return getBody;
}


// ======================= added by me ==========================
//
//void testTelegramDirect()
//{
//  WiFiClientSecure testClient;
//  testClient.setInsecure();
//  testClient.setTimeout(5000);
//
//  Serial.println("Connecting directly to Telegram...");
//
//  if (!testClient.connect("api.telegram.org", 443)) {
//    Serial.println("Direct connection to Telegram FAILED");
//    return;
//  }
//
//  Serial.println("Direct connection to Telegram OK");
//
//  testClient.print(String("GET /bot") + BOTtoken + "/getMe HTTP/1.1\r\n" +
//                   "Host: api.telegram.org\r\n" +
//                   "Connection: close\r\n\r\n");
//
//  while (testClient.connected() || testClient.available()) {
//    if (testClient.available()) {
//      Serial.write(testClient.read());
//    }
//  }
//
//  testClient.stop();
//}
void sendTextMessage(String message) // function used to send text to the bot 
{
  Serial.println("\nSending Telegram message...");

  if (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("WiFi not connected!");
    return;
  }

  Serial.print("WiFi OK. IP: ");
  Serial.println(WiFi.localIP());

  if (bot.sendMessage(chatId, message, "")) 
  {
    Serial.println("Telegram text message sent successfully!");
  } 
  else 
  {
    Serial.println("Failed to send Telegram text message.");
  }
  delay(500);
}
