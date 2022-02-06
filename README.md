# IoTProject
Project for the course Embedded Software for the Internet of Things @ UniTN fall 2021, made by me Eric.

It consists of connecting the MSP432P401R and the NodeMCU ESP8266 microcontrollers in order to ***send microphone data*** to a ***server*** and then ***visualizing the waveform on a iOS app.***

If loud noises are detected by the microcontroller, a ***notification message*** is sent to the smartphone using a Telegram bot, since I do not have an Apple Developer Account subscription.

## HW Requirements
For this project it is used:
- Texas Instrument MSP432P401R LaunchPad Development Kit
- Texas Instrument BoosterPack MK II
- NodeMCU Amica ESP8266
- A breadboard
- Some jumpers
- 2 USB cables

## SW Requirements
- [Code Composer Studio](https://www.ti.com/tool/CCSTUDIO) v10.4.0.00006 
- [NodeMCU PyFlasher](https://github.com/marcelstoer/nodemcu-pyflasher)
- Apache httpd v2.4.51
- Telegram Bot API key
- NodeJS (with express and node-telegram-bot-api modules installed)

## Setting up
### MPS432 Firmware
For this project, the NodeMCU needs to have flashed [this firmware](https://docs.ai-thinker.com/en/esp8266/sdk), **download** the one with this header:
* ESP8266 MQTT transparently transmits AT firmware (AT port is changed to UART0)

It is necessary in order to communicate with the NodeMCU ESP8266 with AT commands through UART.
After downloading it, **flash** the NodeMCU ESP8266 with the firmware using the **NodeMCU PyFlasher**
> You can test it using a serial console with baud rate of 115200 and both CR and LF (like the one in the Arduino IDE) and type `AT`, if the answer is `OK` that's all set!
### Apache httpd
> The purpose of utilizing a Apache httpd is to use it as a Proxy Server for decoupling the HTTPS requests from the NodeMCU that are too memory intensive for the latter.
> So, the NodeMCU will do normal HTTP requests that will be handled by the Proxy and redirected as HTTPS to the webisites.
> **If you plan to run the server locally on your computer, skip this part.**

Firstly, set up Apache httpd by modifying the `httpd.conf` file
```
brew install httpd
nano /usr/local/etc/httpd/httpd.conf
```
Uncomment these lines at the start of the file
```
LoadModule proxy_module modules/mod_proxy.so
LoadModule proxy_connect_module modules/mod_proxy_connect.so
LoadModule proxy_http_module modules/mod_proxy_http.so
LoadModule ssl_module modules/mod_ssl.so
```
Add this at the bottom of the file replacing SERVER_ADDRESS (e.g. https://yourserver.com/api) with the address of your server
```
<IfModule mod_proxy.c>

ProxyRequests Off

<Proxy *>
Order deny,allow
Allow from all
</Proxy>

SSLProxyEngine on
ProxyPass /api SERVER_ADDRESS
ProxyPassReverse /api SERVER_ADDRESS

</IfModule>

<Proxy *>
```
Then **run** the proxy server
`sudo brew services start httpd`

The **default port** for httpd is **8080**, and it's the one used in this project.
Remember to have a **static IP address on the PC running the Proxy** (the one that I use will be 192.168.1.37)

### Code Composer Studio
This project makes use of [TI's DriverLib](https://www.ti.com/tool/MSPDRIVERLIB): download it and link the files required for interacting with the BoosterPack MK II sensors and establishing a connection with the ESP8266 through UART communication: go to the Project's properties, and:

- Under Arm Compiler/Include Options include the path to your DriverLib installation (<path>/simplelink_msp432p4_sdk_3_40_01_02/source).
- Under Arm Linker/File Search Path include the DriverLib (<path>/simplelink_msp432p4_sdk_3_40_01_02/source/ti/devices/msp432p4xx/driverlib/ccs/msp432p4xx_driverlib.lib).
  
This project contains some readapted libraries (to interface with the NodeMCU ESP8266 through UART) found [here](https://github.com/amartinezacosta/MSP432-ESP8266), they are inside the CCS project with these names:
* Devices
* Hardware
  
#### MSP432P401R and NodeMCU ESP8266 connections
  Connect directly with some jumpers, or even with a breadboard, the two microcontrollers like so:
  ```/* VCC = 3.3V
 * GND = GND
 * TX = P3.2
 * RX = P3.3
 * RST = P6.1 */
  ```
  >***If the microcontroller reads weird data from the microphone, remove VCC and GND jumpers and use a USB cable to power the ESP8266 module***
  
 ![Image showing the pinout of ESP8266](https://i2.wp.com/randomnerdtutorials.com/wp-content/uploads/2019/05/ESP8266-NodeMCU-kit-12-E-pinout-gpio-pin.png?quality=100&strip=all&ssl=1)
 
#### Secrets
You need to add a secrets.h file in your CCS Project folder containing your Wi-Fi SSID and password like so:
```
#ifndef SECRETS_H_
#define SECRETS_H_

#define SSID "YOUR_SSID"
#define PWD "YOUR_WIFI_PWD"


#endif /* SECRETS_H_ */


#### UART
 Inside the `main.c` file there is a configuration for UART communication which uses the NodeMCU ESP8266's default baud rate of 115200 and [CR][LF] (\r\n)
  ```eUSCI_UART_ConfigV1 UART2Config =
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,
     13,
     0,
     37,
     EUSCI_A_UART_NO_PARITY,
     EUSCI_A_UART_LSB_FIRST,
     EUSCI_A_UART_ONE_STOP_BIT,
     EUSCI_A_UART_MODE,
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,
     EUSCI_A_UART_8_BIT_LEN
};
  ```
### REST API and Telegram Bot Server
  The server in this project is plug-and-play, you just need to set the port, in case you don't want to use the port 3000, and then add a secrets.js file containing your Telegram Bot token API and the chatID of your account like so:
  ```
  const token = 'YOUR_TOKEN';
  const myChatID = YOUR_CHATID;

  module.exports = { token, myChatID }
  ```
### iOS APP
Import the Xcode project and run it with your iPhone as target device
  
## Usage
- Run the server
- Flash the MSP432 with the CCS program
- Open the app on your iPhone
- ***enjoy***
  
You can find a video demo [here](https://youtu.be/pb1NpFmttXo)
  
## Credits
I made this project by myself, but thanks to UniTN and the prof. of the course Embedded Software for the Internet of Things for giving me this great opportunity.
