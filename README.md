# IoTProject
Project for the course Embedded Software for the Internet of Things @ UniTN fall 2021, made by me Eric

It consists of conneting the MSP432P401R and the NodeMCU ESP8266

## HW Requirements
For this project it is used:
- Texas Instrument MSP432P401R LaunchPad Development Kit
- Texas Instrument BoosterPack MK II
- NodeMCU Amica ESP8266
- A breadboard
- Some jumpers
- USB cable

## SW Requirements
- [Code Composer Studio](https://www.ti.com/tool/CCSTUDIO) v10.4.0.00006 
- [NodeMCU PyFlasher](https://github.com/marcelstoer/nodemcu-pyflasher)
- Apache httpd v2.4.51
- Telegram Bot API key

## Setting up
### MPS432 Firmware
For this project, the NodeMCU has to have flashed [this firmware](https://docs.ai-thinker.com/en/esp8266/sdk), **download** the one with this header:
* ESP8266 MQTT transparently transmits AT firmware (AT port is changed to UART0)

It is necessary for being able to communicate with the NodeMCU ESP8266 with AT commands through UART.
After downloading it, **flash** the NodeMCU ESP8266 with it using the **NodeMCU PyFlasher**
> You can test it using a serial console with baud rate of 115200 and both CR and LF (like the one in the Arduino IDE) and type `AT`, if the answer is `OK` that's all set!
### Apache httpd
> The purpose of utilizing a Apache httpd is to use it as a Proxy Server for decoupling the HTTPS requests from the NodeMCU that are too memory intensive for the latter.
> So, the NodeMCU will do normal HTTP requests that will be handled by the Proxy and redirected as HTTPS to the webisites.

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
Add this at the bottom of the file replacing YOUR_KEY with your Telegram Bot API key
```
<IfModule mod_proxy.c>

ProxyRequests Off

<Proxy *>
Order deny,allow
Allow from all
</Proxy>

SSLProxyEngine on
ProxyPass /api https://api.telegram.org/botYOUR_KEY/getMe
ProxyPassReverse /api https://api.telegram.org/botYOUR_KEY/getMe

</IfModule>

<Proxy *>
```
Then **run** the proxy server
`sudo brew services start httpd`

The **default port** for httpd is **8080**, and it's the one used in this project.
Remember to have a **static IP address on the PC running the Proxy** (the one that I use will be 192.168.1.32)

### Code Composer Studio
This project makes use of [TI's DriverLib](https://www.ti.com/tool/MSPDRIVERLIB): download it and link the files required for interacting with the BoosterPack MK II sensors and establishing a connection with the ESP8266 through UART communication: go to the Project's properties, and:

- Under Arm Compiler/Include Options include the path to your DriverLib installation (<path>\simplelink_msp432p4_sdk_3_40_01_02\source).
- Under Arm Linker/File Search Path include the DriverLib (<path>\simplelink_msp432p4_sdk_3_40_01_02\source\ti\devices\msp432p4xx\driverlib\ccs\msp432p4xx_driverlib.lib).
  
This project contains some readapted libraries (to interface with the NodeMCU ESP8266 through UART) found [here](https://github.com/amartinezacosta/MSP432-ESP8266), they are inside the CCS project with these names:
* Devices
* Hardware
  
### MSP432P401R and NodeMCU ESP8266 connections
  Connect directly with some jumpers, or with a breadboard, the two microcontrollers like so:
  ```/* VCC = 3.3V
 * GND = GND
 * TX = P3.2
 * RX = P3.3
 * RST = P6.1 */
  ```
 ![Image showing the pinout of ESP8266](https://i2.wp.com/randomnerdtutorials.com/wp-content/uploads/2019/05/ESP8266-NodeMCU-kit-12-E-pinout-gpio-pin.png?quality=100&strip=all&ssl=1)

### UART
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
  ``
