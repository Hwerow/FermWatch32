#ifndef CONFIG_H
#define CONFIG_H
// this file has the configuration stuff from the touch screen and preferences

#include "touchCali.h" // has the preferences.h

// http://www.barth-dev.de/online/rgb565-color-picker/ // in getBFData.h
#define TFT_GREY 0x5AEB
#define lightblue 0x647B
#define pinkish 0xEBCA
#define darkred 0xA041
#define skyblue 0x373E
// #define blue 0x111D // was 0x5D9B
#define bronze 0xDBC3   // was 0xE5EC bit washed out
#define yeast 0xD676
#define gold 0xFEA0
#define cust 0xE99D
#define downDown 0x1BC0 // a darker green

// Button flags to set config variable that can be toggled
// default states all bool all can be used in FW set as global
bool brewF = false; // BPL used
bool FV = false;    // FV not set default = 1, set = 2
bool GM = false; // GM not set direct to FW32 

extern bool wmWIPE;    // needs to be extern for main to use?
//  UTC_offset select for 39600 AEDT  unselect 36000 AEST needs to be extern for main to use
// extern bool BF_Poll;  // BF update interval needs to be extern for main to use
bool Plato = false;   // SG used
bool Fahr = false;    // celcius used
bool Rec = false;      // set = use FW32 to receive bplfw from another FW32
bool Temp_Corr = false; // no temp correction removed from config menu
bool BF_Poll = false;  // 3 minutes default if set 10 minutes
bool save_OK = false; // save to preferences NVS and leave screen
bool DONE = false; // print config for debug and continue to startup screens or bypass if heap restarts esp32

void getConfigFromPreferences() {
  preferences.begin("config", true); // Open the preferences file in read-only mode

  // Retrieve configuration logic states from Preferences
  brewF = preferences.getBool("brewF", false);
  FV = preferences.getBool("FV", false);
  GM = preferences.getBool("GM", false); 
  // wmWIPE = preferences.getBool("wmWIPE", false); 
  Temp_Corr = preferences.getBool("Temp_Corr", false); 
  Plato = preferences.getBool("Plato", false);
  Fahr = preferences.getBool("Fahr", false);
  Rec = preferences.getBool("Rec", false); // rec
  BF_Poll = preferences.getBool("BF_Poll", false);
  // DONE = preferences.getBool("DONE", true);
  // DONE true to allow reset  

  preferences.end(); // Close Preferences

  // Print retrieved configuration details
  Serial.println("");
  Serial.println("Retrieved Configuration details:");
  Serial.print("brewF: "); Serial.println(brewF);
  Serial.print("FV: "); Serial.println(FV);
  Serial.print("GM: "); Serial.println(GM);
  Serial.print("wmWIPE "); Serial.println(wmWIPE);
  Serial.print("Temp_Corr: "); Serial.println(Temp_Corr);
  Serial.print("Plato: "); Serial.println(Plato);
  Serial.print("Fahr: "); Serial.println(Fahr);
  Serial.print("Rec: "); Serial.println(Rec); //rec
  Serial.print("BF_Poll: "); Serial.println(BF_Poll);
  //Serial.print("DONE: "); Serial.println(DONE);
  
}

void saveConfigToPreferences() {
    preferences.begin("config", false);
    
    // Save configuration parameters based on button selections or flags
    preferences.putBool("brewF", brewF);
    preferences.putBool("FV", FV);
    preferences.putBool("GM", GM); // 
    // preferences.putBool("wmWIPE", wmWIPE); dont save as would wipe every time
    preferences.putBool("Temp_Corr", Temp_Corr);
    preferences.putBool("Plato", Plato);
    preferences.putBool("Fahr", Fahr);
    preferences.putBool("Rec", Rec); // rec
    preferences.putBool("BF_Poll", BF_Poll);
    // preferences.putBool("DONE", DONE);
    preferences.end();
  }

// this is to wipe the preference data for testing purposes 
void clearPreferences() {
  Preferences preferences;
  preferences.begin("config", false); // Open preferences with the namespace "config"
  preferences.clear(); // Clear all preferences within this namespace
  preferences.end(); // Close preferences
  // for the wm param stuff
  preferences.begin("auth", false);
  preferences.clear();
  preferences.end(); 
  // for touch
  // preferences.begin("calibration", false); 
  // preferences.clear();
  // preferences.end(); 
}




#endif // CONFIG_H