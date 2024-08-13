#ifndef SCREEN1_H
#define SCREEN1_H

// Screen 1 has mainly the BPL data plus derived App Att and ABV

#include "touchCali.h" // has the preferences.h
#include "config.h" // has the variable names
#include "getBFData.h" 

// 


// global variables for sending on bpl data in the loop

extern byte state; 
extern float beerTemp;
extern float beerSet;
extern float fridgeTemp;
extern float fridgeSet;
extern float roomTemp;
extern byte mode;
// float pressure; // this from BF doesn't work
extern float gravity;
extern float plato;
extern float Bpressure;

extern bool BPLstale;
// Structure to hold the BPL JSON data:

struct BPL_Data {
   byte state;
  float beerTemp;
  float beerSet;
  float fridgeTemp;
  float fridgeSet;
  float roomTemp;
  byte mode;
  float pressure;
  float gravity;
  float plato;
  float voltage;
  float auxTemp;
  double tilt;
  int hydrometerRssi;
  const char* Hydro;
  int sender;
  float AA;
  float ABV;
};

BPL_Data bpl_Data;

  void drawColoredBorder(uint16_t Color)
    {
    tft.fillScreen(Color);// Fill the entire screen with the border color
    tft.fillRoundRect(4, 4, 312, 232, 2, InnerBac);// Fill the inner area with black, leaving a border
    }
  // use instead of itoa for number display
  void intToString(int value, char* result, int size) {
    if (snprintf(result, size, "%d", value) < 0) {
      // Handle the case where the buffer is too small for the formatted string
      // or another error occurred (unlikely).
      result[0] = '\0'; // Null-terminate the string to indicate an error.
    }
  } 
  uint16_t getColorFromState(byte state)  {
    switch (state)   {
      case 0:
        return Idle; // indeterminate state possibly bpl not sending
      case 1:
        return OFF; // off
      case 2:
        return Idle; // door open
      case 3:
        return Heating; // heating
      case 4:
        return Cooling; // cooling
      case 5:
        return WaitCool; // wait to cool
      case 6:
        return WaitHeat; // wait to heat

      default:
        return TFT_GREY;
      }
    }


