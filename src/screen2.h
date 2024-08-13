#ifndef SCREEN2_H
#define SCREEN2_H

//Screen 2 has mainly BF data with pressure from BPL or if BF not used and BPL has an iSpindel/GM attached 
// or GravityMon selected 

#include "MultiMap.h"

#include "touchCali.h" // has the preferences.h
#include "config.h" // has the variable names and the logic flags
#include "getBFData.h" 

// for angle trend
#include <LittleFS.h>


// Batch 1 from BF
// extern float Bpressure; not displayed
// float pressure;  // only if manual from BF at the moment
// float iSp_temp;  // iSpindel 'aux' temperature
float iSp_sg;  // present SG from BPL in main or BF in getBFData?
float iSp_sga; // temperature 20C adjusted present SG
float Itemp;   // unadjusted iSpindel temperature
// float last_angle; // store the last angle to be able to compare in future?
float App_Att;
int16_t batch_No; // select this one for screen 2
int rssi; // BF rssi for iSpindel
float battery;
float angle;

// GM detection from input not tested
// bool GM  logic flag to be derived from GM button state is in config.h
bool GMused = false;  
bool gravmon = false; // was used in json and LittleFS wm config  

// Variables for GM gravity monitor 

extern String GMname; // 
extern String GMID; // gm ID
float gtemp;
float ggrav;
float gangle;
float gbatt;
int grssi;

// Variables from BPL
extern String Hydro; // "hydrometerName":"iSpindel"


// Function to determine if Hydro/iSpindel iSp_id name includes a colour
  String checkColours(String iSp_id) {
  String targets[] = {"BLUE", "RED", "GREEN", "WHITE", "ORANGE", "PCB"};

  for (int i = 0; i < 6; i++) {
    if (iSp_id.indexOf(targets[i]) != -1) {
      return targets[i];
    }
  }
  return "";
  }
  // function to change the iSp_id display colour
  void setDisplayColor(String colour) {
  if (colour == "BLUE") {
    // Set display to blue color
    Debugln("Setting display to blue.");
    ofr.setFontColor(TFT_BLUE, TFT_BLACK);
  } else if (colour == "RED") {
    // Set display to red color
    Debugln("Setting display to red.");
    ofr.setFontColor(TFT_RED, TFT_BLACK);
  } else if (colour == "GREEN") {
    // Set display to green color
    Debugln("Setting display to green.");
    ofr.setFontColor(TFT_GREEN, TFT_BLACK);
  } else if (colour == "WHITE") {
    // Set display to white color
    Debugln("Setting display to white.");
    ofr.setFontColor(TFT_WHITE, TFT_BLACK);
  } else if (colour == "ORANGE" ) {
    // Set display to orange color
    Debugln("Setting display to orange.");
    ofr.setFontColor(TFT_WHITE, TFT_BLACK);
  } else if (colour == "PCB") {
    // Set display to gold color
    Debugln("Setting display to Gold.");
    ofr.setFontColor(TFT_GOLD, TFT_BLACK);
  }
}

