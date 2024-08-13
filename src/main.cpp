/* 
                       FermWatch32 cyd2usb 

in setup
  first use CYD
launch FW title screen
  calibrate touch?
      yes 
launch calibrate
  do the calibration 
  save values in preferences   
  subsequent use 
use saved calibration details 

launch config menu screen shows parameters currently set
Change parameters?
    yes 
select by touching button set = highlighted in yellow; unset grey
once chosen touch OK to save and DONE to continue
saves parameters in preferences 
use parameters all bool in config.h and in the main code

Config screen Options
BF unset = BPL only
2FV unset single batch 2FV set = 2 batches needs BF set
wm_WIPE set wipes all the settings as a one off to enter new Auth_B etc
Temp_Corr set experimental temperature correction unset default
Fahr set = Fahrenheit unset default = Celcius
Plato set = Plato unset default = SG
Receiver set = Receiver unset well - not Receiver
BF_Poll set = 3 minute update interval unset 10 minute update
Must select OK to save the config to preferences ***** Important
DONE to continue

then in setup
run the wm parameter settings input this seems the optimal way to input the Auth_B batch details and save them
Auth_B based on BF = brewF set true run the wm parameter settings to get the batch details
Similarly enter the UTC_offset wherever you are 
Forwarding IP Address Forward Target_IP Broker_IP default is 192.168.0.100

save these to wm preferences 
use wm data

else skip BF and use bpl as the default sets up mqtt etc
positive action required to select BF and other settings

v 001
problem with gamma and screen inversion - seems to affect photos not much difference if text
I get normal colors by commenting out these two lines from ILI9341_Init.h:
writecommand(0xE0); //Set Gamma
writecommand(0XE1); //Set Gamma
but I commented out anyway!
created Button.h for the button stuff, setup options based on saved preferneces for FW

v 002
played about with the LED
created touchCali.h for the calibration stuff and config.h for the parameter stuff

v 003 clean version most comments removed from main.cpp 
  incorporated most of setup and wm from FW to test whole front end using prefs for Auth_B
  did not include the MQTT stuff as bombed out
  changed the wm flow to be per dronebot eg
  created getBFData.h  get the batch details  and get the batch updates 
  loop modified to complete the post setup actions for BF and DST 
 
v 004
  split out  screen 1 and 2 
  checked formatting Fahr and Plato
  added the bpl mqtt stuff but not the gm mqtt as likely gm would come via BF OR BPL
  check all the menu settings work Fahr, Plato done
  Can't solve the memory leak so when the heap gets to 70,000 a reset is initiated
  The heap reset  bypasses the config screen and auto operation of wm a mostly seamless operation
  A 'normal" power type reset launches the menu 
  If wm WIPE is selected it wipes the wifimanager settings and allows rentry of wifi details, Auth_B, etc 
  
  check stability
v 005  
  upgrade name to FW32
  fixed iSp_id to display made a String

New FW32 
v 0.1 
  Multiple Fermentation Vessels/multiple iSpindel/GravityMonitors
v 0.2
refined screen2 and 3 duplication of functions
moved App Att and abv for screen2 and 3 and pressure to screen1
v0.3
When BPL only ie brewF == false 
  - uses the  parts of screen 2 to display iSp
  - App Att and ABV not displayed or sent
v0.4
tested forwarding different IP addresses set from Config Menu
v0.5
Temp_Corr button used for Receive Temp_Corr logic retained but not used or tested
added option to subscribe to bplfw from a different FW32 Rec Receive Turns off bplfw from the receiving FW32
v0.6
Included the changes from modifying when set as receiver
UTC offset and Target Broker IP address now entered via wifimanager and saved in preferences
Added GM and Temp_Corr to menu
V0.7
one BPL and two fermenters - added sorting to GET batch lowest BatchNO being FV1 hopefully
Set up GravityMon MQTT - perhaps useful for calibration? Needs to be setup in gravmon https://github.com/mp-se/gravitymon/tree/v1.3.0 
See video for more detailed explanation 
V0.8
Fixed Temp_Corr for the various options
V0.9
cleaned up code
turned Debug off

v1.0
First Stable version

v1.1 
Update platform to 6.2.3
Added  angle trend indicator still doesn't work properly
v1.2
screen2  Made calculate the angle trend a function - so that it can be called from getBFDataupdates and the 10 min default timing 
added an alternative abv calculator - will stick with the UK Excise one for now
Updated to receive wireless hydrometer name from BPL note "hydrometerRssi" instead of ispindelRssi

v1.2.1 Changed to hard coded AEST this version set for 192.168.0.92 

v1.2.2 Angle trend not working properly. Display disabled in screen2.h
Was using ILI9341_2 Driver which worked. Changed to ST7789 Driver for a much crisper display
Colour coded dispaly to match iSpindel colour

v1.2.3 Auto Detect current timzone on startup/reboot using ip-api. Used sprite for clock to remove flickering.

First Release

 */


#include <Arduino.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include <TFT_eSPI.h>
#include "Math.h"
#include <HTTPClient.h> // auto detect TZ and Daylight  Saving Time Offset

// code split out hopefully for some clarity?
#include "Button.h" // Include the Button header with all the button logic
#include "touchCali.h"  // include the touch calibration
#include "config.h"     // has the bools
#include "getBFData.h"  // 2 functions get Batch ID etc and Batch Update
#include <LittleFS.h>
#include "screen1.h"
#include "screen2.h"
#include "screen3.h"
#include <TinyMqtt.h>
#include <WiFi.h>              //for ESP32
#include <WiFiClientSecure.h> // for BF API moved to getBFData
#include <NTPClient.h>
#include <WiFiUdp.h>

// IP API endpoint for setting the UTC Offset
const char* ipApiEndpoint = "http://ip-api.com/json/?fields=status,message,countryCode,city,timezone,offset,query";

/* #include "NotoSans_Bold.h" is now in getBFData.h
#include "OpenFontRender.h"
#define TTF_FONT NotoSans_Bold // The font is referenced with the array name look in the file!!
OpenFontRender ofr; */

#include <WiFiManager.h>
//#include <ArduinoJson.h>
// ------------------------------------------------------------------------------------

const char version[] = "1.2.3";

// ------------------------------------------------------------------------------------

// Define MQTT broker details
// https://github.com/hsaturn/TinyMqtt simple broker with wifi
// NB delete tinystring.cpp from TinyConsole to compile in Visual Studio Code

const char* Target_Broker = ""; //Tiny MQTT doesn't like the use of IPAddress
#define PORT 1883

MqttBroker broker(PORT);
MqttClient mqttbpl(&broker);
MqttClient mqttbplfw(&broker); //rec
MqttClient mqttgm(&broker); // 
// Initialize the MQTT client for the sender
MqttClient mqtt_sender(&broker);

// MQTT Topics set to
const char* topic = "bpl"; // latest version ESP32 with modified version of 4.3p  BrewPiLess was const char topic[] =
const char* topic2 = "bplfw"; // Forwarding - to differentiate from that data coming from bpl
const char* topic3 = "gm";   // GravityMonitor software on iSpindel hardware 


// unsigned long lastTopicTime = 0;
const unsigned long topicTimeout = 120000; // Timeout in milliseconds (120 seconds)
bool bplSource; // Flag to control the display state for bpl was false
const char *Source ="";//rec

