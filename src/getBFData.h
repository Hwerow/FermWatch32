#ifndef GETBATCH_H
#define GETBATCH_H

// This file has the details to get the BF Batch details and updates including the colours etc for convenience


#include "touchCali.h" // has the preferences.h
#include "config.h" // has the variable names
#include <WiFiClientSecure.h> // to access the BF API
#include <ArduinoJson.h>
#include "NotoSans_Bold.h"
#include "OpenFontRender.h"
#include <map>

#define TTF_FONT NotoSans_Bold // The font is referenced with the array name look in the file!!
OpenFontRender ofr;

// ---------------------------------------------------------------------------------------------------------
// Debugger https://github.com/RalphBacon/224-Superior-Serial.print-statements/tree/main/Simple_Example
#define Debugger 1 // turned off for release

#if Debugger == 1
#define Debug(x) Serial.print(x)
#define Debugln(x) Serial.println(x)
#define Debugf(x) Serial.printf(x) // throws errors not used
#else
#define Debug(x)
#define Debugln(x)
#define Debugf(x)
#endif
// cyd LED pins
#define CYD_LED_RED 4
#define CYD_LED_GREEN 16
#define CYD_LED_BLUE 17

// BPL states and other border colours
int16_t Backgnd = 0xFEA0; // gold
int16_t InnerBac = TFT_BLACK;
int16_t WaitHeat = pinkish;
int16_t Heating = TFT_RED;
int16_t WaitCool = skyblue;
int16_t Cooling = TFT_BLUE;
int16_t WaitPeak = TFT_GREY;
int16_t Idle = TFT_GREY; // was int32
int16_t OFF = downDown;
int16_t Unknown = TFT_ORANGE;
byte lastState; // global variable bpl mode

// for ofr font sizes 
constexpr byte NUM = 85; // for number font height  was 90
constexpr byte TXT = 30; // for fixed text was 38 was 35 CYD
constexpr byte TIT = 40; // title text messages was 50 then 45 now 40
constexpr byte CLK = 40; // for clock display
constexpr byte BRW = 30; // for bottom of screen info

// BF HTTPS requests First set up authorisation - login details to BF API
WiFiClientSecure client; // 

// GLOBAL Variables to save BF batch id and OG made extern as are in main
// const char* batch_id;  // go back to char
// Batch 1
extern char batch_id[33]; // this works 29 char +1 [30] batch id now 31 -- 8 Dec 22 for 31 char used later in adding to strings mde 33 just in case
extern String brew;       // changed to string to be able to count characters for display purposes
extern String Brewname;   // change Batch to something else
extern float SGogy;       // try float measured OG from BF
extern float FGEst;       // estimated final gravity from BF
extern float abv;         // before adjustment is Init_abv local
extern int ABV_adjG;      // gravity adjustment for ABV calc display and sending
extern float adjABV;      // for sending and display
extern float SG_adjT;     // gravity adjustment for temperature correction

extern int batch1FV;

//  iSpindel GLOBAL Variables ex Brewfather to save  angle, id, manual/BPL entry pressure, temp, sg
// float iSp_angle; // current angle
// extern char iSp_id[21];
//extern const char *iSp_id; // const char for printing to screen2
extern String iSp_id;

//extern float pressure;  // manual from BF at the moment
// float iSp_temp;  // iSpindel 'aux' temperature
extern float iSp_sg;  // present SG
extern float iSp_sga; // temperature 20C adjusted present SG
extern float Itemp;   // unadjusted iSpindel temperature
// float last_angle; // store the last angle to be able to compare in future?
extern float App_Att;
extern int16_t batch_No; // 
extern int rssi; // BF rssi for iSpindel
extern float battery;
extern float angle;

//FW32 Batch 2
extern char batch_id2[33]; // this works 29 char +1 [30] batch id now 31 -- 8 Dec 22 for 31 char used later in adding to strings mde 33 just in case
extern String brew2;       // changed to string to be able to count characters for display purposes
extern String Brewname2;   // change Batch to something else
extern float SGogy2;       // try float measured OG from BF
extern float FGEst2;       // estimated final gravity from BF
extern float abv2;         // before adjustment is Init_abv local
extern int ABV_adjG2;      // gravity adjustment for ABV calc display and sending
extern float adjABV2;      // for sending and display
extern float SG_adjT2;     // gravity adjustment for temperature correction

