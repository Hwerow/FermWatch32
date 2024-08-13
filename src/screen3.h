#ifndef SCREEN3_H
#define SCREEN3_H

//Screen 3 has mainly BF data with pressure from BPL for Batch 2

#include "MultiMap.h"

#include "touchCali.h" // has the preferences.h
#include "config.h" // has the variable names
#include "getBFData.h" 
// #include "screen1.h" // may not need to be included??

// Batch 2
extern float Bpressure;
// float pressure2;  // only if manual from BF at the moment
// float iSp_temp;  // iSpindel 'aux' temperature
float iSp_sg2;  // present SG
float iSp_sga2; // temperature 20C adjusted present SG
float Itemp2;   // unadjusted iSpindel temperature
// float last_angle; // store the last angle to be able to compare in future?
float App_Att2;

//int16_t batch_No; // select this one for screen 2
int16_t batch_No2; // this one for screen3
int rssi2; // BF rssi for iSpindel
float battery2;
float angle2;

// GM detection from input not tested
bool GMused2 = false;  // logic flag to be derived from GM button state
bool gravmon2 = false; // was used in json and LittleFS wm config  

// GLOBAL? variables gravity monitor may need to be somewhere else?
char gm2; // was string
// String gname; // was string tried char
String GM_name2; // was string
const char *gtoken2;
const char *gID2; // gm ID
float gtemp2;
float ggrav2;
float gangle2;
float gbatt2;
int grssi2;