/* // Angle Trend ********************not working
// json to store data
#define AngleFile "/angle.json" // 
#define upTolerance 0.5      // Set the tolerance for the "up" trend
#define steadyTolerance 0.01  // Set the plus-minus tolerance for the "steady" trend
#define downTolerance 0.5  // Set the tolerance for the "down" trend
// #define NUM_READINGS 30       // Number of readings to average Now in getBFData
double averageDifference; // needs to be global

// Create a JSONDocument object for the angle data
StaticJsonDocument  <1024> angleData; 

// Angle Trend function Screen Display is in screen2
  void calculateAngleTrend(double currentAngle, bool& isFirstReading, double readings[], int& currentIndex, int& validReadings, double& previousAverage) {
  double average = 0.0;

  if (isFirstReading) {
    for (int i = 0; i < NUM_READINGS; i++) {
      readings[i] = currentAngle;
      validReadings++;
    }
    previousAverage = currentAngle;
    isFirstReading = false;
  }
  else {
    readings[currentIndex] = currentAngle;
    currentIndex = (currentIndex + 1) % NUM_READINGS;
    if (validReadings < NUM_READINGS) {
      validReadings++;
    }
    previousAverage = 0.0;
    for (int i = 0; i < validReadings; i++) {
      previousAverage += readings[i];
    }
    previousAverage /= validReadings;
  }

  for (int i = 0; i < validReadings; i++) {
    average += readings[i];
  }
  average /= validReadings;

  double averageDifference = average - previousAverage;
  Serial.println("");
  Serial.println("------------ Angle Trend --------------");
  Serial.print("previous average =  "); Serial.println(previousAverage, 4);
  Serial.print("average =  "); Serial.println(average, 4);
  Serial.print("average difference =  ");   Serial.println(averageDifference, 4);
  
  angleData["average"] = average;
  File file = LittleFS.open(AngleFile, "w");
  if (file) {
    serializeJson(angleData, file);
    file.close();
  }
  else {
    Serial.println("Failed to open angle file for writing");
  }

  Serial.print("Current angle: ");
  Serial.println(currentAngle, 3);
  Serial.print("-------------------------"); Serial.println("");
}
 */
//GM MQTT eg received in main
// {"name":"iSpindelblue", "ID":"389c58", "temperature": 29, "gravity":1.1094,"angle": 88.498, "battery":3.8, "rssi": -66}

// if not BF ie BPL only 
// BPL MQTT eg received in main 
// {"state":4,"beerTemp":21.69300079,"beerSet":20,"fridgeTemp":22.27300072,"fridgeSet":11.9119997,"mode":2,"pressure":0,
// "gravity":1.012,"plato":-0.0048,"voltage":3.700000048,"auxTemp":23.5,"tilt":25.43000031,"ispindelRssi":-64}

// iSp_sg from BPL