extern int batch2FV;

//  iSpindel GLOBAL Variables ex Brewfather to save  angle, id, manual/BPL entry pressure, temp, sg
// float iSp_angle; // current angle
// extern char iSp_id[21];
//extern const char *iSp_id; // const char for printing to screen2
extern String iSp_id2;

// extern float pressure2;  // manual from BF at the moment
// float iSp_temp;  // iSpindel 'aux' temperature
extern float iSp_sg2;  // present SG
extern float iSp_sga2; // temperature 20C adjusted present SG
extern float Itemp2;   // unadjusted iSpindel temperature
// float last_angle; // store the last angle to be able to compare in future?
extern float App_Att2;
extern int16_t batch_No2; // 
extern int rssi2; // BF rssi for iSpindel
extern float battery2;
extern float angle2;

extern char Auth_B[126];

// struct to store all the received measurements data
struct Measurement {
    const char* text;
    int value;
};
// Function to check if a string contains a substring used for only FV in texts
bool containsSubstring(const char* str, const char* substr) {
     return strstr(str, substr) != nullptr;
}

/* // angle trend variable defintion  see updateBFData below timing default 10 mins used in screen2 -------
#define NUM_READINGS 10       // Number of readings to average was 30
double previousAverage = 0.0; // Previous angle value
double readings[NUM_READINGS];
int currentIndex = 0;
int validReadings = 0; // Number of valid readings
bool isFirstReading = true;
// Function declaration for angle trend
void calculateAngleTrend(double currentAngle, bool& isFirstReading, double readings[], int& currentIndex, int& validReadings, double& previousAverage);

 */

