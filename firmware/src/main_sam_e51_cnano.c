/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main_sam_e51_cnano.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/
// DOM-IGNORE-BEGIN
/*
    (c) 2021 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "click_routines/heartrate9/heartrate9_example.h"
#include "string.h"
#include "config/sam_e51_cnano/rnbd/rnbd_interface.h"
#include "rnbd/rnbd.h"

const char DevName[]="RNBD451_HEART_RATE";
uint8_t service_uuid=0xC0;
bool init;
bool data;
extern char HR_data[];

typedef enum
{
    /* TODO: Define states used by the application state machine. */
    RNBD_INIT,
    RNBD_FACTORY_RESET,
    RNBD_CMD,
    RNBD_CMD1,
    RNBD_CMD2,
    RNBD_SET_NAME,
    RNBD_SET_PROFILE,
    RNBD_SET_APPEARANCE,
    RNBD_REBOOT,
    RNBD_REBOOT1,      
    RNBD_SERVICE_UUID,
    RNBD_SERVICE_CHARACTERISTICS,
    RNBD_SERVICE_CHARACTERISTICS1,
    RNBD_SERVICE_CHARACTERISTICS2,
    RNBD_SEND_HR_DATA, 
    RNBD_WAIT,   
            
} STATES;

typedef struct
{
    /* The application's current state */
    STATES state;
    
} RNBD_STATE;

RNBD_STATE rnbd_state;
volatile bool switchPressEvent = false;

static void Switch_Press_Handler(uintptr_t context)
{
    switchPressEvent = true;
}

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

void RNBD_HEART_RATE(void)
{
    switch(rnbd_state.state)
    {
        case RNBD_INIT:
        {
            if(init)
            {
                data=false;
                data=RNBD_EnterCmdMode();
                printf("Initialized RNBD\r\n");
                rnbd_state.state=RNBD_FACTORY_RESET;
            }
        }
        break;
        case RNBD_FACTORY_RESET:
        {
            if(data)
            {
                data=false;
                data=RNBD_FactoryReset();
                printf("Entered CMD\r\n");
                rnbd_state.state=RNBD_CMD;
            }
        }
        break;
        case RNBD_CMD:
        {
            if(data)
            {
                data=false;
                data=RNBD_EnterCmdMode();
                printf("Factory Reset Done\r\n");
                rnbd_state.state=RNBD_SET_NAME;
            }
        }
        break;
        case RNBD_SET_NAME:
        {
            if(data)
            {
                data=false;
                data=RNBD_SetName(DevName,strlen(DevName));
                printf("Entered CMD\r\n");
                rnbd_state.state=RNBD_SET_PROFILE;
            }
        }
        case RNBD_SET_PROFILE:
        {
            if(data)
            {
                data=false;
                data=RNBD_SetServiceBitmap(service_uuid);
                printf("Name Set\r\n");
                rnbd_state.state=RNBD_SET_APPEARANCE;
            }
        }
        break;
        break;
        case RNBD_SET_APPEARANCE:
        {
            if(data)
            {
                data=false;
                char appearance[]="0340";
                data=RNBD_SetAppearance(appearance,strlen(appearance));
                printf("Device Name Set\r\n");
                rnbd_state.state=RNBD_REBOOT;
            }
        }
        break;
        case RNBD_REBOOT:
        {
            if(data)
            {
                data=false;
                data=RNBD_RebootCmd();
                printf("Appearance Set\r\n");
                rnbd_state.state=RNBD_CMD1;
            }
        }
        break;
        case RNBD_CMD1:
        {
            if(data)
            {
                data=false;
                data=RNBD_EnterCmdMode();
                printf("Reboot Completed\r\n");
                rnbd_state.state=RNBD_SERVICE_UUID;
            }
        }
        break;
        case RNBD_SERVICE_UUID:
        {
            if(data)
            {
                data=false;
                char PS[]="180D";
                data=RNBD_SetServiceUUID(PS,strlen(PS));
                printf("Entered CMD\r\n");
                rnbd_state.state=RNBD_SERVICE_CHARACTERISTICS;
            }
        }
        break;
        case RNBD_SERVICE_CHARACTERISTICS:
        {
            if(data)
            {
                data=false;
                char PC[]="2A37,10,05";
                data=RNBD_SetServiceCharacteristic(PC,strlen(PC));
                printf("Service UUID Set\r\n");
                rnbd_state.state=RNBD_SERVICE_CHARACTERISTICS1;
            }
        }
        break;
        case RNBD_SERVICE_CHARACTERISTICS1:
        {
            if(data)
            {
                data=false;
                char PC1[]="2A38,02,05";
                data=RNBD_SetServiceCharacteristic(PC1,strlen(PC1));
                printf("Service Characteristics Set\r\n");
                rnbd_state.state=RNBD_SERVICE_CHARACTERISTICS2;
            }
        }
        break;
        case RNBD_SERVICE_CHARACTERISTICS2:
        {
            if(data)
            {
                data=false;
                char PC2[]="2A39,08,05";
                data=RNBD_SetServiceCharacteristic(PC2,strlen(PC2));
                printf("Service Characteristics Set\r\n");
                rnbd_state.state=RNBD_REBOOT1;
            }
        }
        break;
        case RNBD_REBOOT1:
        {
            if(data)
            {
                data=false;
                data=RNBD_RebootCmd();
                printf("Service Characteristics Set\r\n");
                rnbd_state.state=RNBD_CMD2;
            }
        }
        break;
        case RNBD_CMD2:
        {
            if(data)
            {
                data=false;
                data=RNBD_EnterCmdMode();
                printf("Reboot Completed\r\n");
                rnbd_state.state=RNBD_WAIT;
            }
        }
        case RNBD_WAIT:
        {
            
            data=false;
            
            if(switchPressEvent == true)
            {
                printf("!!! Measuring Heart Rate !!!\r\n");
                switchPressEvent = false;
                heartrate9_example();
                rnbd_state.state=RNBD_SEND_HR_DATA;
            }
            else
            {
               rnbd_state.state=RNBD_WAIT; 
            }
        }
        break;
        case RNBD_SEND_HR_DATA:
        {
            data=false;
            char handle[]="1002";            
            data=RNBD_WriteLocalCharacteristic(handle,strlen(handle),HR_data,2);
            rnbd_state.state=RNBD_WAIT;
            
        }
        break;
    }    
}


int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    printf("RNBD451_HEART_RATE_PROFILE\r\n");
    init=RNBD_Init();
    if(init == true)
    {
        printf("RNBD451_INITIALIZED\r\n");
        rnbd_state.state=RNBD_INIT;        
    }
    else
    {
        printf("RNBD451_NOT_INITIALIZED\r\n");
    }
    
    EIC_CallbackRegister(EIC_PIN_15, Switch_Press_Handler, 0);

    while ( true )
    {
        RNBD_HEART_RATE();
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