// Function to determine if Hydro/iSpindel iSp_id name includes a colour
  String checkColours2(String iSp_id2) {
  String targets[] = {"BLUE", "RED", "GREEN", "WHITE", "ORANGE", "PCB"};

  for (int i = 0; i < 6; i++) {
    if (iSp_id2.indexOf(targets[i]) != -1) {
      return targets[i];
    }
  }
  return "";
  }
  // function to change the iSp_id2 display colour
  void setDisplayColor2(String colour) {
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

// screen 3 has most of the BF or BPL stuff dependent on what option selected or Gravity Monitor if used
void screen3() // needs to be up here or error messages result if put at end
{
    // printf("iSp_id value in screen2: %s\n", iSp_id); // debug
    
    if (BPLstale) {
    drawColoredBorder(TFT_ORANGE);
    }else{
    drawColoredBorder(getColorFromState(bpl_Data.state));
    }  
  

  // tft display mostly iSpindel and derived functions-----------------------------------------------------------------------------------------------------
  // wipe the screen innerbac and update
  // tft.fillScreen(TFT_VIOLET); see if heating cooling etc is maintained
   tft.fillRoundRect(4, 4, 312, 232, 2, InnerBac); // clear startup/last screen
  
  
  //  Display brew name and Local IP at the bottom of the screen
    ofr.setFontColor(bronze,TFT_BLACK);
    ofr.setFontSize(BRW);
    ofr.setCursor(10,215);// was 160,215
    // ofr.cprintf(brew.c_str()); // removed centre and added Local IP
    ofr.printf(brew2.c_str());
    ofr.setCursor(220,215); // 
    // ofr.printf(WiFi.localIP().toString().c_str());

   ofr.setFontColor(TFT_SILVER,TFT_BLACK);
   ofr.setFontSize(TXT);
   ofr.setCursor(12,5); // was 12, 12 now 12, 5
  
  // tft.setTextFont(1);
  // tft.setCursor(12, 12);
  // tft.print("iSpindel");
  // compare iSp connection status https://docs.arduino.cc/built-in-examples/strings/StringComparisonOperators

  

  if (iSp_id2 == "manual")
  {
    Debugln("iSpindel not attached in BF or manual reading(s) entered!");
    ofr.setFontColor(TFT_RED,TFT_BLACK);
    ofr.setFontSize(TXT);
    ofr.setCursor(14,7);// was 12,5 and as below
    ofr.printf("Manual      ");   
    
    /* tft.setCursor(12, 12); // was 130,12
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.print("Manual      "); // indicates that a manual reading has been input to BF e.g. pressure reset of fields,  last values received 900s BF cycle
    // or the fermenting bach has been kegged, BF status changed to conditioning and the values are no longer valid */
  }

  if ((brewF == false) && (FV))// BF not used and 2 batches selected
  {
    Debugln("BF not used");
    ofr.setCursor(14,7);
    ofr.setFontColor(TFT_RED, TFT_BLACK);
    ofr.printf("BF :  ");
    ofr.printf("   BF Not Used");  
       
  }


  
  if (brewF == true) // 
  //{
  ofr.setFontSize(TXT);
  ofr.setCursor(14,7);
  ofr.setFontColor(TFT_MAGENTA, TFT_BLACK);
  ofr.printf("BF :  ");
  
  // change display colour to suit found Hydrometer Colour
    String colourFound = checkColours(iSp_id2);
    if (colourFound != ""){
    Debug("Found Colour: "); Debugln(colourFound);
    setDisplayColor(colourFound);
    }
    else
    {
    Debugln("No colour found");
    ofr.setFontColor(TFT_SILVER, TFT_BLACK);// default
    }
    
  ofr.printf(iSp_id2.c_str());
       
    // Serial.print(F("iSp_id =  ")); Serial.println(iSp_id); // debug
  //}
  ofr.setFontColor(TFT_SILVER, TFT_BLACK);
  ofr.setFontSize(TXT);
  ofr.setCursor(12,35); // was 12,40
  ofr.printf("iSp RSSI");

  ofr.setCursor(80, 35);
  if (rssi2 <= -80) {
    // then red
    ofr.setFontColor(TFT_RED, TFT_BLACK);
    } else if (rssi2 <= -67) {
    // then yellow
    ofr.setFontColor(TFT_YELLOW, TFT_BLACK);
    } else {
    // then green
    ofr.setFontColor(TFT_GREEN, TFT_BLACK);
    }
  /* char rssiDisp[6]; // 5 numbers should be heaps incl minus sign
  itoa(rssi, rssiDisp, 10); //10 is the base i.e. decimal
  ofr.printf(rssiDisp);ofr.setFontColor(TFT_SILVER, TFT_BLACK);// ofr.printf(" dBm"); */
  ofr.printf("%d", rssi2); // Print the RSSI directly
  
  //Fixed text
  ofr.setCursor(35, 90); // was 40, 95
  ofr.setFontColor(TFT_SILVER, TFT_BLACK);
  ofr.printf("iSp Temp.\n");

  
  ofr.setCursor(145, 35); //was 165, 35 
  ofr.printf("iSp Battery\n");

  // Display iSpindel Voltage
  ofr.setFontSize(TXT);
  ofr.setCursor(240, 35);
   
   if (battery2 >= 3.80) {
    // then green
    ofr.setFontColor(TFT_GREEN, TFT_BLACK);
    } else if (battery2 >= 3.50 && battery < 3.80) {
    // then yellow
    ofr.setFontColor(TFT_YELLOW, TFT_BLACK);
    } else {
    // then red
    ofr.setFontColor(TFT_RED, TFT_BLACK);
    }
  
  char iVDisp[5];
    snprintf(iVDisp, sizeof(iVDisp), "%.2f", battery2); // width of 5 and 2 decimal places
    ofr.printf(iVDisp);ofr.setFontColor(TFT_SILVER, TFT_BLACK);ofr.printf( " V\n");

  // App Att and ABV fixed text  AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
  ofr.setFontSize(TXT);
  ofr.setFontColor(TFT_SILVER, TFT_BLACK);
  ofr.setCursor(10,162);// 
  ofr.printf("App.Att. %% ");
  ofr.setCursor(37,192);
  ofr.printf("ABV %% ");
  
  // AA meetings etc 
  // Apparent Attenuation - appears on all screens 1 from BPL, 2 & 3  from BF Batch(es)
  ofr.setFontSize(50); // 
  ofr.setFontColor(TFT_GREEN, TFT_BLACK);
  ofr.setCursor(97, 155); // 
  
  if (iSp_sg2 == 0)
  { // if iSp_sg not received yet print blank  don't forget the ==
    ofr.printf("");
  }
  else if (iSp_sg2 > 0 && Temp_Corr == false && FV) // greater than 0 and no temp correction 2 FV
  {
    App_Att2 = (((SGogy2 - 1) - (iSp_sg2 - 1)) / (SGogy2 - 1)) * 100; // 40 - say 20  =20/40 = 50% * by 100
    bool neg_Val = App_Att2 < 0;                                   // trap negative numbers and force to zero
    if (neg_Val) App_Att2 = 0;
      
      char aaDisp[5];
      snprintf(aaDisp, sizeof(aaDisp), "%.1f",App_Att2);
      ofr.printf(aaDisp);
    
    Serial.println("");Serial.print(brew2);Serial.printf(" Current Apparent Attenuation = %.2f%s\n", App_Att2, "%");
    
  }

  // App Att corrected for iSp temperature
    // ofr.setFontColor(TFT_GREEN, TFT_BLACK);
    // ofr.setCursor(15, 170);
  
  
  if (iSp_sg2 == 0)
  { // if iSp_sg not received yet print blank  don't forget the ==
    ofr.printf("");
   }
  else if (iSp_sg2 > 0 && Temp_Corr == true)
  {
                                                           // use iSp_sga
    float App_Att_Corr2 = (((SGogy2 - 1) - (iSp_sga2 - 1)) / (SGogy2 - 1)) * 100; // using temp adjusted PG
    bool neg_Val2 = App_Att_Corr2 < 0;          // CYD                         // trap negative numbers and force to zero
    if (neg_Val2 || App_Att_Corr2 >= 100) {
    App_Att_Corr2 = 0;
     } else {

      char aacDisp[5];
      snprintf(aacDisp, sizeof(aacDisp), "%.1f",App_Att_Corr2);
      ofr.printf(aacDisp);
      
    
    // testing only
    //Serial.printf("Current Temp Corr. Apparent Attenuation = %.2f%s\n", App_Att_Corr2, "%");
    //Debugln("");
    }
  }  
  // Calculate ABV that uses the UK Excise method that takes into account the alcohol
  // https://www.gov.uk/government/publications/excise-notice-226-beer-duty/excise-notice-226-beer-duty--2#calculation-strength
  // extract og and fg
  float og = SGogy2;
  float fg = iSp_sg2;
  float Init_abv2; // first calc ABV then work out the fudge factor 
  Init_abv2 = ((og - fg) * 131.25); // nominal 131.25 bit high as 6% beers! seems to be what BF uses
  Debugln("");
  Serial.print(brew2);Serial.printf(" Initial ABV = %.3f%s\n", Init_abv2, "%"); 

  // Define the ABV ranges and their corresponding values
  float ABV_ranges[] = {0.0, 3.3, 4.6, 6.0, 7.5, 9.0, 10.5, 12.0};
  int ABV_values[] = {128, 129, 130, 131, 132, 133, 134};

  // Find the appropriate ABV_adjG value based on Init_abv
  int ABV_adjG2 = 0;
  for (int i = 0; i < sizeof(ABV_ranges) / sizeof(ABV_ranges[0]); ++i) {
  if (Init_abv2 >= ABV_ranges[i] && Init_abv2 <= ABV_ranges[i + 1]) {
  ABV_adjG2 = ABV_values[i];
  break;
  }
  }

  Debug("Fudge : ");
  Debugln(ABV_adjG2);
  // recalculate the ABV using the appropriate fudge factor 
  float abv2 = ((og - fg) * ABV_adjG2);
  Serial.print(brew2);Serial.printf(" Adjusted ABV   =  %.2f%s\n", abv2, "%\n");
                          
  /// ABV % display
  // check for a negative value perhaps when the iSp shows an increase in gravity at the start of fermentation
    bool neg_Val = abv2 < 0; 
    if (neg_Val) abv2 =0; // set to zero

    ofr.setFontColor(0xDBC3, TFT_BLACK);
    ofr.setFontSize(50);
    ofr.setCursor(97,180);
    char adjABVDisp[5];
    snprintf(adjABVDisp, sizeof(adjABVDisp), "%.2f",abv2);
    ofr.printf(adjABVDisp);


      
    
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
    float iSpTemp = ((Itemp2 * 1.8) + 32.0);
    Serial.print(brew2);Serial.printf(" iSpindel Temp  =  %.0f F\n", iSpTemp);
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
    Serial.print(brew2);Serial.printf(" iSpindel Temp  =  %.1f C\n", Itemp2);
    
    // tft.fillRect(10, 175, 300, 40, TFT_BLACK); // blanking bottom row DONE
    ofr.setCursor(15,105);
    if (Itemp2 < 10)
    {
     ofr.setCursor(30,105);
    }

    char itDisp[5];
    snprintf(itDisp, sizeof(itDisp), "%.1f",Itemp2);
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
  snprintf(anDisp, sizeof(anDisp), "%.2f",angle2);
  ofr.printf(anDisp);
  ofr.setFontSize(TXT);
  ofr.setCursor(285,110);// was 290
  ofr.printf("o");
  ofr.setFontSize(NUM);
  
  
  // Present Gravity   from iSpindel SG unadjusted by iSp temperature
  ofr.setFontColor(TFT_ORANGE, TFT_BLACK);
    
  
  if (Plato == true && Temp_Corr == false)
  {
    Debug("If Plato true ");    Debug(Plato); Debug("  & Temp_Corr false  ");Debugln(Temp_Corr);
    float iSpPlato2 = ((259 - (259 / iSp_sg2)));
    Serial.print(brew2);Serial.printf(" iSpindel/GM PG Plato  =  %.1f deg P\n", iSpPlato2);
    
    if (iSpPlato2 < 10.0) { //  step to the right
      ofr.setCursor(190,165);
    }
    else {
    ofr.setCursor(170,165); // was 190,165
    }
    char ispPDisp[5];
    
    snprintf(ispPDisp, sizeof(ispPDisp), "%.1f",iSpPlato2);
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
    Debug("Present Gravity  ");Debugln(brew2);
    Debug("  Plato false ");Debug(Plato); Debug("  & Temp_Corr false  ");Debugln(Temp_Corr);
    Serial.print(brew2);Serial.printf(" iSpindel/GM PG SG  =  %.5f deg\n", iSp_sg2);
    
    
    ofr.setCursor(165,165);
    char issgDisp[7];
    //dtostrf(iSp_sg,5,4, issgDisp);
    snprintf(issgDisp, sizeof(issgDisp), "%.4f",iSp_sg2);
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
      SG_adjT2 = multiMap<float>(Itemp, input, adj_Val, 41);

      Debug("SG_adjT = ");
      Serial.println(SG_adjT2, 5);
      delay(10);

      iSp_sga2 = (iSp_sg2 + SG_adjT2);

      Debug("Adjusted Gravity = ");
      Serial.println(iSp_sga2, 5);
      Debug("iSp SG Temp correction ");
      Serial.println(Temp_Corr, 5);
      Debug("Correction : ");
      Serial.println(SG_adjT2, 5);
    }
  }

  if (Temp_Corr == true && Plato == false)
  {
    // display a small orange circle when temperature corr active
    tft.fillCircle(295, 166, 4, TFT_ORANGE);
    bool negVal2 = iSp_sga2 < 0; // trap negative values and force to zero 
    if (negVal2 || iSp_sga2 >=1.150 || iSp_sga2 <= 0.0043){  // trap nonsense numbers on startup only appies to screen 3 and a second bach when BF not selected
    iSp_sga2 = 0;
    }else{
    // print adjusted SG
    
    ofr.setCursor(165, 165);
    char issgaDisp[7];
    snprintf(issgaDisp, sizeof(issgaDisp), "%.4f",iSp_sga2);
    ofr.printf(issgaDisp);
    }
    
  }

  // for temperature adjusted Plato
  if (Temp_Corr == true && Plato == true)
  {
    Debug("Plato true "); Debug(Plato); Debug("  &  Temp_Corr true = 1  "); Debugln(Temp_Corr);
    // tft.fillRect(190, 200, 95, 35, cust);
    float iSpPlato2 = ((259 - (259 / iSp_sga2))); // adjusted version
    bool negVal2 = iSpPlato2 < 0; // trap negative values and force to zero 
    if (negVal2 || iSpPlato2 >=30 ){ 
    iSpPlato2 = 0;
    }else{
        
    ofr.setFontColor(TFT_ORANGE, TFT_BLACK);
    ofr.setCursor(190,165);
    
    char iSpPDisp[5];
    snprintf(iSpPDisp, sizeof(iSpPDisp), "%.1f",iSpPlato2);
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
  // Message Line
  
  //  swop adjusted SG value for unadjusted value
  if (Temp_Corr == true)
  {
    Debug("");
    Debugln("SG adjusted for iSpindel/GM temperature");
  }
  else if (Temp_Corr == false) // debug only
  {
    // Debugln("");
    // Debugln("No SG temp adjustment");
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
    float OGPlato = ((259 - (259 / SGogy2)));
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
   
    
    float FGPlato = ((259 - (259 / FGEst2)));
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
    ofr.setCursor(50,60);
    ofr.setFontColor(TFT_GREEN, TFT_BLACK);
    // float to string for display
    char OGsgDisp[7];
    // width of 5 and 4 decimal places
    snprintf(OGsgDisp, sizeof(OGsgDisp), "%.5f", SGogy2);
    ofr.printf(OGsgDisp);

    ofr.setCursor(240,60);
    ofr.setFontColor(0xF502, TFT_BLACK);
    char FGsgDisp[7];
    // width of 5 and 4 decimal places
    snprintf(FGsgDisp, sizeof(FGsgDisp), "%.5f", FGEst2);
    ofr.printf(FGsgDisp);
    
    
  }

  
  
  
} // end screen3












#endif