// cyd LED pins
#define CYD_LED_RED 4
#define CYD_LED_GREEN 16
#define CYD_LED_BLUE 17

// custom hostname  The valid letters for hostname are a-z 0-9 and -  works
const char *hostname = "FW32";

// preferences used in preference to EEPROM which didn't work on ESP32 and used instead of the wm config json file
#include <Preferences.h>

/* this now in touchCali // Touch Screen pins cyd2usb touch uses HSPI pins not VSPI
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

SPIClass mySpi = SPIClass(VSPI);  // this was HSPI which was wrong
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ); */

// TFT_eSPI tft = TFT_eSPI(); // is now in Button.h

// chatGPT calibrate only once and save to Preferences. Check if values exist during setup if not calibrated 
// i.e. click on red cross hairs
// float xCalM = 0.0, xCalC = 0.0, yCalM = 0.0, yCalC = 0.0;

// these are used in touchCali and Button.h  set to extern
float xCalM = 0.0; 
float xCalC = 0.0; 
float yCalM = 0.0; 
float yCalC = 0.0;

// touch screen debounce
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 100; // Debounce time in milliseconds
bool buttonState = false; // Initial state of the button
bool lastButtonState = false; // Previous state of the button

// MQTT Forwarding set by default bplfw
char Target_IP [16] = ("192.168.0.100"); // 192.168.xxx.xxx 15 plus 1 = 16   IP 99 is FWO, 100 is FW32

int Sender_IP = 92; // needs to match the FW32 IP (WiFi.localIP() // automate?
 
long BF_updateInt = 180000;   // default value 3 mins

// automatic UTC_offset from detected IP address 
int UTC_offset = 36000; // default to suit how timeClient works for AEDT 39600 for AEST 36000

int getUTCOffset() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(ipApiEndpoint);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode)); // 200 is good
      Serial.println("Payload: " + payload);

      // Parse JSON response
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        http.end();
        return false;
        //return -1;
      }

      // Extract the UTC offset in seconds
      UTC_offset = doc["offset"].as<int>(); // offset  seconds
      Serial.print("Extracted UTC offset  "); Serial.println(UTC_offset);
      http.end();
      return true;
    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpResponseCode);
      http.end();
      return false;
      //return -1;
    }
  } else {
    Serial.println("Wi-Fi not connected");
    return false;
    // return -1;
  }
}


// bool DONE button used to move to the title screen sequence in config.h
bool configCompleted = false;

/* flags to set config variable that can be toggled in config.h
default states all bool all can be used in FW set as global
*/

// FW32
// JSON configuration file now changed to preferences 

// ------Default configuration values   this section is the WiFi Manager Parameter stuff------
// WiFi Manager Parameter - Global variables defaults
// length should be max size + 1 =126 if left blank BF not used - detect blank
char Auth_B[126] = (""); // testing for BF not connected and release
 
// flag for saving wm data
bool shouldSaveConfig = false; // false per 1.2 using https://dronebotworkshop.com/wifimanager/
bool forceConfig = false; // made global was in setup
 
// setup wifi and get BF API Auth_B
WiFiManager wm;

void saveAuthToPreferences() {
  Preferences preferences;
  preferences.begin("auth", false); // Open preferences with the namespace "auth"
  preferences.putString("Auth_B", Auth_B); // Save Auth_B to preferences with the key "Auth_B"
  preferences.end(); // Close preferences
}
void saveUTCToPreferences() {
  Preferences preferences;
  preferences.begin("UTC", false); 
  preferences.putLong("UTC_offset", UTC_offset); // Save UTC_offset to preferences with the key "UTC"
  preferences.end(); // Close preferences
}

void saveTargetIPToPreferences() {
  Preferences preferences;
  preferences.begin("Target_IP", false); // Open preferences with the namespace "auth"
  preferences.putString("Target_IP", Target_IP); // Save Target_IP to preferences with the key "Target_IP"
  preferences.end(); // Close preferences
}

bool retrieveAuthFromPreferences() {
  Preferences preferences;
  preferences.begin("auth", true); // Open preferences in read-only mode
  String retrievedAuth = preferences.getString("Auth_B", "");
  preferences.end();
  retrievedAuth.toCharArray(Auth_B, sizeof(Auth_B));
  return true;
}
bool retrieveUTCFromPreferences() {
  Preferences preferences;
  preferences.begin("UTC", true); // Open preferences in read-only mode
  long retrievedUTC = preferences.getLong("UTC_offset", 39600); // 39600 is the default value
  preferences.end();
  UTC_offset = retrievedUTC;
  
  // Debug statements to check the retrieved value
  Serial.print("Retrieved UTC from preferences: ");
  Serial.println(retrievedUTC);
  if (retrievedUTC != 39600) {
    UTC_offset = retrievedUTC;
    return true;
  } else {
    return false;
  }
  //return true;
}

bool retrieveTargetIPFromPreferences() {
  Preferences preferences;
  preferences.begin("Target_IP", true); // Open preferences in read-only mode
  String retrievedAuth = preferences.getString("Target_IP", "");
  preferences.end();
  retrievedAuth.toCharArray(Target_IP, sizeof(Target_IP));
  Target_Broker = Target_IP;
  return true;
}

void saveConfigCallback()
// Callback notifying us of the need to save configuration
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
 
void configModeCallback(WiFiManager *myWiFiManager)
// Called when config mode launched
{
  Serial.println("Entered Configuration Mode");
 
  Serial.print("Config SSID: ");
  Serial.println(myWiFiManager->getConfigPortalSSID());
 
  Serial.print("Config IP Address: ");
  Serial.println(WiFi.softAPIP());
}
//Global variable from Prefs to use in main
// bool Temp_Corr = false; // extern bool in config.h 
// bool BF_Poll;
bool wmWIPE = false; // ditto

// Get NTP Time and set the offset to UTC
// AEDT 39600 UTC+11:00  need to set offset for daylight saving time DST
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", UTC_offset);

// not tested 3 minutes is the default
// This function is to set the interval to send a GET request to BF  3 or 10 minutes
void setBF_PollInterval() {
  Serial.println("");
  if (BF_Poll) {
    Serial.printf("BF_Poll is true 3 minute interval\n");
    BF_updateInt = 180000; // 3 minutes
    // Serial.print("BF_Poll = "); Serial.println(BF_updateInt);
  } else {
    Serial.printf("BF_Poll is false 10 minute interval\n");
    BF_updateInt = 600000;  // 10 minutes
    
  }
  // Serial.print("BF_Poll = "); Serial.println(BF_updateInt);
 } 

// void setBF_PollInterval();

unsigned long currentTime; // for BF Update in loop 

// for the clock display
TFT_eSprite sprite = TFT_eSprite(&tft);

// Probably working  - variables from touch 
// ------------------------------------------------------------------------------------------------------------
/* Timing for LOOP functions NTP and accessing BF and screen refresh for  rotation  BPL set for 90 secs data may or may not have been updated yet from GM BPL BF
  "independant" timed events  https://www.programmingelectronics.com/arduino-millis-multiple-things/   */