// screen1 mainly from BPL
void screen1()
{
    if (BPLstale) {
    drawColoredBorder(TFT_ORANGE);
    Debug("BPL not sending 1 = true  ");Debugln(BPLstale);
    } else{
    drawColoredBorder(getColorFromState(bpl_Data.state));
    }
   
  // Display FermWatch IP Address name right aligned at the bottom of the screen
    ofr.setFontColor(TFT_SILVER,TFT_BLACK);
    ofr.setFontSize(BRW);
    ofr.setCursor(20,215);
    ofr.printf("FermWatch IP :  ");
    ofr.printf(WiFi.localIP().toString().c_str());
  
  // Modes
  mode = bpl_Data.mode; // 3
  
  ofr.setFontSize(TXT);
  ofr.setCursor(20,7); // was 12,7 
  Debugln("");// Debug("BPL MODE :  ");
  if (mode == 0)
  {
    Debugln("Mode 0 BPL : OFF ");
    ofr.setFontColor(TFT_RED, TFT_BLACK);
    ofr.printf("BPL : OFF\n");
    ofr.setFontColor(TFT_SILVER, TFT_BLACK);    
  }
  if (mode == 1)
  {
    Debugln("Mode 1 BPL : FRIDGE CONSTANT ");
    ofr.setFontColor(TFT_SILVER, TFT_BLACK);
    ofr.printf("BPL : Fridge CONSTANT\n");
  }
      
  if (mode == 2)
  {
    Debugln("Mode 2 BPL : BEER CONSTANT");
    ofr.setFontColor(TFT_SILVER, TFT_BLACK);
    ofr.printf("BPL : Beer CONSTANT");
    
  }
  if (mode == 3)
  {
    Debugln("Mode 3 BPL : BEER PROFILE");
    ofr.setFontColor(bronze);
    ofr.printf("BPL : Beer PROFILE");
    
  }
 
  
  
  // Fixed Text
  
  ofr.setFontSize(TXT);
  ofr.setFontColor(TFT_SILVER, TFT_BLACK);
  ofr.setCursor(40,30);
  ofr.printf("Room\n)");
  ofr.setCursor(190,30);
  if (mode == 1) {
  ofr.printf("        "); // don't display Beer Temp when Fridge Constant set ## not sure about this
  }else{
  ofr.printf("Beer\n)");
  }
  
  tft.fillRect(10, 55, 260, 38, TFT_BLACK); // yes  needs to be behind custom text moved here to get rid of flicker DONE
  ofr.setFontSize(NUM);

  // Conversions F to C ----------------------------------------------------------------------------------
  // float TempC;
  //  // TempF = 0;
  // float TempF = (TempC*1.8)+32.0;
  // Serial.print(TempF);
  // Serial.println("°C");

  // Room Temp
  Serial.println("");Serial.printf("BPL\n");
  roomTemp = bpl_Data.roomTemp; // 35 C
  if (Fahr == true)
  {
    float FroomTemp = ((roomTemp * 1.8) + 32.0);
    Serial.printf("Room Temp  =  %.0f F\n", FroomTemp); // minimum number of digits to write 100.0
    ofr.setFontColor(TFT_WHITE, TFT_BLACK);
    ofr.setCursor(40,45);
    
    if (mode==0){
      ofr.printf("XX");
    }
    else {   
    // no need to shift results for F < 10 well below freezing if >= 100F should just print 
    if (FroomTemp >= 100)
    {
    ofr.setFontColor(TFT_RED, TFT_BLACK);
    ofr.setCursor(25,45);
    }
    char frmDisp[5];//was 8
    // whole number %.0f rounded to nearest integer
    snprintf(frmDisp, sizeof(frmDisp), "%.0f",FroomTemp);
    ofr.printf(frmDisp);
    }    
  }
  else if (Fahr == false)
  {
    Serial.printf("Room Temp =  %.1f C\n", roomTemp); // https://alvinalexander.com/programming/printf-format-cheat-sheet/  \n for newline == println
    ofr.setFontColor(TFT_WHITE, TFT_BLACK);
    ofr.setCursor(25,45);
    
    if (roomTemp < 10)
    { // change cursor posn 
      ofr.setCursor(27,45);
    } 
    if (roomTemp >= 40)
    {
    ofr.setFontColor(TFT_RED, TFT_BLACK);
    }
    char rmDisp[5];
    
    snprintf(rmDisp, sizeof(rmDisp), "%.1f",roomTemp);
    ofr.printf(rmDisp);
       
  }
  // Beer temp
  ofr.setCursor(185,45);
  if (mode == 1) {
  ofr.printf("        ");// don't display Beer Temp when Fridge Constant set
  }else
  {
  beerTemp = bpl_Data.beerTemp; // 10.074
  if (Fahr == true)
  {
    float FbeerTemp = ((beerTemp * 1.8) + 32.0);
    Serial.printf("Beer Temp =  %.0f F\n", FbeerTemp);
    ofr.setFontColor(TFT_GOLD, TFT_BLACK);
    ofr.setCursor(185,45);
    if (mode==0){
      ofr.printf("XX"); // print XX when BPL mode is 0 ie OFF
    }
    else {   
    if (FbeerTemp < 10.0) // try using 10.0 rather than 10    corrected position Fbeer not beer <99
    {
      ofr.setCursor(177,45); // was 165,45 
      
    }
    char FbDisp[5];
    //integer
    snprintf(FbDisp, sizeof(FbDisp), "%.0f", FbeerTemp);
    ofr.printf(FbDisp);    
    // get the beer temp
    }
  }
  else if (Fahr == false)
  {
    Serial.printf("Beer Temp =  %.1f C\n", beerTemp);
    ofr.setFontColor(TFT_GOLD, TFT_BLACK);
    ofr.setCursor(165,45);

    if (beerTemp < 10)
    { //   change cursor posn 
      ofr.setCursor(177,45); // 177,45? was 165,45
    }
    char bDisp[5];
    snprintf(bDisp, sizeof(bDisp), "%.1f", beerTemp);
    ofr.printf(bDisp);
    
  }
  } // end of don't display with fridge const
  
  // Fixed text
  ofr.setFontSize(NUM);
  ofr.setFontColor(TFT_SILVER, TFT_BLACK);
  ofr.setCursor(280, 80);// was 105

  if (Fahr == true)
  {
    ofr.printf("F");
    // Debugln("Fahrenheit set");
  }
  else if (Fahr == false)
  {
    ofr.printf("C");
    // Debugln("Celsius set");
  }
  ofr.setFontSize(TXT);
  ofr.setCursor(265,85);
  ofr.printf("o");
    
  // Fixed Text
  ofr.setFontSize(TXT);
  ofr.setFontColor(TFT_SILVER, TFT_BLACK);
  ofr.setCursor(40,96);// 40,91
  ofr.printf("Fridge\n)");
     
  if (mode == 1) {
  ofr.setCursor(172,96);
  ofr.printf("Fridge Set\n)");// don't display Beer Set (Target) when Fridge Constant set updates once bpl data rxed
  }else
  {
    ofr.setCursor(180,96);
  ofr.printf("Target\n)");
  } 
  tft.fillRect(10, 115, 260, 38, TFT_BLACK); // background for custom font  DONE
  ofr.setFontSize(NUM);
  ofr.setFontColor(TFT_WHITE, TFT_BLACK);

  // Fridge Temp
  fridgeTemp = bpl_Data.fridgeTemp; // 14.893
  if (Fahr == true)
  { 
    float FfridgeTemp = ((fridgeTemp * 1.8) + 32.0);
    Serial.printf("F Fridge Temp  =  %.0f F\n", FfridgeTemp);
    ofr.setCursor(40,105);
    if (mode==0){
      ofr.printf("XX");
    }
    else {   
    if (FfridgeTemp < 10) // not a likely scenario
    {
      // change cursor posn +32px if less than 10 to align the display position
      ofr.setCursor(30,105);
    }
    char fftDisp[5];
    // integer
    snprintf(fftDisp, sizeof(fftDisp), "%.0f",FfridgeTemp);
    ofr.printf(fftDisp);
    }    
  }
  else if (Fahr == false)
  {
    Serial.printf("Fridge Temp =  %.1f C\n", fridgeTemp);
    ofr.setCursor(25,105);

    if (fridgeTemp < 10)
    { // change cursor posn 
      ofr.setCursor(27,105); // was 30,105
    }
    char ftDisp[5];
    //integer
    snprintf(ftDisp, sizeof(ftDisp), "%.1f",fridgeTemp);
    ofr.printf(ftDisp);       
  }
  
  // Beer Set ie Target Temperature
  // If Beer constant fridgeSet = bpl_Data.fridgeSet;
  if (mode==1){
  beerSet = bpl_Data.fridgeSet;
  } else {
  beerSet = bpl_Data.beerSet;   
  }
  if (Fahr == true)
  {
    float FbeerSet = ((beerSet * 1.8) + 32.0);
    Serial.printf("F Target Temp =  %.0f F\n", FbeerSet);
    ofr.setFontColor(TFT_ORANGE, TFT_BLACK);
    ofr.setCursor(185,105);
    if (mode==0){
      ofr.printf("XX");
    }
    else {   
    if (FbeerSet < 10)
    {
      // change cursor posn +32px if less than 10 to align the display position
      ofr.setCursor(165, 105);
    }
    char fbDisp[5];
    //integer
    snprintf(fbDisp, sizeof(fbDisp), "%.0f",FbeerSet);
    ofr.printf(fbDisp);    
    }
  }
  else if (Fahr == false)
  {
    Serial.printf("Target Temp =  %.1f C\n", beerSet);
    ofr.setFontColor(TFT_CYAN, TFT_BLACK); // or 0x57DD
    ofr.setCursor(165,105);//was 150

    if (beerSet < 10)
    { // change cursor posn 
      ofr.setCursor(177,105); // was 177,105 xx
    }
    char bDisp[5];
    snprintf(bDisp, sizeof(bDisp), "%.1f",beerSet);
    ofr.printf(bDisp);      
        
  }
  
  // fridgeSet = bpl_Data.fridgeSet; // 13.752 not useful
  // Debug("Fridge Set :  ");
  // Debugln(fridgeSet);
  
  // REF only ---------------------------------------------------------------------
  // for use when information comes via BPL
  // // iSpindel
  // float plato = doc["plato"]; // 13.752
  // Debug("Plato :  ");
  // Debugln(plato);

  // float auxTemp = doc["auxTemp"]; // 13.752
  // Debug("iSpindel Temp :  ");
  // Debugln(auxTemp,1);

  // float voltage = doc["voltage"]; // 13.752
  // Debug("iSpindel V :  ");
  // Debugln(voltage);

  // float tilt = doc["tilt"]; // 13.752
  // Debug("iSpindel Angle :  ");
  // Debugln(tilt);

  // BPL Pressure ---- pressure direct from BPL for FV1 batch controlling ----------------------------------------------------------------------------
  
  Bpressure = bpl_Data.pressure;                             // 
  Serial.printf("BPL Pressure  =  %.2f PSI\n", Bpressure);Serial.println(""); // 2 dp
  // Pressure from BPL
  ofr.setFontColor(TFT_MAGENTA, TFT_BLACK);
  ofr.setFontSize(NUM);
  ofr.setCursor(170,165);// was 175

  if (Bpressure < 10)
    {
     ofr.setCursor(190,165);
    }
  char BpDisp[5];
  snprintf(BpDisp, sizeof(BpDisp), "%.1f",Bpressure);
  ofr.printf(BpDisp);
  
  ofr.setFontColor(TFT_SILVER, TFT_BLACK);
  ofr.setFontSize(TXT);
  ofr.setCursor(205,155); // was 190,150
  ofr.printf(" PSI ");
  

  // Mostly BPL MODE Screen 1 ***********************************************************************************
  
  state = bpl_Data.state; // this needs to be here so that the state fwbpl data gets sent
  
  ofr.setFontColor(TFT_SILVER, TFT_BLACK);
  ofr.setFontSize(TXT);
  ofr.setCursor(12,5);

  
 // Fixed Text  Screen 1 
  /* ofr.setFontSize(TXT);
  ofr.setFontColor(TFT_SILVER, TFT_BLACK);
  ofr.setCursor(20,155);// 20,150
  ofr.printf("App.Att. %% ");
  ofr.setCursor(190,155); // was 190,150
  ofr.printf("ABV %% "); // https://www.geeksforgeeks.org/how-to-print-using-printf/ ie. double%% */
  
  
  
  
  ofr.setFontSize(TXT);
  ofr.setFontColor(TFT_SILVER, TFT_BLACK);
  ofr.setCursor(10,162);// 
  ofr.printf("App.Att. %% ");
  ofr.setCursor(37,192);
  ofr.printf("ABV %% ");

  // ---------------------------------------------------------------------------------------------------------------------------------
  // Apparent Attenuation - appears on all screens 1 from BPL, 2 & 3  from BF Batch(es)
  // if one batch shows those details
  ofr.setFontSize(50); // 
  ofr.setFontColor(TFT_GREEN, TFT_BLACK);
  ofr.setCursor(97, 155); // 
  
  if (iSp_sg == 0)
  { // if iSp_sg not received yet print blank  don't forget the ==
    ofr.printf("");
  }
  else if ((iSp_sg > 0) && (Temp_Corr == false) && (FV == false)) // greater than 0 and no temp correction one FV
  {
    App_Att = (((SGogy - 1) - (iSp_sg - 1)) / (SGogy - 1)) * 100; // 40 - say 20  =20/40 = 50% * by 100
    bool neg_Val = App_Att < 0;                                   // trap negative numbers and force to zero
    if (neg_Val) App_Att = 0;
      
      char aaDisp[5];
      snprintf(aaDisp, sizeof(aaDisp), "%.1f",App_Att);
      ofr.printf(aaDisp);
    
    // Serial.printf("Current Apparent Attenuation = %.2f%s\n", App_Att, "%");
    // Debugln("");
  }
  
  // App Att corrected for iSp temperature
    // ofr.setFontColor(TFT_GREEN, TFT_BLACK);
    // ofr.setCursor(15, 170);
  
  
  if (iSp_sg == 0)
  { // if iSp_sg not received yet print blank  don't forget the ==
    ofr.printf("");
   }
  else if ((iSp_sg > 0) && (Temp_Corr == true))
  {
                                                           // use iSp_sga
    float App_Att_Corr = (((SGogy - 1) - (iSp_sga - 1)) / (SGogy - 1)) * 100; // using temp adjusted PG
    bool neg_Val = App_Att_Corr < 0;          // CYD                         // trap negative numbers and force to zero
    if (neg_Val) App_Att_Corr = 0;           // CYD


      char aacDisp[5];
      snprintf(aacDisp, sizeof(aacDisp), "%.1f",App_Att_Corr);
      ofr.printf(aacDisp);
      
      
    
    // testing only
    Serial.printf("Current Temp Corr. Apparent Attenuation = %.2f%s\n", App_Att, "%");
    Debugln("");
  }
  
  // tft.setTextColor(TFT_SILVER, TFT_BLACK);

  // ABV  all calc using UK tax rules   from iSpindel calc in SG - display in Plato as an option
  // this is the original code the ones in screen2 and 3 are updated but give the same result

  if (iSp_sg <= 0) // less than or equal to 0
  { // if iSpindel gravity not received yet removed the - 0 or negative don't calculate abv
    abv = 0.0;
    // Serial.printf("ABV   =  %.2f%s\n", abv, "%");
    ofr.setFontColor(TFT_LIGHTGREY, TFT_BLACK);
    ofr.setFontSize(50);
    ofr.setCursor(97,180);
    ofr.printf("--.--");
    
  }
  
  
  else if (iSp_sg > 0)
  {                 // greater than zero was not equal to zero
    float Init_abv; // first calc ABV then work out the fudge factor https://www.gov.uk/government/publications/excise-notice-226-beer-duty/excise-notice-226-beer-duty--2#calculation-strength
    Init_abv = ((SGogy - iSp_sg) * 131.25);
    // Serial.printf("Initial ABV = %.3f\n", Init_abv); // nominal 131.25 bit high as 6% beers! seems to be what BF uses
    if (Init_abv >= 0.0 && Init_abv <= 3.3)
      ABV_adjG = 128;
    if (Init_abv >= 3.3 && Init_abv <= 4.6)
      ABV_adjG = 129;
    if (Init_abv >= 4.6 && Init_abv <= 6.0)
      ABV_adjG = 130;
    if (Init_abv >= 6.0 && Init_abv <= 7.5)
      ABV_adjG = 131;
    if (Init_abv >= 7.5 && Init_abv <= 9.0)
      ABV_adjG = 132;
    if (Init_abv >= 9.0 && Init_abv <= 10.5)
      ABV_adjG = 133;
    if (Init_abv >= 10.5 && Init_abv <= 12.0)
      ABV_adjG = 134;

    // Debug("Fudge : ");
    // Debugln(ABV_adjG);

    // recalculate the ABV using the fudge factor and print the results

    adjABV = ((SGogy - iSp_sg) * ABV_adjG);
    // Serial.printf("Adjusted ABV   =  %.2f%s\n", adjABV, "%");
    
    // ABV % display
    // check for a negative value perhaps when the iSp shows an increase in gravity at the start of fermentation
    bool neg_Val = adjABV < 0; 
    if (neg_Val) adjABV =0; // set to zero
    ofr.setFontColor(0xDBC3, TFT_BLACK);
    ofr.setFontSize(50);
    ofr.setCursor(97,180);
    char adjABVDisp[5];
    snprintf(adjABVDisp, sizeof(adjABVDisp), "%.2f",adjABV);
    ofr.printf(adjABVDisp);
        
  } // end abv adjustment
  
  // blank app att and abv when 2 batches FV true or when just BPL brewF false
  if ((FV ) || (brewF == false)){
    tft.fillRect(5, 160, 150, 60, TFT_BLACK);
  }
        // Don't forget the ==
        // x == y (x is equal to y)
        // x != y (x is not equal to y)
        // x <  y (x is less than y)
        // x >  y (x is greater than y)
        // x <= y (x is less than or equal to y)
        // x >= y (x is greater than or equal to y)
  
  
} // end of mostly BPL stuff











#endif