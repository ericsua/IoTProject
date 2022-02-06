#include "Hardware/CS_Driver.h"
#include "Devices/MSPIO.h"
#include "Devices/ESP8266.h"
#include "secrets.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include <string.h>


/* VCC = 3.3V
 * GND = GND
 * TX = P3.2
 * RX = P3.3
 * RST = P6.1
 * CH_PD = 3.3V */


/*We are connecting to api.themoviedb.org, ESP8266 can resolve DNS, pretty cool huh!*/
char HTTP_WebPage[] = "192.168.1.37"; //https://akabab.github.io/superhero-api/api/all.json
/*HTTP is TCP port 80*/
char Port[] = "3000";
/*Data that will be sent to the HTTP server. This will allow us to query movie data. Get an api key from api.themoviedb.org*/
char HTTP_Request[] = "GET /api HTTP/1.1\r\nHost: 192.168.1.37\r\n\r\n";
//char HTTP_Req[600] = "GET /api?data=";
//char HTTP_uest[] = " HTTP/1.1\r\nHost: 192.168.1.37\r\n\r\n";
char HTTP_Req[600] = "POST /api HTTP/1.1\r\nHost: 192.168.1.37\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ";
/*Subtract one to account for the null character*/
uint32_t HTTP_Request_Size = sizeof(HTTP_Request) - 1;

//variable to control the sending of data to server
int send = 0;


/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
eUSCI_UART_ConfigV1 UART0Config =
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

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
eUSCI_UART_ConfigV1 UART2Config =
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

//----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------MICROPHONE-------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------


void InitTimer() {
    Timer32_initModule(TIMER32_0_BASE,
                       TIMER32_PRESCALER_1,
                       TIMER32_32BIT,
                       TIMER32_PERIODIC_MODE);
}

void Timer1msStartOneShot() {
    Timer32_setCount(TIMER32_0_BASE, 300);   // 10 KHz (100 microsecond period)
    Timer32_startTimer(TIMER32_0_BASE, true);
}

int Timer1msExpiredOneShot() {
    return (Timer32_getValue(TIMER32_0_BASE) == 0);
}

Graphics_Context g_sContext;

void InitGraphics() {
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&g_sContext,
                         &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setFont(&g_sContext, &g_sFontCmtt40);
    Graphics_clearDisplay(&g_sContext);
}

void initADC() {
    ADC14_enableModule();

    // This sets the conversion clock to 3MHz
    ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC,
                     ADC_PREDIVIDER_1,
                     ADC_DIVIDER_1,
                      0
                     );

    ADC14_setResolution(ADC_14BIT);

    // This configures the ADC to store output results
    // in ADC_MEM0 (single-channel conversion, repeat mode)
    ADC14_configureSingleSampleMode(ADC_MEM0, true);

    // This configures the ADC in automatic conversion mode
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
}

void startADC() {
    // The ADC is in continuous sampling mode, so after calling this once
    // the ADC will continuously update
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();
}

void initMicrophone() {
    // This configures ADC_MEM0 to store the result from
    // input channel A10 (Microphone), in non-differential input mode
    // (non-differential means: only a single input pin)
    // The reference for Vref- and Vref+ are VSS and VCC respectively
    ADC14_configureConversionMemory(ADC_MEM0,
                                    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A10,
                                    ADC_NONDIFFERENTIAL_INPUTS);

    // This selects the GPIO as analog input
    // A9 is multiplexed on GPIO port P4 pin PIN4
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                                               GPIO_PIN3,
                                               GPIO_TERTIARY_MODULE_FUNCTION);
}

unsigned getSampleMicrophone() {
    return ADC14_getResult(ADC_MEM0);
}


unsigned sampleconv(unsigned v) {
    // This function maps on ADC sample (range 0 to 3FFF, midpoint 1FFF)
    // to the display Y coordinates (range 0 to 127, midpoint 64)
    // It also adds a digital gain factor
    int s0127 = (0x3FFF - v) / 128; // scaled 0 to 127, midpoint 64
    int gain = 4;
    return (unsigned) (s0127 - 64) * gain + 64;
}

char *ESP8266_Data;

