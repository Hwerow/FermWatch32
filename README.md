# FermWatch32 - ESP32 CYD
Monitoring fermentation with Brewfather®, BrewPiLess and iSpindel or "No phone No worries!"

- Have you struggled to see the BPL SSD1306 OLED screen?
- Don’t have your phone/tablet handy?
- Forgot your glasses?
- Or just want to see where your latest brew is up to?

The video Tutorial has step-by-step instructions and describes how to make a FermWatch32 with a 2.8" CYD to monitor your fermentation – in the shed, beside the bed, wherever your local network extends!
  
Link to video:    

**Important** I am a brewer and author, I write books on brewing traditional beers - **NOT a PROGRAMMER** - as will be obvious from inspection of the code. https://www.lulu.com/spotlight/prsymons 
# Project
This project takes the FermWatch 8266 version and boldly migrates it to ESP32 with an ILI9341 TFT then to a CYD - ESP32 Arduino LVGL WIFI & Bluetooth Development Board 2.8 " 240*320 Smart Display Screen 2.8inch LCD TFT Module With Touch WROOM  AITEXM ROBOT Official Store. This is a 2 USB Variant.

# Limitations
Not many - see the video

# Software issues and change requests
Please do not expect any form of support. I am a brewer and author, not a coder/programmer! I have used ChatGPT for some of the fancy bits of code and don't fully understand how it works. I have done a fair bit of testing but if you find something please let me know and I will investigate – no promises! 
 
# Main Prerequisites
- [CYD] ESP32 Arduino LVGL WIFI & Bluetooth Development Board 2.8 " 240*320 Smart Display Screen 2.8inch LCD TFT Module With Touch 
  WROOM  AITEXM ROBOT Official Store (no affiliation just the one I found)
- BrewPiLess System [BPL] Software v4.3.1 by Vitotai  https://github.com/vitotai/BrewPiLess
- iSpindel [iSp] Software v7.1.0 https://github.com/universam1/iSpindel    Cherry Philip board v4.0 Build your own 
  https://youtu.be/4HYzm0psaNw or
- GravityMon [GM] https://mp-se.github.io/gravitymon/index.html 
- Brewfather® v2.10.9 [BF]  https://brewfather.app/ Premium Version - so yes you have to pay an annual subscription but it is worth every cent.
# System Overview
![FW32_System_2024-08-13_14-04-31](https://github.com/user-attachments/assets/b8448ba3-4f71-44dd-aded-40f8fd23b5a9)

# Configuration Options
- BF Not Selected - can be used just with BPL without BF
- Select 2FV? to monitor 2 BF Fermenting Batches
- Select GM to use with GravityMon - might be useful for calibration?
- Select wm_WIPE clears the WiFiManger settings
- Select Temp_Corr iSpindel SG 20°C approximate temperature correction - default none. 	An experimental function that modifies 
  the displayed Present 	Gravity, Apparent Attenuation and % ABV values
- Select Fahrenheit  - default Celsius
- Select Plato - default SG
- Select Receiver to well receive forwarded BPL data from another FW32
- Select BF_Poll to change the BF update interval to 10 minutes from the default 15 minutes

# Links
- Base64 Encode  https://www.base64encode.org/ 
- Postman   https://www.postman.com/ 
- Flasher https://github.com/marcelstoer/nodemcu-pyflasher/releases
- MQTT Explorer https://mqtt-explorer.com/
- Espressif Flash Download Tool https://www.espressif.com/en/support/download/other-tools

# Acknowledgements
This project would not have been possible without using libraries from Bodmer - TFT_eSPI screen, hsaturn - TinyMqtt Broker, B Blanchon - ArduinoJson, tzapu - WiFiManger, NTPClient and the MultiMap for interpolation and others who are referenced in the code. Brian Lough https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display

# FermWatch32 Versions

1.2.3  First Release

# Future improvements 
- Monitor the amount of change of iSp angle/SG to assess the rate of fermentation
  
# My YouTube channel, Buy My Books, GitHub and website:  
\-------------------------------------------------------------------------------------------------  
- https://m.youtube.com/channel/UCRhjjWS5IFHzldBhO2kyVkw/featured   Tritun Books Channel
- https://www.lulu.com/spotlight/prsymons  Books available Print on demand from Lulu
- https://github.com/Hwerow/FermWatch32  This 
- https://prstemp.wixsite.com/tritun-books   Website  
\-------------------------------------------------------------------------------------------------