// resets
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
      // Serial.print("rstDONE: "); Serial.println(rstDONE); // debug only
      return true;
    }

    // reset the rstDONE to false in Setup so that a normal reset will launch the config screen
    void SetrstDONEFalsePrefs() {
      rstDONE = false;
      Preferences preferences;
      preferences.begin("config", false);
      preferences.putBool("rstDONE", rstDONE);
      preferences.end();
      
    }
  
    // Unsubtle -- but if the heap reduces to 70000 bytes restart the esp
    // blue LED starts to flash
    void checkFreeHeap() {
      size_t freeHeap = ESP.getFreeHeap();
      /* Serial.print("Free Heap: ");
      Serial.println(freeHeap); */ //Debug
      
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


// Get data from the Brewfather API   mostly code from
  // https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFiClientSecure/examples/WiFiClientInsecure/WiFiClientInsecure.ino
  // and for the JSON
  // https://github.com/witnessmenow/arduino-sample-api-request/blob/master/ESP8266/HTTP_GET_JSON/HTTP_GET_JSON.ino
  // and  should not be in setup so that the batch ID can be available elsewhere?? not sure what was meant here

    // The idea is to skip this whole section if BF not used
    
   // for getting BF Data

void getBFBatch (){ 
  //   if (brewF == true)
  //  {

      Debug("Auth_B from  getBFBatch    ");Debugln(Auth_B);
      Debug("BF API stuff status 0 not set ");
      Debugln(brewF);
      //************************** latest Batch updates  **************************************************
      Debugln("\nStarting connection to server...");
      client.setInsecure(); // the magic line, use with caution??? no certificate
      if (!client.connect("api.brewfather.app", 443))
      {
        Debugln("BF Connection Failed!");
      }
      /* else if (brewF == true)  // qqq
      {
        Debugln("Connected to BF API !"); */

        // Send HTTP request using the html from Postman converted using  https://davidjwatts.com/youtube/esp8266/esp-convertHTM.html
        // base 64 encoded ASCII to Linux URLsafe encoding   https://www.base64encode.org/ suggest paste result in the line below
        //

        // Make a HTTPS request: NB using fgEstimated does not cater for when FG is Fixed Final Gravity so use fg instead
        // client.print("GET https://api.brewfather.app/v2/batches/?include=measuredOg,recipe.fg&status=Fermenting HTTP/1.1\r\n"); // made 1.1 and V2
         client.print("GET https://api.brewfather.app/v2/batches/?include=measuredOg,recipe.fg,measurements&order_by=batchNo&order_by_direction=desc&status=Fermenting HTTP/1.1\r\n");
        client.print("Host: api.brewfather.app\r\n"); // additional \r\n
        // changed GET to have batches sorted by batchNo the lowest number being FV1 hopefully

        // Construct the Authorization header directly
        client.print("Authorization: Basic ");
        client.println(Auth_B);

        client.println("Connection: close\r\n\r\n");
        client.println();

        while (client.connected())
        {
          String line = client.readStringUntil('\n');
          if (line == "\r")
          {
            // Debugln("Authorised. Batch headers received");
            break;
          }
        }
          // // Debug To check if there are incoming bytes available
          // // from the server, read them and print them: this lot under needs commenting out so the Json will work and vice versa
          //   while (client.available()) {
          //     char c = client.read();
          //     Serial.write(c); // to print
          //     // save possibly String buffer =Serial.readString();
          //   }
          //   //client.stop(); moved down
          //   // }

          // Use the ArduinoJson Assistant to calculate this:  https://arduinojson.org/v6/assistant/
          //  used stream input code
          //  size_t inputLength;

          //FW32
          StaticJsonDocument<176> filter;

          JsonObject filter_0 = filter.createNestedObject();
          filter_0["_id"] = true;
          filter_0["batchNo"] = true;
          filter_0["measuredOg"] = true;

          JsonObject filter_0_recipe = filter_0.createNestedObject("recipe");
          filter_0_recipe["name"] = true;
          filter_0_recipe["fg"] = true;

          JsonObject filter_0_measurements_0 = filter_0["measurements"].createNestedObject();
          filter_0_measurements_0["text"] = true;
          filter_0_measurements_0["value"] = true;
        
        
        StaticJsonDocument<768> doc;
        // change arduino assistant from input to client
        DeserializationError error = deserializeJson(doc, client, DeserializationOption::Filter(filter)); 

        if (error)
        {
          Serial.print(F("deserializeJson() Brew failed: "));
          Debugln(error.f_str());
          Debug("");
          tft.fillScreen(TFT_ORANGE);// Orange error surrround
          tft.fillRoundRect(4, 4, 312, 232, 2, InnerBac); // clear startup/last screen
          
          ofr.setFontColor(TFT_RED, TFT_BLACK);
          ofr.setFontSize(TIT); // set font size
          ofr.setCursor(20, 40);
          ofr.printf("Failed to Connect to BF\n");
          ofr.setCursor(20, 70);
          ofr.printf("Check Auth Key entered\n");
          ofr.setCursor(20, 100);
          ofr.printf("properly or wait a bit and\n");
          ofr.setCursor(20, 130);
          ofr.printf("FW will reboot directly\n");
          ofr.setCursor(20, 160);
          ofr.printf("      ");
                
                   
          return;
      }
      
      // Define a map to associate FV values with their names for each batch ChatGPT
      std::map<int, std::string> FV_Name_Map[2]; // Two separate maps for root_0 [0] and root_1 [1]


      // BF data -----------------------------------------------------------------------------
      // FW32
           
      //  Batch 1 ------------------------------
      if (FV) //ie true 2 batches 222222222222222222222222222
      
      {
      JsonObject root_0 = doc[0];
      const char* root_0_id = root_0["_id"]; // ""
      strcpy(batch_id, root_0_id); // use copy string to get the batch ID into the global variable batch_id
      int root_0_batchNo = root_0["batchNo"]; // 411
      float root_0_measuredOg = root_0["measuredOg"]; // 1.039

      for (JsonObject root_0_measurement : root_0["measurements"].as<JsonArray>()) {

      const char* root_0_measurement_text = root_0_measurement["text"]; // "Tap", "FV"
      int root_0_measurement_value = root_0_measurement["value"]; // 1, 1
      
      // Check if the text is "FV" and save the value
        if (strcmp(root_0_measurement_text, "FV") == 0) {
        // Save the value global
        batch1FV = root_0_measurement_value;
        }
      }

      float root_0_recipe_fg = root_0["recipe"]["fg"]; // 1.009
      const char* root_0_recipe_name = root_0["recipe"]["name"]; // "2024 Courage Best 2.0"
      const char* root_0_measurements_0_text = root_0["measurements"][0]["text"]; // "FV" was [1] 
      int root_0_measurements_0_value = root_0["measurements"][0]["value"]; // 2
      
      brew = root_0_recipe_name;
      // Debugln(brew);
      batch_No = root_0_batchNo;
      SGogy = root_0_measuredOg;
      FGEst = root_0_recipe_fg;

      // Check if the text is "FV" and save the value
      if (strcmp(root_0_measurements_0_text, "FV") == 0) {
      // Save the value global
      FV_Name_Map[0][root_0_measurements_0_value] = std::string(root_0_recipe_name);
      
      batch1FV = root_0_measurements_0_value;
      
      }        

             
      // Display the associated brew name for the received FV value
      // Serial.print("Brew Name for FV");
      // Serial.print(batch1FV);
      // Serial.print(" : ");
      if (FV_Name_Map[0].find(batch1FV) != FV_Name_Map[0].end()) {
      Serial.println(FV_Name_Map[0][batch1FV].c_str());
      } else {
      Serial.println("Brew name not found for this FV value.");
      }
       
      

      
      // Batch 2 --------------------------------
      JsonObject root_1 = doc[1];
      const char* root_1_id = root_1["_id"]; // ""
      strcpy(batch_id2, root_1_id); // use copy string to get the batch ID into the global variable batch_id2
      int root_1_batchNo = root_1["batchNo"]; // 443
      float root_1_measuredOg = root_1["measuredOg"]; // 1.050

      float root_1_recipe_fg = root_1["recipe"]["fg"]; // 1.009
      const char* root_1_recipe_name = root_1["recipe"]["name"]; // "2024 Test Batch 2"
      const char* root_1_measurements_1_text = root_1["measurements"][0]["text"]; // "FV" 
      int root_1_measurements_1_value = root_1["measurements"][0]["value"]; // 2

      brew2 = root_1_recipe_name;
      // Debugln(brew2);
      batch_No2 = root_1_batchNo;
      SGogy2 = root_1_measuredOg;
      FGEst2 = root_1_recipe_fg;
           
      // 
      for (JsonObject root_1_measurement : root_1["measurements"].as<JsonArray>()) {

        const char* root_1_measurement_text = root_1_measurement["text"]; // "Tap", "FV"
        int root_1_measurement_value = root_1_measurement["value"]; // 0, 1
        // Check if the text is "FV" and save the value
        if (strcmp(root_1_measurement_text, "FV") == 0) {
        // Save the value global
        batch2FV = root_1_measurement_value;
       }
          
      }
      // Check if the text is "FV" and save the value
      if (strcmp(root_1_measurements_1_text, "FV") == 0) {
      // Save the value global
      FV_Name_Map[1][root_1_measurements_1_value] = std::string(root_1_recipe_name);
      
      batch2FV = root_1_measurements_1_value;
      
      }   
      
      FV_Name_Map[1][batch2FV] = std::string(root_1_recipe_name);
      // Display the associated brew name for the received FV value
      // Serial.print("Brew Name for FV");
      // Serial.print(batch2FV);
      // Serial.print(" : ");
      if (FV_Name_Map[1].find(batch2FV) != FV_Name_Map[1].end()) {
      Serial.println(FV_Name_Map[1][batch2FV].c_str());
      } else {
      Serial.println("Brew name not found for this FV value.");
      }
      

      }
      else // just a single batch 11111111111111111111111111111
      {
      JsonObject root_0 = doc[0];
      const char* root_0_id = root_0["_id"]; // ""
      strcpy(batch_id, root_0_id); // use copy string to get the batch ID into the global variable batch_id
      int root_0_batchNo = root_0["batchNo"]; // 666
      float root_0_measuredOg = root_0["measuredOg"]; // 1.046

      const char* root_0_measurements_0_text = root_0["measurements"][0]["text"]; // "FV"
      int root_0_measurements_0_value = root_0["measurements"][0]["value"]; // 2
      float root_0_recipe_fg = root_0["recipe"]["fg"]; // 1.009
      const char* root_0_recipe_name = root_0["recipe"]["name"]; // "".
      
      brew = root_0_recipe_name;
      // Debugln(brew);
      batch_No = root_0_batchNo;
      SGogy = root_0_measuredOg;
      FGEst = root_0_recipe_fg;

          
      batch1FV = root_0_measurements_0_value;
      // Debug("FV # ");Debugln(batch1FV);
      
      // Display the associated brew name for the received FV value
      // Serial.print("Brew Name for FV");
      // Serial.print(batch1FV);
      // Serial.print(" : ");
      // if (FV_Name_Map[0].find(batch1FV) != FV_Name_Map[0].end()) {
      // Serial.println(FV_Name_Map[0][batch1FV].c_str());
      //} else {
      // Serial.println(brew);
      //}
      // }
           
      
      } // end FV selection

      client.stop(); // matches previous
     

      // } // end of skip BF section 

      // // Extract values for Debugging if needed
      //   // Debugln(F("From Brewfather API:"));
      //   Serial.printf("Batch ID  = %s\n", batch_id);
      //   // Debug("Status:   ");
      //   // Debugln(root_0_status);
      //   // Debug("Batch No:  ");
      //   // Debugln(batch_No);
      //   // Serial.printf("Brew  = %s\n", brew);

      /* if (Plato == true)
      {
        float OGPlato = ((259 - (259 / SGogy)));
        Serial.printf("OG Plato  =  %.1f deg P\n", OGPlato);
        //tft.setCursor(20, 200);
        //tft.print("Plato Set"); // tft.println(OGPlato,1);// not sure if this is working
      }
      else if (Plato == false)
      {
        Debugln("");
        Serial.printf("BF OG SG  =  %.4f deg\n", SGogy);
        Serial.printf("BF FG Est. SG  =  %.4f deg\n", FGEst);
        Debugln("");
      } */

  
      //if (brewF == true)
      //  {
      //Debug("BF status : ");
      //Debugln(brewF);

      tft.fillScreen(TFT_ORANGE);
      tft.fillRoundRect(4, 4, 312, 232, 2, InnerBac); // clear startup/last screen
      ofr.setFontSize(TIT); // set font size
      ofr.drawString("Connected to ...   ", 60, 30, bronze, TFT_BLACK);
      ofr.drawString("Brewfather API", 60, 60, bronze, TFT_BLACK);
      ofr.drawString("Waiting for batch  ", 60, 90, bronze, TFT_BLACK);
      ofr.drawString("to update ...", 60, 120, bronze, TFT_BLACK);
      
      /* Not used
      //Batch 1
      // convert int batch number to a char for display purposes
      ofr.setCursor(160,130);
      char batchNoDisp[6]; // 5 numbers should be heaps
      itoa(batch_No, batchNoDisp, 10); //10 is the base i.e. decimal
      ofr.setFontColor(TFT_SILVER,TFT_BLACK);
      ofr.printf(batchNoDisp);
      
      // Batch 2
      // convert int batch number to a char for display purposes
      ofr.setCursor(210,130);
      char batchNoDisp2[6]; // 5 numbers should be heaps
      itoa(batch_No2, batchNoDisp2, 10); //10 is the base i.e. decimal
      ofr.setFontColor(TFT_SILVER,TFT_BLACK);
      ofr.printf(batchNoDisp2); */
      
      // 28 character limit for BF Name
      // this is definitely getting tricky ChatGPT wise
      // Search for FV1 in both maps
      // Search for FV1 and FV2 names in both root_0 and root_1
      ofr.setFontColor(TFT_SILVER, TFT_BLACK);
      ofr.setCursor(7, 130);
      ofr.printf("FV");
      
      if (FV){
      std::string fv1_name = "";
      std::string fv2_name = "";
      
      if (FV_Name_Map[0].count(1) > 0) {
        fv1_name = FV_Name_Map[0][1];
      } else if (FV_Name_Map[1].count(1) > 0) {
        fv1_name = FV_Name_Map[1][1];
      }

      // Search for FV2 in both maps
      if (FV_Name_Map[0].count(2) > 0) {
        fv2_name = FV_Name_Map[0][2];
      } else if (FV_Name_Map[1].count(2) > 0) {
        fv2_name = FV_Name_Map[1][2];
      }
      
      // Batch 1      
      ofr.setFontSize(40); // set font size
      ofr.setCursor(10, 160);
      Debug("FV1 ");
      ofr.printf("1 "); 
      
      Serial.println(fv1_name.c_str());
      ofr.printf(fv1_name.c_str());
      
      // Batch 2      
      ofr.setCursor(10, 190);
      Debug("FV2 ");
      ofr.printf("2 ");
      
      Serial.println(fv2_name.c_str());
      ofr.printf(fv2_name.c_str());
      }
      else  //single batch
      {
      ofr.setFontSize(40);
      ofr.setFontColor(TFT_SILVER, TFT_BLACK);
      ofr.setCursor(10, 160);
      Debug("FV1 ");
      ofr.printf("1 "); 
      
      Serial.println(brew.c_str());
      ofr.printf(brew.c_str());    

      }
      /* // variable brew name not used
      if (brew.length() > 31) // greater than 26 characters print small with tft or 31 with ofr
      {
        ofr.setCursor(35, 160); // was left just 10,180 try 160
        ofr.setFontSize(30);// try 30
        ofr.printf(brew.c_str());// c_str() to be able to print the converted string to screen 

      }
      else if (brew.length() <= 30) // 39 character limit before wrapping
      {
        ofr.setCursor(35, 160); //was 12
        ofr.setFontSize(40);// 
        ofr.printf(brew.c_str());
        
      }
      
      if (brew2.length() > 31) // greater than 26 characters print small with tft or 31 with ofr
      {
        ofr.setCursor(35, 190); // was left just 10,180
        ofr.setFontSize(30);// try 30
        ofr.printf(brew2.c_str());// c_str() to be able to print the converted string to screen 

      }
      else if (brew2.length() <= 30) // 39 character limit before wrapping
      {
        ofr.setCursor(35, 190); //was 12
        ofr.setFontSize(40);// 
        ofr.printf(brew2.c_str());
        
      }
            */
      
      delay(3000);     // was delay(3000); 
    
  //}                     
}


// this lot is called in the loop to get the updates when BF used
void updateBFData() 
{
      // Debug("BF status Green LED used, Red LED not used: "); //CYD
      // Debugln(brewF);

      //********************************from ChatGPT disaggregated strings
      // Debugln("\nStarting connection to server...");
      client.setInsecure(); // Use with caution, as it disables certificate validation

      if (!client.connect("api.brewfather.app", 443))
      {
      // Debugln("Connection to BF Batch Failed!"); // Likely due to an internet failure
      }
      else
      {
      //Debugln("Connecting to BF Batch");

      //Debug("Batch ID   ");
      //Debugln(batch_id); // Global variable

      // Construct the request string directly
      client.print("GET https://api.brewfather.app/v2/batches/"); // was V1 V2 now
      client.print(batch_id);
      client.println("/readings/last HTTP/1.1");  // was 1.0
      client.println("Host: api.brewfather.app");

      // Construct the Authorization header
      client.print("Authorization: Basic ");  // was A1
      client.println(Auth_B);

      client.println("Connection: close");
      client.println();

        while (client.connected())
        {
            String line = client.readStringUntil('\n');
            if (line == "\r")
            {
               // Debugln(" Latest Batch readings received timing per BF_Poll interval");
                break;
            }
        }
      
    
        /* // for Debugging
        // if there are incoming bytes available from the server, read them and print them:
        //  this lot under needs commenting out so the Json will work
          while (client.available()) {
            char c = client.read();
            Serial.write(c); // to print
            // save possibly String buffer =Serial.readString(); //?
          }
        Serial.print(batch_id);//Debug */
        
        // stale BF test   ####################################### pssibly
                
        //
        StaticJsonDocument<320> batch; // changed from the calculated 192 to 256 to get it to work try 320 
        DeserializationError error = deserializeJson(batch, client);
          
        // added error messages likely due to not entering the proper Authorisation string
        if (error)
        {
          Debug(F("deserializeJson() BF Batch failed: "));
          Debugln(error.f_str());
        }  
        // Remote reporting from BF --------------------------------------------------------------------------------
        // NB any float int etc used here makes it local not GLOBAL
        // const char* comment = batch["comment"]; // nullptr
        // 
        // Debugln("BF batch details BF_Poll check ************************************");
        // pressure = batch["pressure"]; // 4 does not work
        // long long time = batch["time"]; // 1644817884597
        rssi = batch["rssi"];   // -69 was int rssi
        String BiSp_id_id = batch["id"];   // "ISPINDELPCBSG" was local string changed iSp_id to global const char* 
        angle = batch["angle"]; // 29.20479 was float angle
        // const char* type = batch["type"]; // "iSpindel"
        battery = batch["battery"]; // 3.889468 was float battery
        iSp_sg = batch["sg"];       // 1.0088  removed the float from the front
        Itemp = batch["temp"];      // 4.1
        client.stop(); // matches previous
        
        iSp_id = BiSp_id_id;
        
        /* // Debug("iSp_id from batch  ");Debugln(iSp_id);
        // function for screen2 angle trend not in use
        double currentAngle = angle; // Retrieve the current angle 
        calculateAngleTrend(currentAngle, isFirstReading, readings, currentIndex, validReadings, previousAverage);
 */
      }

 

}   
void updateBFData2() 
{
      // Debug("BF status Green LED used, Red LED not used: "); //CYD
      // Debugln(brewF);

      //********************************from ChatGPT disagregated strings
      // Debugln("\nStarting connection to server...");
      client.setInsecure(); // Use with caution, as it disables certificate validation

      if (!client.connect("api.brewfather.app", 443))
      {
      // Debugln("Connection to BF Batch Failed!"); // Likely due to an internet failure
      }
      else
      {
      //Debugln("Connecting to BF Batch");

      //Debug("Batch ID   ");
      //Debugln(batch_id); // Global variable

      // Construct the request string directly
      client.print("GET https://api.brewfather.app/v2/batches/"); // was S1 V2 now
      client.print(batch_id2);
      client.println("/readings/last HTTP/1.1");  // was S2
      client.println("Host: api.brewfather.app");

      // Construct the Authorization header
      client.print("Authorization: Basic ");  // was A1
      client.println(Auth_B);

      client.println("Connection: close");
      client.println();

        while (client.connected())
        {
            String line = client.readStringUntil('\n');
            if (line == "\r")
            {
               // Debugln(" Latest Batch readings received timing per BF_Poll interval");
                break;
            }
        }
      
    
        /* // for Debugging
        // if there are incoming bytes available from the server, read them and print them:
        //  this lot under needs commenting out so the Json will work
          while (client.available()) {
            char c = client.read();
            Serial.write(c); // to print
            // save possibly String buffer =Serial.readString(); //?
          }
        Serial.print(batch_id);//Debug */
        
        // stale BF test   ####################################### possibly
                
        //
        StaticJsonDocument<320> batch2; // changed from the calculated 192 to 256 to get it to work try 320 
        DeserializationError error = deserializeJson(batch2, client);
          
        // added error messages likely due to not entering the proper Authorisation string
        if (error)
        {
          Debug(F("deserializeJson() BF Batch failed: "));
          Debugln(error.f_str());
        }  
        // Remote reporting from BF --------------------------------------------------------------------------------
        // NB any float int etc used here makes it local not GLOBAL
        // const char* comment = batch["comment"]; // nullptr
        // 
        // Debugln("BF batch details BF_Poll check ************************************");
        // pressure = batch["pressure"]; // 4 does not work
        // long long time = batch["time"]; // 1644817884597
        rssi2 = batch2["rssi"];   // -69 was int rssi
        String BiSp_id_id = batch2["id"];   // "ISPINDELPCBSG" was local string changed iSp_id to global const char* 
        angle2 = batch2["angle"]; // 29.20479 was float angle
        // const char* type = batch["type"]; // "iSpindel"
        battery2 = batch2["battery"]; // 3.889468 was float battery
        iSp_sg2 = batch2["sg"];       // 1.0088  removed the float from the front
        Itemp2 = batch2["temp"];      // 4.1
        client.stop(); // matches previous
        
        iSp_id2 = BiSp_id_id;
        
        // Debug("iSp_id from batch  ");Debugln(iSp_id);
      }

 

}   
#endif