void addSample(unsigned vx) {
    // This function queues up 128 samples
    // When 128 samples are received, it draws the buffer on the display
    // while removing the previous trace
    unsigned i;
    static unsigned count = 0;
    static unsigned countSend = 0;
    static unsigned oldsamplebuf[128];
    static unsigned newsamplebuf[128];

    newsamplebuf[count++] = vx;

    if (count == 128) {
        count  = 0;


        // buffer filled with 128 samples, dump to the display
        for (i=1; i<128; i++) {
            // remove the previous trace
            if(send == 0) {
                Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_RED);
                Graphics_drawLine(&g_sContext,
                                  i-1, sampleconv(oldsamplebuf[i-1]),
                                  i,   sampleconv(oldsamplebuf[  i]));
            } else {
                Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_BLUE);
                Graphics_drawLine(&g_sContext,
                                  i-1, sampleconv(oldsamplebuf[i-1]),
                                  i,   sampleconv(oldsamplebuf[  i]));
            }
           // put the new trace
           Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_YELLOW);
           //Graphics_drawString(&g_sContext, "SEND: OFF", 10, 10, 10, false);
           Graphics_drawLine(&g_sContext,
                             i-1, sampleconv(newsamplebuf[i-1]),
                            i,   sampleconv(newsamplebuf[  i]));


        }



        if(send == 1 && countSend++ == 10) {
            countSend = 0;
            char HTTP_Req[600] = "POST /api HTTP/1.1\r\nHost: 192.168.1.37\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ";
            //char HTTP_Req2[400] = "POST /api HTTP/1.1\r\nHost: 192.168.1.37\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 31\r\n\r\ndata=606060606060606060606060\r\n";
            char req[128*4];
            int index = 0;
            int ii = 0;
            for(ii=0; ii < 128; ii++) {
                index += sprintf(&req[index], "%d,", sampleconv(newsamplebuf[ii]));
                //printf("req %d: %s\n", ii,req);
                //printf("sample: %d\n", sampleconv(newsamplebuf[i]));
            }
            //printf("params: %s\n", req);
            //char finalReq[] = strcat(strcat(HTTP_Req, req), HTTP_uest);
            char sizePost[17];
            sprintf(sizePost, "%d\r\n\r\ndata=", strlen(req)+7);
            strcat(HTTP_Req, sizePost);
            strcat(HTTP_Req, req);
            strcat(HTTP_Req, "\r\n");
            //printf("finalReq: %s\n", HTTP_Req);
            if(!ESP8266_SendDataLoop('0', HTTP_Req, strlen(HTTP_Req)))
            //if(!ESP8266_SendData('0', HTTP_Request, sizeof(HTTP_Request)-1))
              {
                  //MSPrintf(EUSCI_A0_BASE, "Failed to send: %s to %s \r\nError: %s\r\n", HTTP_Request, HTTP_WebPage, ESP8266_Data);
                  printf("Failed to send: %s to %s \r\nError: \r\n", HTTP_Req, HTTP_WebPage);
                  while(1);
              }
            else {
                //printf("Sent: %s\n", HTTP_Req);
            }
            //printf("Data sent: %s to %s\r\n\r\nESP8266 Data Received: %s\r\n", HTTP_Req, HTTP_WebPage, ESP8266_Data);
        }

        // update the buffer
        for (i=0; i<128; i++)
          oldsamplebuf[i] = newsamplebuf[i];
    }
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void main(void)
{
	MAP_WDT_A_holdTimer();

    /*MSP432 Running at 24 MHz*/
	CS_Init();

	/*Initialize required hardware peripherals for the ESP8266*/
	UART_Init(EUSCI_A0_BASE, UART0Config);
	UART_Init(EUSCI_A2_BASE, UART2Config);
    GPIO_Init(GPIO_PORT_P6, GPIO_PIN1);

    MAP_Interrupt_enableMaster();

    //Setting buttons to send or not data to server (BUTTON S1 send) (BUTTON S2 do not send)
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN5);
    //GPIO_interruptEdgeSelect(GPIO_PORT_P3, GPIO_PIN5, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN5);

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    //GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN1);

    Interrupt_enableInterrupt(INT_PORT5);
    Interrupt_enableInterrupt(INT_PORT3);

    //Interrupt_setPriority(INT_PORT5, 255);
    //Interrupt_setPriority(INT_PORT3, 255);

    //Interrupt_enableMaster();

    /*Pointer to ESP8266 global buffer*/
    *ESP8266_Data = ESP8266_GetBuffer();
    printf("%p\n", ESP8266_Data);

    /*Hard Reset ESP8266*/
    ESP8266_HardReset();
    __delay_cycles(48000000);
    /*flush reset data, we do this because a lot of data received cannot be printed*/
    UART_Flush(EUSCI_A2_BASE);

    /*Check UART connection to MSP432*/
    if(!ESP8266_CheckConnection())
    {
        MSPrintf(EUSCI_A0_BASE, "Failed MSP432 UART connection\r\n");
        printf("Failed MSP432 UART connection, %d\r\n", ESP8266_CheckConnection());
        /*Trap MSP432 if failed connection*/
        while(1);
    }

    MSPrintf(EUSCI_A0_BASE, "Nice! We are connected to the MSP432\r\n\r\n");
    printf("Nice! We are connected to the ESP8266\r\n\r\n");
    MSPrintf(EUSCI_A0_BASE, "Checking available Access Points\r\n\r\n");
    printf("Checking available Access Points\r\n\r\n");

    if(!ESP8266_CWMODE()) {
        printf("Failed to change cwmode\r\n");
        while(1);
    }

    printf("ESP8266 is in client+server mode!\r\n");
    /*Check available Access Points*/
    if(!ESP8266_AvailableAPs())
    {
        MSPrintf(EUSCI_A0_BASE, "Failed to obtain Access Points\n\r ERROR: %s \r\n", ESP8266_Data);
        printf("Failed to obtain Access Points\n\r ERROR: %s \r\n", ESP8266_Data);
        /*Trap MSP432 if failed to obtain access points*/
        while(1);
    }

    /*Print received Access Points Data to console*/
    MSPrintf(EUSCI_A0_BASE, "Got it! Here are the available Access Points: %s\r\n\r\n", ESP8266_Data);
    printf("Got it! Here are the available Access Points: %s\r\n\r\n", ESP8266_Data);

    /*Connect to Access Point if necessary here*/
    if(!ESP8266_ConnectToAP(SSID, PWD)) // change SSID AND PWD with your Wi-Fi data
    {
        /*Trap MSP432 we are not connected*/
       printf("Can't connect to AP!\r\nERROR: %s \n", ESP8266_Data);
       while(1);
    }


    /*Enable multiple connections, up to 4 according to the internet*/
    if(!ESP8266_EnableMultipleConnections(true))
    {
        MSPrintf(EUSCI_A0_BASE, "Failed to set multiple connections\r\n");
        printf("Failed to set multiple connections\r\n");
        while(1);
    }

    MSPrintf(EUSCI_A0_BASE, "Multiple connections enabled\r\n\r\n");
    printf("Multiple connections enabled\r\n\r\n");


    /*
    if(!ESP8266_BUILTINLED())
    {
        MSPrintf(EUSCI_A0_BASE, "Failed to light up LED\r\n");
        printf("Failed to light up LED: %s\r\n\r\n", ESP8266_Data);
    }*/

    /*Try to establish TCP connection to a HTTP server*/
    if(!ESP8266_EstablishConnection('0', TCP, HTTP_WebPage, Port))
    {
        MSPrintf(EUSCI_A0_BASE, "Failed to connect to: %s\r\nERROR: %s\r\n", HTTP_WebPage, ESP8266_Data);
        printf("Failed to connect to: %s\r\nERROR: %s\r\n", HTTP_WebPage, ESP8266_Data);
        while(1);
    }

    MSPrintf(EUSCI_A0_BASE, "Connected to: %s\r\n\r\n", HTTP_WebPage);
    printf("Connected to: %s\r\n\r\n", HTTP_WebPage);



    /*Query data to connected HTTP server, in order to do this look for an API and request a key*/
    if(!ESP8266_SendData('0', HTTP_Request, HTTP_Request_Size))
    {
        MSPrintf(EUSCI_A0_BASE, "Failed to send: %s to %s \r\nError: %s\r\n", HTTP_Request, HTTP_WebPage, ESP8266_Data);
        printf("Failed to send: %s to %s \r\nError: %s\r\n", HTTP_Request, HTTP_WebPage, ESP8266_Data);
        while(1);
    }

    /*We succesfully sent the data to the HTTP server, we are now going to receive a lot of data from the ESP8266 (or depending
     * on what information you are querying, CHANGE ESP8266 BUFFER SIZE AND UARTA2 BUFFER SIZE, OTHERWISE YOU WILL LOSS DATA!!!!!).*/
    MSPrintf(EUSCI_A0_BASE, "Data sent: %s to %s\r\n\r\nESP8266 Data Received: %s\r\n", HTTP_Request, HTTP_WebPage, ESP8266_Data);
    printf("Data sent: %s to %s\r\n\r\nESP8266 Data Received: %s\r\n", HTTP_Request, HTTP_WebPage, ESP8266_Data);


    while(1)
    {
        /*You can query data here by sending the HTTP request string every n seconds.
         * Be careful about the amount of times per second that you can query data from an API
         */
        WDT_A_hold(WDT_A_BASE);
        unsigned vx;

        initADC();
        initMicrophone();
        InitGraphics();
        InitTimer();

        Timer1msStartOneShot();
        startADC();

        while (1) {
          if (Timer1msExpiredOneShot()) {
              vx = getSampleMicrophone();
              addSample(vx);
              Timer1msStartOneShot();

          }
        }
    }
}


// INTERRUPT HANDLERS FOR THE PUSHBUTTONS ON THE BOOSTERPACK XL

void PORT5_IRQHandler(void) {
    //printf("1 interrupt\n");

    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
    if (send == 1) return;
    if(status & GPIO_PIN1) {
        Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
        Graphics_clearDisplay(&g_sContext);
        //printf("button down");
        send = 1;
    }
}

void PORT3_IRQHandler(void) {

    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, status);
    if (send == 0) return;
    //printf("button up");

    if((status & GPIO_PIN5)) {
        Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_RED);
        Graphics_clearDisplay(&g_sContext);
        //printf("button up");
        send = 0;
    }

}