const long eventTime_1_NTP = 1000;     // in ms 1 sec  DONE
const long eventTime_2_BF = BF_updateInt;
// const long eventTime_2_BF = 180000;     // in ms  1 min 1000 * 60  = 60000 180 = 3 mins note max 150 requests per hour changed to 60000 for testing
const long eventTime_3_GM = 10000; // gravmon update 10 secs for calibration not currently set up 
const long eventTime_4_mqtt = 40000;   // for mqtt sending 40 secs
/* When did they start the race? */
unsigned long previousTime_1 = 0; // NTP
unsigned long previousTime_2 = 0; // BF
unsigned long previousTime_3 = 0; // gravmon
unsigned long previousTime_4 = 0; // mqtt sending

// BF HTTPS requests First set up authorisation - login details to BF API
// WiFiClientSecure client; // secure moved to getBFData

// global variables for forwarding bpl data in the loop
byte state; 
float beerTemp;
float beerSet;
float fridgeTemp;
float fridgeSet;
float roomTemp;
byte mode;
// float pressure; // this from BF doesn't work
float gravity;
float plato;
String Hydro;

// GLOBAL Variables to save BF batch id and OG

// Batch 1
char batch_id[33]; // this works 29 char +1 [30] batch id now 31 -- 8 Dec 22 for 31 char used later in adding to strings made 33 just in case
String brew;       // changed to string to be able to count characters for display purposes
String Brewname;   // rename Batch
float SGogy;       // try float measured OG from BF
float FGEst;       // estimated final gravity from BF recipe actually FG
float abv;         // before adjustment is Init_abv local
int ABV_adjG;      // gravity adjustment for ABV calc display and sending
float adjABV;      // for sending and display
float SG_adjT;     // gravity adjustment for temperature correction
int batch1FV;      // FV number for Batch 1

// Batch 2
char batch_id2[33]; // this works 29 char +1 [30] batch id now 31 -- 8 Dec 22 for 31 char used later in adding to strings made 33 just in case
String brew2;       // changed to string to be able to count characters for display purposes
String Brewname2;   // rename Batch
float SGogy2;       // try float measured OG from BF
float FGEst2;       // estimated final gravity from BF recipe actually FG
float abv2;         // before adjustment is Init_abv local
int ABV_adjG2;      // gravity adjustment for ABV calc display and sending
float adjABV2;      // for sending and display
float SG_adjT2;     // gravity adjustment for temperature correction
int batch2FV;       // FV number for Batch 2

// GLOBAL variables for BPL not sure about these - only one would be coming from BPL
float Bpressure;
// float Bpressure2; // 

// last seen BPL for stale indications for BPL or more likely a wireless connection failure
// Start the timer for topic stale

unsigned long lastBPLMessageTime = 0;
const unsigned long BPLStaleTime = 120000; // 120 seconds in milliseconds to suit BPL when set for 90 secs
bool BPLstale = false;

// last seen BF // not done not needed?
unsigned long currentTime1;
unsigned long lastSeen1;
bool BFstale = false;

// iSpindel Global
String iSp_id ="";
String iSp_id2 ="";

// GM Global
String GMname;
String GMID;

// BPL global
// float iSp_sg;

// const char *iSp_id ="test iSp";
// printf("iSp_id value in main: %s\n", iSp_id);

// iSpindel GLOBAL Variables ex Brewfather to save  angle, id, manual/BPL entry pressure, temp, sg
// float iSp_angle; // current angle
// moved to getBFData.h
/* const char *iSp_id; // const char for printing to screen
String iSp_name; // iSpindel name BF
float pressure;  // manual from BF at the moment
// float iSp_temp;  // iSpindel 'aux' temperature
float iSp_sg;  // present SG
float iSp_sga; // temperature 20C adjusted present SG
float Itemp;   // unadjusted iSpindel temperature
// float last_angle; // store the last angle to be able to compare in future?
float App_Att;
int16_t batch_No; // 
int rssi; // BF rssi for iSpindel
float battery;
float angle; */

// http://www.barth-dev.de/online/rgb565-color-picker/ // now in config.h


// Function prototypes for each screen display
void switchScreen();
void screen1();
void screen2();
void screen3();  
// void screen4();

// TinyMQTT Broker ---------------------------------------------------------------------------
// TinyMQTT Broker bpl or bplfw topic ----------------------------------------------------------------------------

