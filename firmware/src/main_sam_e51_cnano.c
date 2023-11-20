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
bool Err;
int8_t heart_rate;
char HR_data[2] = ""; 

typedef enum
{
    /* TODO: Define states used by the application state machine. */
    RNBD_INIT,
    RNBD_FACTORY_RESET,
    RNBD_CMD,
    RNBD_CMD1,
    RNBD_CMD2,
    RNBD_CMD3,
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
    RNBD_SEND_HRBS,
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

void RNBD_heartrate_example(void)
{
    switch(rnbd_state.state)
    {
        case RNBD_INIT:
        {
            Err=RNBD_Init();
            if(Err)
            {
                Err=false;
                printf("RNBD451_INITIALIZING\r\n");
                rnbd_state.state=RNBD_CMD;        
            }
        }
        break;
        case RNBD_CMD:
        {
            Err=RNBD_EnterCmdMode();
            if(Err)
            {
                Err=false;                
                printf("Entered CMD Mode\r\n");
                rnbd_state.state=RNBD_FACTORY_RESET;
            }
        }
        break;
        case RNBD_FACTORY_RESET:
        {
            Err=RNBD_FactoryReset();
            if(Err)
            {
                Err=false;                
                printf("Factory Reset Done\r\n");
                rnbd_state.state=RNBD_CMD1;
            }
        }
        break;
        case RNBD_CMD1:
        {
            Err=RNBD_EnterCmdMode();
            if(Err)
            {
                Err=false;                
                printf("Entered CMD Mode\r\n");
                rnbd_state.state=RNBD_SET_NAME;
            }
        }
        break;
        case RNBD_SET_NAME:
        {
            Err=RNBD_SetName(DevName,strlen(DevName));
            if(Err)
            {
                Err=false;                
                printf("Device Name Set\r\n");
                rnbd_state.state=RNBD_SET_PROFILE;
            }
        }
        break;
        case RNBD_SET_PROFILE:
        {
            Err=RNBD_SetServiceBitmap(service_uuid);
            if(Err)
            {
                Err=false;                
                printf("Service Bitmap Set\r\n");
                rnbd_state.state=RNBD_SET_APPEARANCE;
            }
        }        
        break;
        case RNBD_SET_APPEARANCE:
        {
            char appearance[]="0340";
            Err=RNBD_SetAppearance(appearance,strlen(appearance));
            if(Err)
            {
                Err=false;                
                printf("Appearance Set\r\n");
                rnbd_state.state=RNBD_REBOOT;
            }
        }
        break;
        case RNBD_REBOOT:
        {
            Err=RNBD_RebootCmd();
            if(Err)
            {
                Err=false;                
                printf("Reboot Completed\r\n");
                rnbd_state.state=RNBD_CMD2;
            }
        }
        break;
        case RNBD_CMD2:
        {
            Err=RNBD_EnterCmdMode();
            if(Err)
            {
                Err=false;                
                printf("Entered CMD Mode\r\n");
                rnbd_state.state=RNBD_SERVICE_UUID;
            }
        }
        break;
        case RNBD_SERVICE_UUID:
        {
            char PS[]="180D";
            Err=RNBD_SetServiceUUID(PS,strlen(PS));
            if(Err)
            {
                Err=false;                
                printf("Service UUID Set\r\n");
                rnbd_state.state=RNBD_SERVICE_CHARACTERISTICS;
            }
        }
        break;
        case RNBD_SERVICE_CHARACTERISTICS:
        {
            char PC[]="2A37,10,05";
            Err=RNBD_SetServiceCharacteristic(PC,strlen(PC));
            if(Err)
            {
                Err=false;                
                printf("Service Characteristics Set\r\n");
                rnbd_state.state=RNBD_SERVICE_CHARACTERISTICS1;
            }
        }
        break;
        case RNBD_SERVICE_CHARACTERISTICS1:
        {
            char PC1[]="2A38,02,05";
            Err=RNBD_SetServiceCharacteristic(PC1,strlen(PC1));
            if(Err)
            {
                Err=false;                
                printf("Service Characteristics Set\r\n");
                rnbd_state.state=RNBD_SERVICE_CHARACTERISTICS2;
            }
        }
        break;
        case RNBD_SERVICE_CHARACTERISTICS2:
        {
            char PC2[]="2A39,08,05";
            Err=RNBD_SetServiceCharacteristic(PC2,strlen(PC2));
            if(Err)
            {
                Err=false;                
                printf("Service Characteristics Set\r\n");
                rnbd_state.state=RNBD_REBOOT1;
            }
        }
        break;
        case RNBD_REBOOT1:
        {
            Err=RNBD_RebootCmd();
            if(Err)
            {
                Err=false;                
                printf("Reboot Completed\r\n");
                rnbd_state.state=RNBD_CMD3;
            }
        }
        break;
        case RNBD_CMD3:
        {
            Err=RNBD_EnterCmdMode();
            if(Err)
            {
                Err=false;                
                printf("Entered CMD Mode\r\n");
                rnbd_state.state=RNBD_SEND_HRBS;
            }
        }
        break;
        case RNBD_SEND_HRBS:
        {
            char handle[]="1005,03";
            Err=RNBD_WriteLocalCharacteristic(handle,strlen(handle),0,0);
            if(Err)
            {
            Err=false;
            printf("HRBS Set\r\n");
            printf("Press the User Button to Start Measuring the Heart Rate\r\n");
            rnbd_state.state=RNBD_WAIT;  
            }
        }
        break;
        case RNBD_WAIT:
        {
            
            Err=false;
            
            if(switchPressEvent == true)
            {
                switchPressEvent = false;
                printf("!!! Measuring Heart Rate !!!\r\n");                
                heart_rate = heartrate9_example();                
                if(heart_rate != -1)
                {                
                    rnbd_state.state=RNBD_SEND_HR_DATA;
                }
                else
                {
                    rnbd_state.state=RNBD_WAIT;
                }         
            }
            else
            {
               rnbd_state.state=RNBD_WAIT; 
            }
        }
        break;
        case RNBD_SEND_HR_DATA:
        {
            char handle[]="1002";
            sprintf(HR_data,"%x",heart_rate);
            Err=RNBD_WriteLocalCharacteristic(handle,strlen(handle),HR_data,2);
            if(Err)
            {
            Err=false;
            rnbd_state.state=RNBD_WAIT;  
            }
        }
        break;
    }    
}


int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );   
    EIC_CallbackRegister(EIC_PIN_15, Switch_Press_Handler, 0);
    printf("RNBD451_HEART_RATE_PROFILE\r\n");
    rnbd_state.state=RNBD_INIT;

    while ( true )
    {
        RNBD_heartrate_example();
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