// screen 2 is mostly the iSpindel display from BF or BPL dependent on what option selected or direct from Gravity Monitor if GM set
// pluse derived functions App Att and ABV
void screen2() 
{
    
    // printf("iSp_id value in screen2: %s\n", iSp_id); // debug
    if(GM){
    drawColoredBorder(TFT_GREENYELLOW);
    } else if (BPLstale) {
    drawColoredBorder(TFT_ORANGE);
    }else{
    drawColoredBorder(getColorFromState(bpl_Data.state));
    }  

    if (Temp_Corr){ 
    Debug("");
    Debugln("SG adjusted for iSpindel/GM temperature");
  } else {
    Debug("");
    Debugln("No SG temp adjustment");
  }

  /* used for Debugging 
   if (brewF == false)
  {
    Debugln("");
    Debug(F("BPL only  "));
    // Debugln(batch_id);
    Serial.printf("iSpindel/GravityMonitor Name = ");
    Debugln(iSp_id);// Debugln(iSp_name);
    // Debug("    or /   ");
    // Debugln(GM_name);
    
       
    Debug("");
    Serial.printf("BPL Pressure =  %.1f PSI\n", Bpressure);
    Serial.printf("Present Gravity =  %.5f deg.\n", iSp_sg); // .4 decimal places
    Serial.printf("Battery  =  %.2f V\n", battery);
    Serial.printf("Angle    =  %.2f deg.\n", angle);
    Serial.printf("ITemp    =  %.1f C\n", Itemp);
    Serial.printf("BPL iSp rssi = %.2d dBm\n",rssi);
    Serial.println(F(""));

    
  } */
  
  // tft display mostly iSpindel and derived functions-----------------------------------------------------------------------------------------------------
  // wipe the screen innerbac and update
  // tft.fillScreen(TFT_VIOLET); //diag see if heating cooling etc is maintained
   tft.fillRoundRect(4, 4, 312, 232, 2, InnerBac); // clear startup/last screen
  
  
  //  Display brew name at the bottom left of the screen
    ofr.setFontColor(bronze,TFT_BLACK);
    ofr.setFontSize(BRW);
    ofr.setCursor(10,215);// was 160,215
    ofr.printf(brew.c_str());
    ofr.setCursor(220,215); // 
    // ofr.printf(WiFi.localIP().toString().c_str()); // option

   ofr.setFontColor(TFT_SILVER,TFT_BLACK);
   ofr.setFontSize(TXT);
   ofr.setCursor(12,5); // was 12, 12 now 12, 5
  
  
  // compare iSp connection status https://docs.arduino.cc/built-in-examples/strings/StringComparisonOperators
 
  // Option flags (switches)
  
  
  // manual indicates that a manual reading has been input to BF e.g. pressure reset of fields,  last values received 900s BF cycle
  // or the fermenting bach has been kegged, BF status changed to conditioning and the values are no longer valid 
  if (iSp_id == "manual")
  {
    Debugln("iSpindel not attached in BF or manual reading(s) entered!");
    ofr.setFontColor(TFT_RED,TFT_BLACK);
    ofr.setFontSize(TXT);
    ofr.setCursor(14,5);// was 12,5 and as below
    ofr.printf("Manual      ");   
      
  }

  // BF not used so BPL only and GM not set - allows GM via BPL if set that way
  if ((brewF == false) && (GM == false)) 
  {
    Debugln("BF not used so BPL only");
    ofr.setCursor(14,5);
    ofr.setFontColor(TFT_GREEN, TFT_BLACK);
    ofr.printf("BPL :  "); ofr.printf("%s", Hydro);// needs iSpindel name from BPL MQTT
    
    //ofr.printf("From BPL");  
    // blank out OG and FG
    tft.fillRect(10, 60, 300, 15, TFT_BLACK);
  
  }
  // If set to Receive  display message
  if ((brewF == false) && (Rec)){
    Debugln(" Received from FW32");
    ofr.setFontColor(TFT_GOLD, TFT_BLACK);
    // ofr.printf("From FW32");  // messes up Hydro
    // blank out OG and FG
    tft.fillRect(10, 60, 300, 20, TFT_BLACK);

  }
  // if GM used and no received name from gm yet  gm name changed from string to char
  /* if ((GM) && (brewF == false)){
    ofr.setCursor(14,5);
    ofr.setFontColor(TFT_BLUE, TFT_BLACK);
    ofr.printf("%c",gname);  
  } */
  /* {
    Debugln("GM update from MQTT not received yet");
    ofr.setCursor(14,5);
    ofr.setFontColor(TFT_YELLOW, TFT_BLACK);
    ofr.printf("GM Pending");  
  }else{
    ofr.setFontColor(TFT_BLUE, TFT_BLACK);
    ofr.printf(gname.c_str()); 
  } */
  // ie GM set and BF not used  
  if ((GM) && (brewF == false)) 
  {
    // allocate gmdata to display names
    angle = gangle;
    Itemp = gtemp;
    iSp_sg = ggrav;
    battery = gbatt;
    rssi = grssi;
    
    // display gm name
    ofr.setFontSize(TXT);
    ofr.setCursor(14,7);
    ofr.setFontColor(TFT_GREENYELLOW, TFT_BLACK);
    ofr.printf("GM :  ");
    ofr.printf(GMname.c_str());
    // Debug(""); Debug("screen2 GMname = ");Debugln(GMname);

  }
   
  // ie BF name used with no local GM 
  if (brewF == true) 
  {
    ofr.setFontSize(TXT);
    ofr.setCursor(14,7);
    
    // change display colour to suit found Hydrometer Colour
    String colourFound = checkColours(iSp_id);
    if (colourFound != ""){
    Debug("Found Colour: "); Debugln(colourFound);
    setDisplayColor(colourFound);
    }
    else
    {
    Debugln("No colour found");
    ofr.setFontColor(TFT_SILVER, TFT_BLACK);// default
    }
    
    ofr.printf("BF :  ");
    ofr.printf(iSp_id.c_str());
       
    // Serial.print(F("iSp_id =  ")); Serial.println(iSp_id); // debug
  }
  ofr.setFontColor(TFT_SILVER, TFT_BLACK);
  ofr.setFontSize(TXT);
  ofr.setCursor(12,35); // was 12,40
  ofr.printf("iSp RSSI");

  ofr.setCursor(80, 35);
  if (rssi <= -80) {
    // then red
    ofr.setFontColor(TFT_RED, TFT_BLACK);
    } else if (rssi <= -67) {
    // then yellow
    ofr.setFontColor(TFT_YELLOW, TFT_BLACK);
    } else {
    // then green
    ofr.setFontColor(TFT_GREEN, TFT_BLACK);
    }
  /* char rssiDisp[6]; // 5 numbers should be heaps incl minus sign
  itoa(rssi, rssiDisp, 10); //10 is the base i.e. decimal
  ofr.printf(rssiDisp);ofr.setFontColor(TFT_SILVER, TFT_BLACK);// ofr.printf(" dBm"); */
  if(rssi==0){
    // don't display as it might have come from bplfw when no iSpindel attached to BPL
  }else{
  ofr.printf("%d", rssi); // Print the RSSI directly
  }
  //Fixed text
  ofr.setCursor(35, 90); // was 40, 95
  ofr.setFontColor(TFT_SILVER, TFT_BLACK);
  ofr.printf("iSp Temp.\n");

  
  ofr.setCursor(145, 35); //was 165, 35 
  ofr.printf("iSp Battery\n");

  // Display iSpindel Voltage
  ofr.setFontSize(TXT);
  ofr.setCursor(240, 35);
   
   if (battery >= 3.80) {
    // then green
    ofr.setFontColor(TFT_GREEN, TFT_BLACK);
    } else if (battery >= 3.50 && battery < 3.80) {
    // then yellow
    ofr.setFontColor(TFT_YELLOW, TFT_BLACK);
    } else {
    // then red
    ofr.setFontColor(TFT_RED, TFT_BLACK);
    }
  
  char iVDisp[5];
    snprintf(iVDisp, sizeof(iVDisp), "%.2f", battery); // width of 5 and 2 decimal places
    ofr.printf(iVDisp);ofr.setFontColor(TFT_SILVER, TFT_BLACK);ofr.printf( " V\n");

  

  // App Att and ABV fixed text  AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
  ofr.setFontSize(TXT);
  ofr.setFontColor(TFT_SILVER, TFT_BLACK);
  ofr.setCursor(10,165);// 
  ofr.printf("App.Att. %% ");
  ofr.setCursor(37,192);
  ofr.printf("ABV %% ");
  
  // AA meetings etc 
  // Apparent Attenuation - appears on all screens 1 from BPL, 2 & 3  from BF Batch(es)
  ofr.setFontSize(50); // 
  ofr.setFontColor(TFT_GREEN, TFT_BLACK);
  ofr.setCursor(97, 155); // 
  
  if (iSp_sg == 0)
  { // if iSp_sg not received yet print blank  don't forget the ==
    
    ofr.printf("");
  }
  else if ((iSp_sg > 0 ) && (Temp_Corr == false) && (brewF)) // greater than 0  and no temp correction BF set
  {
    App_Att = (((SGogy - 1) - (iSp_sg - 1)) / (SGogy - 1)) * 100; // 40 - say 20  =20/40 = 50% * by 100
    bool neg_Val = App_Att < 0;                                   // trap negative numbers and force to zero
    if (neg_Val){
    App_Att = 0;  
    } 
    else if (App_Att >= 100){
    App_Att = 0;  
    } else{

      char aaDisp[5];
      snprintf(aaDisp, sizeof(aaDisp), "%.1f",App_Att);
      ofr.printf(aaDisp);
    
    Serial.println("");Serial.print(brew);Serial.printf(" Current Apparent Attenuation =  %.2f%s\n", App_Att, "%");
    }
  }

  // App Att corrected for iSp temperature
      
  if (iSp_sg == 0)
  { // if iSp_sg not received yet print blank  don't forget the ==
    ofr.printf("");
   }
  else if ((iSp_sg > 0) && (Temp_Corr == true))
  {
   // use iSp_sga for iSp_sg
    float App_Att_Corr = (((SGogy - 1) - (iSp_sga - 1)) / (SGogy - 1)) * 100; // using temp adjusted PG
    bool neg_Val = App_Att_Corr < 0;          // CYD                         // trap negative numbers and force to zero
    
    if (neg_Val || App_Att_Corr >= 100) {
    App_Att_Corr = 0;
    } else{

      char aacDisp[5];
      snprintf(aacDisp, sizeof(aacDisp), "%.1f",App_Att_Corr);
      ofr.printf(aacDisp);
        
    
    // testing only
    //Serial.printf("Current Temp Corr. Apparent Attenuation = %.2f%s\n", App_Att_Corr, "%");
    //Debugln("");
    } 
  }
  if(brewF)
  {
  // Calculate ABV that uses the UK Excise method that takes into account the alcohol
  // https://www.gov.uk/government/publications/excise-notice-226-beer-duty/excise-notice-226-beer-duty--2#calculation-strength
  // extract og and fg
  float og = SGogy;
  float fg = iSp_sg;
  float Init_abv; // first calc ABV then work out the fudge factor 
  Init_abv = ((og - fg) * 131.25); // nominal 131.25 bit high as 6% beers! seems to be what BF uses
  Debugln("");
  Serial.println(brew);Serial.printf(" Initial ABV *131.25 =  %.2f%s\n", Init_abv, "%"); 

  // Define the ABV ranges and their corresponding values
  float ABV_ranges[] = {0.0, 3.3, 4.6, 6.0, 7.5, 9.0, 10.5, 12.0};
  int ABV_values[] = {128, 129, 130, 131, 132, 133, 134};

  // Find the appropriate ABV_adjG value based on Init_abv changed i to n
  int ABV_adjG = 0;
  for (int n = 0; n < sizeof(ABV_ranges) / sizeof(ABV_ranges[0]); ++n) {
  if (Init_abv >= ABV_ranges[n] && Init_abv <= ABV_ranges[n + 1]) {
  ABV_adjG = ABV_values[n];
  // Debug("Fudge : ");Debug(ABV_adjG); // testing only
  break;
  }
  }
  
  // recalculate the ABV using the appropriate fudge factor 
  float abv = ((og - fg) * ABV_adjG);
  Serial.printf(" UK Adjusted ABV     =  %.2f%s\n", abv, "%");

  // alternative ABV Calc https://www.youtube.com/watch?v=RPfxf-6FcEg&t=6s
  // abv = ((76.08*(OG-FG)/(1.775-OG)) * (FG/0.794))
  float Altabv = ((76.08*(og-fg)/(1.775-og)) * (fg/0.794));
  Serial.printf(" Alternative ABV     =  %.2f%s\n", Altabv, "%");
  Serial.print(" Diff UK ABV to Alt ABV "); Serial.println(abv-Altabv);Serial.print("");

  /// ABV % display
    // check for a negative value perhaps when the iSp shows an increase in gravity at the start of fermentation
    bool neg_Val = abv < 0; 
    if (neg_Val) abv =0; // set to zero
    ofr.setFontColor(0xDBC3, TFT_BLACK); // was bronze bit anemic
    ofr.setFontSize(50);
    ofr.setCursor(97,180);
    char adjABVDisp[5];
    snprintf(adjABVDisp, sizeof(adjABVDisp), "%.2f",abv);
    ofr.printf(adjABVDisp);
      
  }
  // Blank app att and abv when BPL
  if (brewF == false){
    tft.fillRect(5, 160, 150, 60, TFT_BLACK);
  }
  
    
  //-Fixed Text ------------------------------------------------------------------------------------------------------------------
    ofr.setFontSize(TXT);
    ofr.setFontColor(TFT_SILVER, TFT_BLACK);
    ofr.setCursor(210, 90);
    ofr.printf("Angle\n");
    // ofr.setCursor(50, 155); //was 40
    // ofr.printf("PSI\n");
    ofr.setCursor(170, 155);//was 160
    ofr.printf("Present Grav.\n");
  
  // tft.fillRect(10, 180, 300, 40, TFT_BLACK); // background for custom font

  // iSpindel Temperature
  ofr.setFontSize(NUM);
  ofr.setFontColor(TFT_GOLD, TFT_BLACK);
  
  if (Fahr == true)
  {
    float iSpTemp = ((Itemp * 1.8) + 32.0);
    Serial.print(brew);Serial.printf(" iSpindel Temp  =  %.0f F\n", iSpTemp);
    ofr.setCursor(45,105);
    if (brewF == 0){
      ofr.printf("XX");
    }
    else { 
    char isptDisp[5];
    // whole numbers only
    snprintf(isptDisp, sizeof(isptDisp), "%.0f",iSpTemp);
    ofr.printf(isptDisp);   
    }
    
    ofr.setFontColor(TFT_SILVER, TFT_BLACK);
    ofr.setCursor(120,105); // was 128,105
    ofr.printf("F");
    ofr.setFontSize(TXT);
    ofr.setCursor(108,110); // was 110,110
    ofr.printf("o");
    ofr.setFontSize(NUM);
    
  }
  else if (Fahr == false)
  {
    Serial.print(brew);Serial.printf(" iSpindel Temp  =  %.1f C\n", Itemp);
    
    // tft.fillRect(10, 175, 300, 40, TFT_BLACK); // blanking bottom row DONE
    ofr.setCursor(15,105);
    if (Itemp < 10)
    {
     ofr.setCursor(30,105);
    }

    char itDisp[5];
    snprintf(itDisp, sizeof(itDisp), "%.1f",Itemp);
    ofr.printf(itDisp);   
    
    ofr.setFontColor(TFT_SILVER, TFT_BLACK);
    ofr.setCursor(130,105);
    ofr.printf("C");
    ofr.setFontSize(TXT);
    ofr.setCursor(117,110);
    ofr.printf("o"); 
    ofr.setFontSize(NUM);   
    
    
  }
  
  // iSpindel Angle
   ofr.setFontColor(TFT_RED, TFT_BLACK);
  ofr.setCursor(175,105);
   
  char anDisp[6];
  snprintf(anDisp, sizeof(anDisp), "%.2f",angle);
  ofr.printf(anDisp);
  ofr.setFontSize(TXT);
  ofr.setCursor(285,110);// was 290
  ofr.printf("o");
  ofr.setFontSize(NUM);
  
  // Angle trend Not working display inhibited -------------
  /*
  // display the trend
  if (averageDifference > upTolerance) {
    ofr.setFontSize(NUM);
    ofr.setCursor(295,105);
    ofr.setFontColor(TFT_RED,TFT_BLACK);
    ofr.printf("/"); 
    Serial.println("Angle Trend is UP / ");  
  }
  else if (averageDifference < -downTolerance) {
    ofr.setFontSize(NUM);
    ofr.setFontColor(TFT_GREEN,TFT_BLACK); // darker green
    ofr.setCursor(285,105);
    ofr.printf(" \\ "); 
    Serial.println("Angle Trend is Down \\ "); 
  }
  else if (abs(averageDifference) <= steadyTolerance) {
    ofr.setFontSize(NUM);
    ofr.setFontColor(TFT_SILVER,TFT_BLACK);
    ofr.setCursor(288,104);
    ofr.printf("--"); 
    Serial.println("Angle Trend is Steady -- "); 
  }
 */

  // Present Gravity   from iSpindel SG unadjusted by iSp temperature
  ofr.setFontColor(TFT_ORANGE, TFT_BLACK);
    
  
  if (Plato == true && Temp_Corr == false)
  {
    Debug("If Plato true ");    Debug(Plato); Debug("  & Temp_Corr false  ");Debugln(Temp_Corr);
    float iSpPlato = ((259 - (259 / iSp_sg)));
    Serial.print(brew);Serial.printf(" iSpindel/GM PG Plato  =  %.1f deg P\n", iSpPlato);
    
    if (iSpPlato < 10.0) { //  step to the right
      ofr.setCursor(190,165);
    }
    else {
    ofr.setCursor(170,165); // was 190,165
    }
    char ispPDisp[5];
    snprintf(ispPDisp, sizeof(ispPDisp), "%.1f",iSpPlato);
    ofr.printf(ispPDisp);
       
    
    ofr.setFontColor(TFT_LIGHTGREY, TFT_BLACK);
    ofr.setCursor(280,165); //260,165
    ofr.printf("P");
    ofr.setFontSize(TXT);
    ofr.setCursor(265,170); // 245,170
    ofr.printf("o");
    ofr.setFontSize(NUM);
    
  }
  else if (Plato == false && Temp_Corr == false)
  {
    Debugln("");
    Debug("Present Gravity  ");Debugln(brew);
    // Debug("Plato false ");Debug(Plato); Debug("  & Temp_Corr false  ");Debugln(Temp_Corr);
    Serial.print(brew);Serial.printf(" iSpindel/GM PG SG  =  %.5f deg\n", iSp_sg);
        
    ofr.setCursor(165,165);
    char issgDisp[7];
    snprintf(issgDisp, sizeof(issgDisp), "%.4f",iSp_sg);
    ofr.printf(issgDisp);
   
    
  }
  
  // Interpolation for temperature correction of present gravity ------------------------------------------------------------------------------
  // https://github.com/RobTillaart/MultiMap
  if (Temp_Corr == true)
  {
    float input[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0};
    float adj_Val[] = {-0.0016, -0.0017, -0.0017, -0.0018, -0.0018, -0.0018, -0.0017, -0.0017, -0.0016, -0.0016, -0.0015, -0.0014, -0.0013, -0.0012, -0.001, -0.0009, -0.0007, -0.0006, -0.0004, -0.0002, 0.0,
                       0.0002, 0.0004, 0.0007, 0.0009, 0.0012, 0.0014, 0.0017, 0.002, 0.0023, 0.0026, 0.0029, 0.0032, 0.0035, 0.0039, 0.0042, 0.0045, 0.0049, 0.0053, 0.0056, 0.006};
    {
      SG_adjT = multiMap<float>(Itemp, input, adj_Val, 41);

      
      iSp_sga = (iSp_sg + SG_adjT);
      Debugln("");
      Debug("iSp SG Temp correction 1 = true : "); Serial.println(Temp_Corr);
      Debug("Unadjusted SG = ");Serial.println(iSp_sg, 5);
      Debug("Correction : ");Serial.println(SG_adjT, 5);
      Debug("Adjusted SG = "); Serial.println(iSp_sga, 5);
      
      
    }
  }

  if ((Temp_Corr == true) && (Plato == false))
  {
    // display a small orange circle when temperature corr active
    tft.fillCircle(295, 166, 4, TFT_ORANGE);

    bool negVal = iSp_sga < 0; // trap negative values and force to zero 
    if (negVal || iSp_sga >=1.150 ){ 
    iSp_sga = 0;
    }else{
    // display adjusted SG
    ofr.setCursor(165, 165);
    char issgaDisp[7];
    snprintf(issgaDisp, sizeof(issgaDisp), "%.4f",iSp_sga);
    ofr.printf(issgaDisp);
    }
       
  }

  // for temperature adjusted Plato
  if (Temp_Corr == true && Plato == true)
  {
    Debug("Plato true "); Debug(Plato); Debug("  &  Temp_Corr true  "); Debugln(Temp_Corr);
    
    float iSpPlato = ((259 - (259 / iSp_sga))); // adjusted version
    
    bool negVal = iSpPlato < 0; // trap negative values and force to zero 
    if (negVal || iSpPlato >=30 ){ 
    iSpPlato = 0;
    }else{
    ofr.setFontColor(TFT_ORANGE, TFT_BLACK);
    ofr.setCursor(190,165);
    
    char iSpPDisp[5];
    snprintf(iSpPDisp, sizeof(iSpPDisp), "%.1f",iSpPlato);
    ofr.printf(iSpPDisp);
    }
    
    ofr.setFontColor(TFT_LIGHTGREY, TFT_BLACK);
    ofr.setCursor(280,165); // 260,165
    ofr.printf("P");   
    ofr.setCursor(265,180); // 245,180
    ofr.setFontSize(TXT);
    ofr.printf("o");   
    
    ofr.setFontSize(NUM);
    tft.fillCircle(295, 166, 4, TFT_ORANGE); // near present gravity with P
  }
  
  // OG  no background blanking as fixed values
  
  ofr.setFontSize(TXT);
  ofr.setCursor(12,60); // was ,55
  ofr.setFontColor(TFT_SILVER, TFT_BLACK);
  ofr.printf("OG ");
  ofr.setCursor(165,60);
  ofr.printf("Est. FG ");

  
  if (Plato == true)
  {
    float OGPlato = ((259 - (259 / SGogy)));
    // Debugln("");
    // Serial.printf("OG Plato  =  %.1f deg P\n", OGPlato);
    ofr.setCursor(50,60);
    ofr.setFontColor(TFT_GREEN, TFT_BLACK);
    // float to string for display
    char PogDisp[5];
    
    snprintf(PogDisp, sizeof(PogDisp), "%.1f",OGPlato);
    ofr.printf(PogDisp);
    ofr.setFontColor(TFT_SILVER, TFT_BLACK);
    ofr.printf(" P\n");
   
    
    float FGPlato = ((259 - (259 / FGEst)));
    ofr.setCursor(240,60);
    ofr.setFontColor(0xF502, TFT_BLACK);
    char PfgDisp[5];
    
    snprintf(PfgDisp, sizeof(PfgDisp), "%.1f",FGPlato);
    ofr.printf(PfgDisp);
    ofr.setFontColor(TFT_SILVER, TFT_BLACK);
    ofr.printf(" P\n");
      
  }
  else if (Plato == false)
  {
    // Debugln("");
    //Serial.printf("OG SG  =  %.5f deg\n", SGogy); //doubled up
    ofr.setCursor(50,60);
    ofr.setFontColor(TFT_GREEN, TFT_BLACK);
    // float to string for display
    char OGsgDisp[7];
    // width of 5 and 4 decimal places
    snprintf(OGsgDisp, sizeof(OGsgDisp), "%.5f", SGogy);
    ofr.printf(OGsgDisp);

    ofr.setCursor(240,60);
    ofr.setFontColor(0xF502, TFT_BLACK);
    char FGsgDisp[7];
    // width of 5 and 4 decimal places
    snprintf(FGsgDisp, sizeof(FGsgDisp), "%.5f", FGEst);
    ofr.printf(FGsgDisp);
    
    
  }
  if (brewF == false){
  // blank out OG and FG
  tft.fillRect(10, 60, 300, 20, TFT_BLACK);
  }
  

} // end screen2












#endif