void onPublishA(const MqttClient * /* source */, const Topic &topic, const char *payload, size_t length /* length */) // added length
//{ Serial << "--> bpl " << topic.c_str() << ", " << bpldata << endl; Serial.println("");} // comment out when json used
{
  // debug to identify topic
 Serial.println("");
 Serial.print("Message received on topic: ");
 Serial.println(topic.c_str());

 // messages from both topics (topic/bpl and topic/bplfw)  go into the same data structure bpl_Data
 // Use the topic to determine the display actions
  if (strcmp(topic.c_str(), "bpl") == 0) {
    Source = "bpl";
    lastBPLMessageTime = millis(); // Update the time of the last bpl message
    Debug(" Received from ");Debugln(Source);
  }else if (strcmp(topic.c_str(), "bplfw") == 0){
   Source = "bplfw";
    lastBPLMessageTime = millis(); // Update the time of the last bpl message //rec
    Debug(" Received from ");Debugln(Source); 
  }
    
  // Clear previous data "zeroes out" the memory occupied by the bpl_Data variable or structure
  memset(&bpl_Data, 0, sizeof(bpl_Data));
      
  StaticJsonDocument<512> doc;   // was  384 increased to 512 with the iSp name
  DeserializationError error = deserializeJson(doc, (String)payload); // 
  if (error)
  {
    Serial.print(F("BPL deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  // Extract data from the JSON document
  bpl_Data.state = doc["state"];
  bpl_Data.beerTemp = doc["beerTemp"];
  bpl_Data.beerSet = doc["beerSet"];
  bpl_Data.fridgeTemp = doc["fridgeTemp"];
  bpl_Data.fridgeSet = doc["fridgeSet"];
  bpl_Data.roomTemp = doc["roomTemp"];
  bpl_Data.mode = doc["mode"];
  bpl_Data.pressure = doc["pressure"];
  bpl_Data.gravity = doc["gravity"]; // present gravity
  bpl_Data.plato = doc["plato"];
  bpl_Data.voltage = doc["voltage"];
  bpl_Data.auxTemp = doc["auxTemp"];// iSpindel or GM
  bpl_Data.tilt = doc["tilt"]; // angle
  bpl_Data.hydrometerRssi =doc["hydrometerRssi"]; // bpl iSp rssi 
  bpl_Data.Hydro =doc["hydrometerName"]; // need bpl iSpindel name

  // print out the received data for debugging
  Serial.println("bpl JSON:"); Serial.print("BPL was stale = 1  ");Serial.println(BPLstale);
  // serializeJsonPretty(doc, Serial);
  serializeJson(doc, Serial);
  
  // for BPL only used by screen2
  if (brewF == 0) {
  iSp_sg = bpl_Data.gravity;
  battery = bpl_Data.voltage;
  Itemp = bpl_Data.auxTemp;
  angle = bpl_Data.tilt;
  rssi = bpl_Data.hydrometerRssi; //ispindelRssi;   
  Hydro = bpl_Data.Hydro;
    } 
  
  // forward iSpindel/hyrometer name from BF
  if (brewF == 1){
  Hydro = iSp_id;
  }
  // Receive only if Rec Selected ie bplfw add in the derived functions AA and ABV
  if (Rec){
  App_Att = bpl_Data.AA; 
  adjABV = bpl_Data.ABV;
  rssi = bpl_Data.hydrometerRssi; // ispindelRssi; //rec 
  Hydro = bpl_Data.Hydro;
  // if no iSpindel attached to BPL causes rssi to display  0 ok
  // but when Rec is also using BF with an iSpindel attached to batch the rssi is shown until wiped by bplfw being 0
  }
}  

  /* // Added some BF data before sending on to the target broker  CHATGPT
  // Add new key-value pairs 
   bpl["tilt"] = angle; 
   bpl["auxTemp"] = Itemp;
   bpl["gravity"] = iSp_sg;
   bpl["voltage"] = battery;
   // add identifier of sender
   bpl["sender"] = sender; // 92
   serializeJson(bpl, serializedJson); // was pretty */
    
  // Debugln("Original bpl JSON plus some BF data:");
  // Debugln(serializedJson);


  // Ref only ----------------------------------------------------------------------------------------------------------
  // // error trap and message if BF batch_id not decoded
  // String batch_ln = String(batch_id);
  // if (batch_ln.length() == 0){
  // Debug("No Batch ID received -  ");
  // Debugln("Brewfather not connected");
  // tft.fillRect(4,155,310,62,TFT_BLACK);
  // tft.setCursor(60,180);
  // tft.setTextFont(1);
  // tft.setTextColor(TFT_RED,TFT_BLACK);
  // tft.print("BF NOT CONNECTED");
  // tft.setTextColor(TFT_SILVER,TFT_BLACK);
  // // return;
  // }
  
// TinyMQTT Broker GravityMonitor -------------------------------------------------------------------------------
void onPublishB(const MqttClient * /* source */, const Topic &topic3, const char *gmdata, size_t length /* length */) // added length
// { Serial << "--> gravitymon " << gm.c_str() << ", " << gmdata << endl; Serial.println(""); } // comment out when json used
{

  // if topic includes gm i.e gravmon = blue test iSpindel
  StaticJsonDocument<192> gmon; //  192 from arduino json assistant
  DeserializationError error1 = deserializeJson(gmon, (String)gmdata);
  if (error1)
  {
    Serial.print(F("GM deserializeJson() failed: "));
    Serial.println(error1.f_str());
    return;
  }
  // from arduinojson  assistant
  const char *gname = gmon["name"]; // "iSpindelblue"
  const char *gID = gmon["ID"];            // "389c58"
  // gtoken = gmon["token"];      // "gravmon"
  // int ginterval = gm["interval"]; // 900
  gtemp = gmon["temperature"]; // 28.5 "C"
  ggrav = gmon["gravity"]; // 1.1467
  gangle = gmon["angle"];  // 87.06
  gbatt = gmon["battery"]; // 3.78
  grssi = gmon["rssi"];    // -71

  /* Debugging
  Debugln(" ");Debugln("GravityMon data details ");
  Debug(F("Name:    ")); Serial.println(gname);
  Debug("GM ID:   ");Serial.println(gID);
  Serial.printf("GM temp =  %.1f C\n", gtemp);
  Serial.printf("GM SG =  %.5f deg.\n", ggrav); // .4 decimal places
  Serial.printf("GM Battery =  %.2f V\n", gbatt);
  Serial.printf("GM Angle   =  %.2f deg.\n", gangle);
  Serial.print("GM RSSI     = ");Serial.println(grssi);
  Debugln(""); */
  
  // see screen2 GM flags and display
  Debug("");Debugln("gm from Gravity Mon");
  serializeJson(gmon, Serial);
  // GM global
  GMname = gname;
  GMID = gID;

} // end of if topic gm


 // Global Screen variables // NB if you get this number wrong you will have too many OR too few screens shown!  
 byte numScreens = 2;

  

byte currentScreen = 0;  // Variable to store the current screen number
const int screenInterval =10000;  // 10 seconds
const int totalInterval = 90000;  // 90 seconds 
unsigned long previousMillis = 0;  



// Screen 1 BPL
// Screen 2 BF Batch 1
// Screen 3 BF Batch 2
// Screen 4 Configuration not used

// config screen Button instances  ie screen positioning and button names
// column 1 
Button button1(20, 40, 130, 25,  "    BF    ");
Button button2(20, 80, 130, 25,  "   2FV?   ");
Button button3(20, 120, 130, 25, "    GM    ");
Button button4(20, 160, 130, 25, "  wm WIPE");
Button button5(20, 200, 130, 25, " Temp_Corr ");
// buttons column 2
Button button6(170, 40, 130, 25,   "   Fahr  ");
Button button7(170, 80, 130, 25,   "  Plato ");
Button button8(170, 120, 130, 25,  " Receiver "); //rec
Button button9(170, 160, 130, 25,  "  BF_Poll  ");
Button button10(170, 200, 65, 25, " OK ");  // need a highlight when pressed
Button button11(245, 200, 65, 25, "DONE" ); //  Config Completed also button for testing - print config

// function to handle button interactions in the loop
void handleButtonInteraction(ScreenPoint sp, Button &button, bool &setting, const char* settingName) {
  if (button.isClicked(sp)) {
    if ((millis() - lastDebounceTime) > debounceDelay) {
      lastDebounceTime = millis();
      setting = !setting;// needed this
      
      buttonState = !buttonState;
      if (buttonState != lastButtonState) {
        lastButtonState = buttonState;
        setting = buttonState;
        Serial.print(settingName);
        Serial.print(": ");
        Serial.println(setting);
      }
    }
  }
} 


// screens
// screen 3 is pretty much a duplicate of screen 2 for a second batch/fermenter
// Define a function to handle screen switching:
// The switchScreen() function is called whenever the screen needs to be switched, 
// such as in the loop() function based on timing or external events. 
// It ensures that the correct screen function is called to update the display accordingly.

void switchScreen(int screenNumber) {
  // Perform actions based on the screenNumber value needs to match Number of screens set
    
 switch (screenNumber) {
    case 0:
      screen1();
      break;
    case 1:
      screen2();
      break;
    case 2:
      screen3();
      break;
    case 3:
      // screen4(); // spare
      break;
    default:
      // Handle invalid screenNumber value
      break;
  }
}
    /* // now in getBFData.h
    
    bool rstDONE;

    // set DONE to true Prefs to withstand reset in the heap code
    void SetrstDONETruePrefs() {
      rstDONE = true;
      Preferences preferences;
      preferences.begin("config", false);
      preferences.putBool("rstDONE", rstDONE);
      preferences.end();
      
    }
    // use in the in the post setup code in the loop
    bool getrstDONEPrefs() {
      Preferences preferences;
      preferences.begin("config", true); // Open preferences in read-only mode
      rstDONE = preferences.getBool("rstDONE", false);
      preferences.end();
      Serial.print("rstDONE: "); Serial.println(rstDONE);
      return true;
    }

   
    // Unsubtle -- but if the heap reduces to 70000 bytes restart the esp
    void checkFreeHeap() {
      size_t freeHeap = ESP.getFreeHeap();
      Serial.print("Free Heap: ");
      Serial.println(freeHeap);
      
      if (freeHeap < 80000) {
        Serial.println("Free heap is below 80,000 bytes. Blue led on.");
        digitalWrite(CYD_LED_BLUE, LOW); // LOW is on BLUE
        digitalWrite(CYD_LED_RED, HIGH);
        digitalWrite(CYD_LED_GREEN, HIGH);
      }
            
      // Check if free heap is below 70,000 bytes
      if (freeHeap < 70000) {
        // set rstDONE to true Prefs to withstand reset
        SetrstDONETruePrefs();
        Serial.println("Free heap is below 70,000 bytes. Initiating restart.");
        ESP.restart();
      }
      }
 */


//############################################################## setup
void setup() 
{
 // Initialize LittleFS for angle trend indicator future use SD Card?
  
  Serial.begin(115200);
  Serial.println(F(""));

  // anglefile not used
  /* if (!LittleFS.begin()) {
    Serial.println("Formatting LittleFS...");
    LittleFS.format();
    if (!LittleFS.begin()) {
      Serial.println("Failed to format LittleFS!");
      return;
    }
  }
  
  Serial.println("LittleFS initialized successfully.");

// Check if the angle file exists
  bool angleFileExists = LittleFS.exists(AngleFile);

// Load the previous angle value from the file, if it exists
  if (angleFileExists) {
    File file = LittleFS.open(AngleFile, "r");
    DeserializationError error = deserializeJson(angleData, file);
    if (error) {
      Serial.println("Failed to read angle file");
    }
    else {
      if (angleData.containsKey("average")) {
        previousAverage = angleData["average"];
        Serial.print("Previous average: ");
        Serial.println(previousAverage, 2);
      }
    }
    file.close();
  }
 */

 /* Ref only
 // Initialize Wi-Fi
  WiFi.begin();
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Obtain and print the MAC address for fixing IP address on the router
  uint8_t mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC Address: ");
  for (int i = 0; i < 6; ++i) {
    Serial.print(mac[i], HEX);
    if (i < 5) {
      Serial.print(':');
    }
  }
  Serial.println(); */

  // *****************************   SET TO false for auto operation   *************************************
  // needs to be here so you don't do touch cali every time 
  bool forceConfig = false; // 1.2.X true takes us to the AP  true after reset launched AP put back to false for normal operation and works automatically after restart
  bool prefsSetup = retrieveAuthFromPreferences(); retrieveUTCFromPreferences();retrieveTargetIPFromPreferences();
  if (!prefsSetup) 
  {
    Serial.println(F("Forcing config mode as there is no saved config"));
    forceConfig = true;
  }

  // set up hostname for the router 
  WiFi.hostname(hostname); // just before WIFI.mode etc works
  WiFi.mode(WIFI_STA);        // explicitly set mode, esp defaults to STA+AP
  
  pinMode(CYD_LED_RED, OUTPUT);
  pinMode(CYD_LED_GREEN, OUTPUT);
  pinMode(CYD_LED_BLUE, OUTPUT);
  
  // Initially, turn off all LEDs
  digitalWrite(CYD_LED_RED, HIGH);
  digitalWrite(CYD_LED_GREEN, HIGH);
  digitalWrite(CYD_LED_BLUE, HIGH);
  
  /* // Define CYAN color (R: 0, G: 100, B: 100)
  analogWrite(CYD_LED_RED, 255);     // Turn off red 255 ie HIGH
  analogWrite(CYD_LED_GREEN, 155);  // Adjust green intensity (100/255 = 39% ,  255* 39% =~100 ,Inversion = 255 - 100 = 155)
  analogWrite(CYD_LED_BLUE, 155);   // Adjust blue intensity ( ditto
   */
    
  // TFT Splash Screen
  tft.init();
  tft.setRotation(1);

  tft.fillScreen(TFT_ORANGE);
  tft.fillScreen(Backgnd);
  tft.fillRoundRect(4, 4, 312, 232, 2, InnerBac);
  
  // open font render setup
  ofr.setDrawer(tft); // Link drawing object to tft instance (so font will be rendered on TFT)
  
  // sprite for clock  
  sprite.createSprite(100,28); // could be size of screen here w by h
  ofr.setFontColor(TFT_SILVER,TFT_BLACK);

    // Load the font and check it can be read OK
    if (ofr.loadFont(TTF_FONT, sizeof(TTF_FONT))) {
      Serial.println(F("Initialise error"));
      return;
    }
  
  // Set the cursor to top left
  ofr.setCursor(12, 10);
  ofr.setFontSize(30); // set font size 
  ofr.printf("FermWatch32\n");

  ofr.setCursor(115, 160); // was 105 145 CYD
  ofr.setFontSize(40); // set font size 
  ofr.printf("version  ");
  ofr.printf(version);
  
  ofr.setFontColor(bronze, TFT_BLACK);
  ofr.setCursor(25, 110);
  ofr.setFontSize(80); // set font size
  ofr.printf("FermWatch32\n");
  delay(2000); // view screen

  // Yeast budding  A grey ellipse origin at (45, 80) with horizontal radius of 35, and vertical radius of 30
  tft.fillEllipse(45, 80, 35, 30, yeast); // mother
  delay(2500);
  tft.fillEllipse(100, 70, 20, 15, yeast); //  daughter

  ofr.setFontColor(TFT_SILVER, TFT_BLACK);
  ofr.setCursor(40, 190);
  ofr.setFontSize(TIT); // set font size
  ofr.printf("Look for FermWatch32_AP");

  delay(2000); 

  // Version
  Serial.println(F(""));
  Serial.print(F("This is FermWatch32 VERSION : ")); Serial.println(F(version));
  Debugln("");
  
  // ******************************************************************************************************* 
  // WiFiManager------------------------------------------------------------------------

  // Remove any previous network settings  for testing
  // wm.resetSettings(); // wipe settings every time REMEMBER for the PRODUCTION version to comment out! !!!!!!!!!!!!!!!!
  
  // set config save notify callback
  wm.setSaveConfigCallback(saveConfigCallback); // save the params
  // Set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wm.setAPCallback(configModeCallback);
  // set a 3 minute timeout so the ESP doesn't hang waiting to be configured, for instance after a power failure,
  wm.setConfigPortalTimeout(180); // timeout if blocked too long and then the autoconnect should return Do a power cycle whenever you want to reconfigure.

  // Text box (String) BF Base64 encoded Authorisation Basic
  WiFiManagerParameter custom_text_box("BFAuth_text", "Enter the Base64 BF Authorization key or leave blank if BF not used", Auth_B, 126); // 125 == max length 125+1
  
  /* // Text box (Number) for NTP Client UTC offset in seconds
  char convertedValue[6];
  sprintf(convertedValue, "%d", UTC_offset); // Need to convert to string to display a default value.
  WiFiManagerParameter custom_text_box_num("UTC_num", "UTC Offset time (seconds)", convertedValue, 6); // 6 == max length */
  
  // Text box (String) for Target IP address 
  WiFiManagerParameter custom_text_box2("Target_IP_text", "Enter Target IP Address", Target_IP, 16); // 16 == max length
  
  // WiFiManagerParameter custom_html("<br>"); // line break separator between the check boxes

  // add all your parameters here
  wm.addParameter(&custom_text_box); // Auth_B
  // wm.addParameter(&custom_text_box_num); // UTC_offset
  wm.addParameter(&custom_text_box2); // Target_IP
  
  // just show the wifi not the other options
  std::vector<const char *> menu = {"wifi"}; // only show WiFi on the menu not the other stuff
  wm.setMenu(menu);                          // custom menu, pass vector

  // set dark theme
  wm.setClass("invert");

  // set static ip  dns  IPAddress(192,168,0,1  if you have more than one FermWatch make sure you use different IP addresses!
  
  //-------------------------*** AP naming also needs to be unique to each FermWatch------------------------------------------------

  wm.setSTAStaticIPConfig(IPAddress(192, 168, 0, 92), IPAddress(192, 168, 0, 1), IPAddress(255, 255, 255, 0), IPAddress(192, 168, 0, 1)); // set static ip,gw,sn,dns
  wm.setShowStaticFields(true);                                                                                                           // force show static ip fields
  wm.setShowDnsFields(true);     
  
  if (forceConfig)
    // Run if we need a configuration
  {
    if (!wm.startConfigPortal("FERMWATCH32_AP", "")) // no pwd for testing
    {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
  }
  else
  {
    if (!wm.autoConnect("FERMWATCH32_AP", ""))
    {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      // if we still have not connected restart and try all over again
      ESP.restart();
      delay(5000);
    }
  }
    
  // If we get here, we are connected to the WiFi
  Debugln("");
  Debug("WiFi connected  ");
  Debug("IP address : ");
  Debugln(WiFi.localIP());
  
  // Get the UTC offset
  getUTCOffset();

  // Copy the string values
  strncpy(Auth_B, custom_text_box.getValue(), sizeof(Auth_B));
  Debug("BF Authorisation Basic Input: ");
  Debugln(Auth_B);  
      
  strncpy(Target_IP, custom_text_box2.getValue(),sizeof(Target_IP) );
  Debug("Forwarding Target IP Address: ");
  Debugln(Target_IP);
  Target_Broker = Target_IP;
  Debug("Target Broker :  ");Debugln(Target_Broker);

  /* // Convert the number values
  UTC_offset = atoi(custom_text_box_num.getValue());
  Debug("UTC Offset: ");
  Debugln(UTC_offset); */
    
  // Save the custom parameters to Preferences
  if (shouldSaveConfig)
  {
    saveAuthToPreferences();
    saveUTCToPreferences();
    saveTargetIPToPreferences();
  }
  
  // check
  retrieveAuthFromPreferences(); //to retrieve Auth_B
  Debug("Auth_B from Prefs  "); Debugln(Auth_B);
  
  retrieveUTCFromPreferences();
  Debug("UTC offset from Prefs  "); Debugln(UTC_offset);
  
  retrieveTargetIPFromPreferences();
  Debug("Forwarding Target IP from Prefs  "); Debugln(Target_IP);
  Debugln("");

  /* // check status of BF using whether the key was entered or not
  if (Auth_B[0] == 0)
  {
    (brewF == false); // false when using GM otherwise for BF enter the proper Authorization
    Debugln("");
    Debug("brewF false should = 0    ");
    Debug("so ");
    Debug(brewF); // brewF =1??
    Debugln("  Brewfather not set");
  } */
  
  // Purple combo here preferences retrieved
  digitalWrite(CYD_LED_RED, LOW);    // Turn on red
  digitalWrite(CYD_LED_GREEN, HIGH); // Turn off green
  digitalWrite(CYD_LED_BLUE, LOW);   // Turn on blue
  delay(2000); // view screen

  // WiFi Connected messages
  tft.fillScreen(TFT_ORANGE);
  tft.fillRoundRect(4, 4, 312, 232, 2, InnerBac);
  
  ofr.setFontSize(TIT); // set font size
  ofr.setCursor(20, 20);
  ofr.printf("WiFi Connected");
  ofr.setCursor(20, 45);// was 20,50
  ofr.printf("FermWatch IP  ");
  ofr.setFontColor(TFT_SILVER,TFT_BLACK);
  ofr.printf(WiFi.localIP().toString().c_str()); // https://forum.arduino.cc/t/how-to-manipulate-ipaddress-variables-convert-to-string/222693/15

  Debugln("");
  Debug("FermWatch IP  ");
  Debugln(WiFi.localIP().toString());

  // MQTT Start the broker ----------------------------------------------------------------
  // subscribe to topic = bpl, topic2 = bplfw, and topic3 = gm
  broker.begin(); //
  mqttbpl.setCallback(onPublishA);
  mqttbpl.subscribe(topic);
  mqttbplfw.setCallback(onPublishA);//rec
  mqttbplfw.subscribe(topic2);
  mqttgm.setCallback(onPublishB);
  mqttgm.subscribe(topic3);
  
  // Sender Subscribe to  topic2 bplfw
  mqtt_sender.subscribe(topic2);
    
  // Start the broker messages
  // Debugln("Starting MQTT broker");
  ofr.setFontColor(bronze, TFT_BLACK);
  ofr.setCursor(20, 70);
  ofr.printf("Starting FW MQTT broker...");
  ofr.setCursor(20, 110);
  ofr.printf("If no data shown check that");
  ofr.setCursor(20, 140);
  ofr.printf("the BPL/GM MQTT IP settings");
  ofr.setCursor(20, 170);
  ofr.printf("are correct or try resetting");
  ofr.setCursor(20, 200);
  ofr.printf("or prompting BPL/GM");
  
   Debugln("If no BPL data shown check BPL/GM MQTT IP Settings match FermWatch or try resetting BPL/GM");
  tft.setTextColor(TFT_SILVER, TFT_BLACK);

  delay(2000); // to allow the screen to be viewed

  // cyd2usb setup touch from
  // https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display/blob/main/Examples/Basics/2-TouchTest/2-TouchTest.ino
  // needs -DUSE_HSPI_PORT  in platformio.ini
  // Start the SPI for the touch screen and init the TS library
  mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin(mySpi);
  ts.setRotation(1);

  // Start the tft display and set it to black
  tft.init();
  tft.setRotation(1); //This is the display in landscape

  // Clear the screen before writing to it
  tft.fillScreen(TFT_BLACK);
  /* 
  //CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
  // Perform calibration routine
  // calibrateTouchScreen(); // Perform calibration uncomment for testing
  
  float xCalM = 0.09;  // Typical  calibration values
  float xCalC = -18.75;
  float yCalM = 0.07;
  float yCalC = -18.16; */

  // if already calibrated skip calibration steps  
  preferences.begin("calibration", false);

  if (checkCalibrationInPreferences()) {
    isCalibrated = preferences.getBool("isCalibrated", false); // Retrieve isCalibrated flag from Preferences
    Serial.print("Calibration found in Preferences 1 is true ");Serial.println(isCalibrated);
    useCalibrationData();
  // Print the stored data from Preferences
    printCalibrationData();

   } else {
    calibrateTouchScreen(); // Perform calibration
    saveCalibrationToPreferences(); // Save calibration and isCalibrated flag to Preferences
    Serial.println("Calibration performed and saved to Preferences");
    // small delay between cali and menu o prevent double click
    delay(200);
  }

  if (isCalibrated){
  // tft.fillScreen(TFT_BLACK);
  // tft.setTextColor(TFT_PURPLE,TFT_BLACK);
  // tft.setTextSize(2);
  // tft.setCursor (12,15);
  Serial.print(  "Calibrated");
  }
     
  // display the config screen
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW,TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor (12,15);
  tft.println("Select Boxes Touch OK to save and DONE to continue");
  
 // show the saved config state by highlighted buttons
      // Call function to retrieve configuration
      getConfigFromPreferences(); 
      // Render buttons based on retrieved settings
      button1.render(brewF); 
      button2.render(FV);  
      button3.render(GM);
      button4.render(wmWIPE); 
      button5.render(Temp_Corr); 
      button6.render(Fahr);
      button7.render(Plato);
      button8.render(Rec);//rec
      button9.render(BF_Poll);

    // renders blank buttons no status  for OK and DONE
    button10.render(tft);button11.render(tft);
      
  /*  Blank buttons for testing
   button1.render(tft);button2.render(tft); button3.render(tft);button4.render(tft);button5.render(tft);
  button6.render(tft); button7.render(tft); button8.render(tft);button9.render(tft);
  button10.render(tft);button11.render(tft); */
  
  // goto Loop to process other functions that were in setup  
  
} // end setup

unsigned long lastFrame = millis();

// LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOP

void loop() 
{ 
  // MQTT Broker ---------------------------------------------------------------------
    broker.loop();
    mqttgm.loop();  // gravmon
    mqttbpl.loop(); // BrewPiLess
    mqttbplfw.loop(); // rec bplfw when in Receiver mode
  
  
  unsigned long currentTime = millis(); // original needed for BF loop timing

  // Setup and Configuration Touch Screen stuff
  if (!configCompleted) 
  {     // heap initiated reset bypasses config screen
        getrstDONEPrefs();
        ScreenPoint sp;
        // Check if DONE button is pressed or rstDONE from prefs = true to cater for a heap initiated reset
        if ((button11.isClicked(sp)) || (rstDONE)) {
         // Debug(" rstDONE = 1 ? "); Debugln(rstDONE);
          getConfigFromPreferences(); // get the values                    
        // get the BF Batch details and run the screens before starting the main program screen cycles
                          
          if (brewF)
          {
            getBFBatch (); // get the batch ID(s)
            delay (3000); // view the screen
            updateBFData(); // get the Batch 1 details   rest of the updates in the loop later 
            if (FV)
            {
            updateBFData2(); // Batch 2 again flag to switch
            }
            
          }
          // Forwarding bplfw all the time except when Receiving
          
          // this is the IP address and port for the target mqtt broker 
          if(Rec == false)// if Rec is true prevent publishing bplfw from the receiver
          {
          mqtt_sender.connect(Target_Broker, PORT, 100);// try keep alive 50 secs increased to 100
          } else
          {
          //Receiving bplfw 
          Debug(" Receiving Source :  ");Debugln(Source);//rec
          Debugln("");  
          }
          
          // Get the UTC offset
          getUTCOffset(); 
          timeClient.begin();

          setBF_PollInterval(); // Call function to set BF GET updates not tested 
                  
          // reset rstDONE to false to cater for an ordinary reset
          SetrstDONEFalsePrefs();

        configCompleted = true;
        }
              // Other configuration logic
                // ScreenPoint sp;
              /* // limit frame rate
              while((millis() - lastFrame) < 20);
              lastFrame = millis();
              */
      if (ts.touched()) 
      {
        TS_Point p = ts.getPoint();
        sp = getScreenCoords(p.x, p.y);
        // call a function with 'sp' and relevant button and settings info
        handleButtonInteraction(sp, button1, brewF, "brewF");
        handleButtonInteraction(sp, button2, FV, "FV");
        handleButtonInteraction(sp, button3, GM, "GM");
        // handleButtonInteraction(sp, button4, wmWIPE, "WIPE"); // not saved
        handleButtonInteraction(sp, button5, Temp_Corr, "Temp_Corr");
        handleButtonInteraction(sp, button6, Fahr, "Fahr");
        handleButtonInteraction(sp, button7, Plato, "Plato");
        handleButtonInteraction(sp, button8, Rec, "Receive");
        handleButtonInteraction(sp, button9, BF_Poll, "BF_Poll");
        
        // these are different use buttons
        // handleButtonInteraction(sp, button10, save_OK, "OK");
        // handleButtonInteraction(sp, button11, PS, "DONE");

        // WIPE All Settings
        if (button4.isClicked(sp)) {
          // If the button is touched, initiate debounce logic
          if ((millis() - lastDebounceTime) > debounceDelay) {
            // Capture the current time for debounce comparison
            lastDebounceTime = millis();
        // Toggle the button state
            buttonState = !buttonState;
            
            // Check if the state has changed
            if (buttonState != lastButtonState) {
              lastButtonState = buttonState; // Update the last state
              
              // Update wmWIPE based on the buttonState
              wmWIPE = buttonState;
              // saveConfigToPreferences(); ddon't save as intended to be a 'one off'
              // clearPreferences(); // wipe auth, config and calibration
              wm.resetSettings(); 
              Debug("WM WIPE ");
              Debugln(wmWIPE);
            }
          }
        }  

              
        // OK to save 
        if (button10.isClicked(sp)) {
          // If the button is touched, initiate debounce logic
          if ((millis() - lastDebounceTime) > debounceDelay) {
            // Capture the current time for debounce comparison
            lastDebounceTime = millis();
            
            // Toggle the button state
            buttonState = !buttonState;
            
            // Check if the state has changed
            if (buttonState != lastButtonState) {
              lastButtonState = buttonState; // Update the last state
              
              // Update OK based on the buttonState
              save_OK = buttonState;
              saveConfigToPreferences();

              // Print the current state (for testing)
              Debugln("Configuration saved!");
              //getConfigFromPreferences(); // Call function to retrieve configuration
              Debug("OK: ");
              Debugln(OK);
            }
          }
        }  
        // and DONE to continue also debug print config 
        if (button11.isClicked(sp)) 
        {
          // If the button is touched, initiate debounce logic
          if ((millis() - lastDebounceTime) > debounceDelay) {
            // Capture the current time for debounce comparison
            lastDebounceTime = millis();
            
            // Toggle the button state
            buttonState = !buttonState;
            
            // Check if the state has changed
            if (buttonState != lastButtonState) {
              lastButtonState = buttonState; // Update the last state
              // Print the current state (for testing)
              // Update based on the buttonState
              DONE = buttonState;
           
                      
              Debugln("Configuration Details");
              // Update  based on the buttonState
              getConfigFromPreferences(); // Call function to retrieve configuration
              Debug("DONE  ");  // debug print
              Debugln(DONE);
            }
        }  
          
      }    
          
          
          // Render the buttons based on their current states
          button1.setPressed(brewF);
          button1.render(tft);
          button2.setPressed(FV);
          button2.render(tft);
          button3.setPressed(GM);
          button3.render(tft);
          button4.setPressed(wmWIPE);
          button4.render(tft);
          button5.setPressed(Temp_Corr);
          button5.render(tft);
          button6.setPressed(Fahr);
          button6.render(tft);
          button7.setPressed(Plato);
          button7.render(tft);
          button8.setPressed(Rec);//rec
          button8.render(tft);
          button9.setPressed(BF_Poll);
          button9.render(tft);
          button10.setPressed(save_OK); // was just OK
          button10.render(tft);
          button11.setPressed(DONE);
          button11.render(tft);

      

      } // end touch and setup
  
     
  } else 
  { // Now run main program logic
        
      // CLOCK   Refresh NTP Time every 1 sec -----------------------------------------------------------------------------
      // https://github.com/RalphBacon/BB5-Moving-to-a-State-Machine/blob/main/Sketches/3-NonBlocking.ino
      
      
      {
        static unsigned long NTPMillis = millis();
        if (millis() - NTPMillis >= 1000) // 1 sec
        {
          
          // tft.fillScreen(TFT_BLACK);   // clear touch screen// test only
          timeClient.update();
          sprite.fillSprite(TFT_BLACK);
          ofr.setFontColor(TFT_SILVER, TFT_BLACK);
          // tft.fillRect(214, 5, 100, 30,TFT_BLACK);// not needed
          // ofr.setFontSize(40);//about text 2 // old way
          // ofr.setCursor(214, 5);//old way
          // ofr.printf(timeClient.getFormattedTime().c_str()); // old way
          sprite.drawString(timeClient.getFormattedTime(),2,2,4); // time, x,y,font was = 1 -  2 a bit small 4 bit big but no flicker
          sprite.pushSprite(200,7); // co-ords for clock was 214,9
          NTPMillis = millis();     //  reset the local millis variable
        }
      }

    

    // BPL off not sending/publishing Stale flags
    unsigned long currentTime = millis();
    if (currentTime - lastBPLMessageTime > BPLStaleTime) {
      // No bpl message received within the specified time, set BPLstale to true
      BPLstale = true;
      
    } else {
      BPLstale = false;
    }
    
    // When FW32 not acting as a Receiver - reconnection logic when not connected to remote broker
    if ((Rec == false) && (not mqtt_sender.connected())) {
    mqtt_sender.connect(Target_Broker, PORT, 60);// try keep alive 50 secs increased to 60   
    }

 
    mqtt_sender.loop();

    // ============= client A bplfw publish - Forwarding ================
    {
      static const int interval = 40000;  // publishes every 40s (don't use delay()) Hard coded
      static uint32_t timer = millis() + interval;
      //topic2 is bplfw
    
      if (millis() > timer)
      {
        static int counter = 0;
        // Serial << "Sender is publishing " << topic_sender.c_str() << endl;
        timer += interval;// set to 40 secs

        // Debug(state);Debug("   state to be sent by bplfw");//rec
        // created here so that the data in the json doesn't aggregate over time sending mutiple messages
        // updated to 384
        StaticJsonDocument<384> doc;
        
        doc["state"] = state;
        doc["beerTemp"] = beerTemp;
        doc["beerSet"] = beerSet;
        doc["fridgeTemp"] = fridgeTemp; 
        doc["fridgeSet"] = fridgeSet;
        doc["roomTemp"] = roomTemp;
        doc["mode"] = mode;   
        doc["pressure"] = Bpressure;
        
        // iSpindel via BrewFather
        doc["tilt"] = angle; 
        doc["auxTemp"] = Itemp;
        doc["gravity"] = iSp_sg;
        doc["voltage"] = battery;
        // add identifier of sender
        doc["sender"] = Sender_IP; // 92 should match the FW32 IP hard coded
        // FW32 calculated values 
        doc["AA"]  = App_Att;
        doc["ABV"] = adjABV; // after correction per uk tax office
        doc["hydrometerRssi"] = rssi;// rssi = bpl_Data.ispindelRssi  rec "hydrometerRssi"
        doc["hydrometerName"] = Hydro;
        // Serialize the JSON document left as string as in loop to send
        String serializedJson;
        serializeJson(doc, serializedJson);
              

        //Rec inhibit sending from receiving own FW32 data
        if(Rec){
        Debug("Receiver sending inhibited should be 1 = "); Debugln(Rec);
        }
        else{
        // non JSON mqtt_sender.publish(topic_sender, "sent by Sender, message #"+std::string(String(counter++).c_str()));
        mqtt_sender.publish(topic2, serializedJson.c_str()); // send it - the bpl plus BF json data 
        Serial.println("");
        Serial.println("Forwarding bplfw JSON >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
        Serial.println(serializedJson);//debug
        }
      }
    }

      // Get the current time 
      unsigned long currentMillis = millis(); 
      // Check if it's time to switch screens
      if (currentMillis - previousMillis >= screenInterval) 
      {
        previousMillis = currentMillis;
        
        // If GM is true, only display screen2
        if (GM) {
        screen2();
        } else {
        // Change numScreens based on the value of FV ChatGPT Q
        numScreens = (FV) ? 3 : 2; // If FV is true, numScreens is 3; otherwise, it's 2
        
        // Increment the current screen index
        currentScreen++;
        if (currentScreen >= numScreens) {
          currentScreen = 0;
        }
        
        // Switch to the next screen
        switchScreen(currentScreen); // 
      
      }     
      }
      // red LED ON if brewF = False
      if(brewF ==false)
      {
            // turn on the RED LED CYD
            digitalWrite(CYD_LED_BLUE, HIGH);
            digitalWrite(CYD_LED_RED, LOW);
            digitalWrite(CYD_LED_GREEN, HIGH);
      }

      if (brewF == true)
      { //turned off to save the battery!    
            digitalWrite(CYD_LED_BLUE, HIGH);
            digitalWrite(CYD_LED_RED, HIGH);
            digitalWrite(CYD_LED_GREEN, HIGH); 
      }
      if (GM)
      { // turn ON the GREEN LED CYD
            digitalWrite(CYD_LED_BLUE, HIGH);
            digitalWrite(CYD_LED_RED, HIGH);
            digitalWrite(CYD_LED_GREEN, LOW); 
      }
      // BF Update  
      if (currentTime - previousTime_2 >= eventTime_2_BF) {
          if(brewF){      // only if selected
          updateBFData(); // Call the Brewfather logic at the set interval BF_Poll
          }
          // printf("iSp_id value in main loop : %s\n", iSp_id); // debug 

          // need a flag if no second batch
          if (FV){
          updateBFData2();
          }
          if (!brewF){
          // BPLforscreen2();
          }
          previousTime_2 = currentTime; // Update the timing
       }  
      
           

  }
      checkFreeHeap(); // monitor the state of the heap
} // end LOOP


/* // To create purple (mixing red and blue):
  digitalWrite(CYD_LED_RED, LOW);    // Turn on red
  digitalWrite(CYD_LED_GREEN, HIGH); // Turn off green
  digitalWrite(CYD_LED_BLUE, LOW);   // Turn on blue
  delay(5000);
  
  // To create yellow (mixing red and green):
  digitalWrite(CYD_LED_RED, LOW);    // Turn on red
  digitalWrite(CYD_LED_GREEN, LOW);  // Turn on green
  digitalWrite(CYD_LED_BLUE, HIGH);  // Turn off blue

  delay(5000); */
  
/*  
For example, if you want to achieve an intensity of 100 out of 255 (which represents approximately 39% intensity), 
you'd need to calculate its inverted value within the 0-255 range. In this case:
Intensity = 100
Inverted intensity = 255 - Intensity
Inverted intensity = 255 - 100 = 155

This calculation gives you the value to set for the LED with active low logic to achieve an intensity equivalent to 100 
in the 0-255 range.
  */
