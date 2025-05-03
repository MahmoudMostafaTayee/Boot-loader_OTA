/**
 * @file BL.c
 * @author Mahmoud Mostafa Tayee (mahmoud.tayee.1994@gmail.com)
 * @brief Implementation of the boot loader with OTA update.
 * @details This module provides the functions for the boot loader with using UART for OTA update.
 * @version 0.1
 * @date 2025-05-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/hw_gpio.h"
#include "drivers/buttons.h"
#include "BL.h"
#include "../common/common.h"

#ifdef DEBUG
/**
 * @brief Error function to be called if a driver library error occurs.
 *
 * This function will be called if a driver library function encounters an
 * error.  The function will print out an error message and then loop
 * indefinitely.
 *
 * @param pcFilename The name of the file in which the error occurred.
 *
 * @param ui32Line The line number on which the error occurred.
 */
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

/* ***********************************************************************************/
/* Global variables                                                                  */
/* ***********************************************************************************/
volatile uint16_t gu16_TickCount = 0;
static const uint32_t gau32_bl_version[2] = {MAJOR, MINOR}; 

/* ***********************************************************************************/
/* Functions prototypes */
/* ***********************************************************************************/
void ConfigureOTAcommUART(void);
void ConfigureDbgUART(void);
void clkConfiguration(uint32_t u32_peripherals);
void LedsInit(void);
void SysTickIntHandler(void);
void InitSysTick(void);
void app_init(void);
static void jump_to_app(void);
retval_t OTA_update(void);

/* ***********************************************************************************/
/* Functions definitions */
/* ***********************************************************************************/
/**
 * @brief Configure the UART and its pins for OTA communication.
 *
 * Enable the GPIO Peripheral used by the UART.
 * Enable UART0.
 * Configure GPIO Pins for UART mode.
 * Use the internal 16MHz oscillator as the UART clock source.
 */
void ConfigureOTAcommUART(void)
{
    // Enable the GPIO Peripheral used by the UART.
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Enable UART0
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure GPIO Pins for UART mode.
    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
}

/**
 * @brief Configure the UART2 peripheral to be used for debug output.
 *
 * Enable the GPIO Peripheral used by the UART.
 * Enable UART2.
 * Configure GPIO Pins for UART mode.
 * Use the internal 16MHz oscillator as the UART clock source.
 * Initialize the UART for console I/O.
 */
void ConfigureDbgUART(void)
{
    // Enable the GPIO Peripheral used by the UART.
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    // Enable UART2
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);

    // Wait for the peripherals to be ready
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));
    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART2));

    // Unlock PD7 (NMI pin) so it can be used as UART
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= GPIO_PIN_7;
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;

    // Configure GPIO Pins for UART mode
    MAP_GPIOPinConfigure(GPIO_PD6_U2RX);
    MAP_GPIOPinConfigure(GPIO_PD7_U2TX);
    MAP_GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    // Use the internal 16MHz oscillator as the UART clock source
    UARTClockSourceSet(UART2_BASE, UART_CLOCK_PIOSC);

    // Initialize the UART for console I/O
    UARTStdioConfig(2, BL_UART_BAUD_RATE, 16000000);
}

/**
 * @brief Configures the system clock and enables the specified peripheral.
 *
 * @param u32_peripherals The peripheral to be enabled.
 * 
 * @details This function sets the system clock to use a 16 MHz crystal with a
 *          phase-locked loop (PLL) and a system divider of 4, resulting in a
 *          100 MHz system clock. It also enables the specified GPIO peripheral
 *          required for the on-board LED and button functionality.
 */
void clkConfiguration(uint32_t u32_peripherals)
{
    // Set the system clock to 100 MHz using PLL and 16 MHz crystal
    MAP_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    // Enable the GPIO port that is used for the on-board LED + button.
    MAP_SysCtlPeripheralEnable(u32_peripherals);
}


/**
 * @brief Initializes the LEDs on the board.
 *
 * @details This function enables the GPIO port to which the LEDs are connected
 *          and configures the GPIO pins for the LEDs as output pins.
 */
void LedsInit(void)
{
    // Enable the GPIO port to which the leds are connected.
    /* This step is being done within clock configuration. */

    // Enable the GPIO pins for all LEDs.
    MAP_GPIOPinTypeGPIOOutput(LEDS_GPIO_BASE, BOARD_LED_PINS);
}


/**
 * @brief Increments the system tick count variable.
 *
 * This is the interrupt handler for the system tick counter. It increments
 * the gu16_TickCount variable on every interrupt.
 */
void SysTickIntHandler(void)
{
    gu16_TickCount++;
}


/**
 * @brief Initializes the SysTick timer for periodic interrupts.
 * 
 * This function configures the SysTick timer to generate interrupts at a 
 * frequency of 1 kHz (every 1 ms). It sets the period, registers the 
 * interrupt handler, enables the SysTick interrupt, and starts the SysTick 
 * timer.
 */

void InitSysTick(void)
{
    SysTickPeriodSet(SysCtlClockGet() / 1000); // 1 ms tick
    SysTickIntRegister(SysTickIntHandler);
    SysTickIntEnable();
    SysTickEnable();
}


/**
 * @brief Initialization of the application.
 *
 * Initializes the peripherals used in the application, initializes
 * the SysTick timer for the application, and initializes the LEDs and
 * buttons. Finally, initializes the DBG UART and the OTA comm. UART
 * for the BL.
 */
void app_init(void){
    uint32_t u32_peripherals = 0x00;

    // Enable the GPIO port that is used for the on-board LED + button.
    u32_peripherals |= (BUTTONS_GPIO_PERIPH | LEDS_GPIO_PERIPH);
    
    // Configure clock for the system + used peripherals.
    clkConfiguration(u32_peripherals);

    InitSysTick();

    // Initialize the Buttons and LEDs.
    ButtonsInit();
    LedsInit();
    
    // Initialize the DBG UART.
    ConfigureDbgUART();

    // Initialize the BL comm. UART.
    ConfigureOTAcommUART();
}


/**
 * @brief Jumps to the Main app's reset handler.
 *
 * This function is called at the end of the BL's main loop to jump to the
 * Main app's reset handler. first, it turns off the
 * BL(RED) LED, and finally calls the Main app's reset handler.
 *
 */
static void jump_to_app(void)
{
    LOG("Jumping to app\n");

    /* Jump to Main app's reset handler. */
    void (*app_reset_handler) () = (void(*)())(*((volatile uint32_t*)MAIN_APP_RESET_HNDL_ADD));

    /* Turn off the BL pin. */
    GPIOPinWrite(LEDS_GPIO_BASE, BOARD_LED_PINS, 0);

    /* Call the Main application. */
    app_reset_handler();
}

retval_t OTA_update(void) { 
    return RETVAL_SUCCESS; 
}
/**
 * @brief Main app
 * 
 * @return int I believe I don't need to tell you what is this for :)
 */
int main(void)
{
    app_init();

    tenu_BLstate enu_BLstate = BL_STARTED;

    // BL!
    LOG("------------------------------------------------------------\n");
    LOG("------------------------------------------------------------\n");
    LOG("BL world!\n");
    LOG("Starting BL version %d.%d\n", gau32_bl_version[0], gau32_bl_version[1]);
    LOG("------------------------------------------------------------\n");
    LOG("------------------------------------------------------------\n");

    // Turn on the BL(RED) LED.
    GPIOPinWrite(LEDS_GPIO_BASE, BOARD_LED_PINS, BL_LED);

    uint16_t u16_startTime = gu16_TickCount;
    LOG("Press the SW2 button to enter the OTA upgrade mode!\r\n");

    while(1)
    {
        uint8_t delta, state;

        state = ButtonsPoll(&delta, NULL); // NULL = don't care about raw state
        
        if (delta & BL_BTN) {           // Only if state changed
            if (state & BL_BTN) {
                LOG("OTA update request!\r\n");
                enu_BLstate = OTA_UPDATE_REQUESTED;
                break;
            } else {
                LOG("SW1 Released!\r\n");
            }
        }
        else if ((uint16_t)(gu16_TickCount - u16_startTime) >= 3000){
            enu_BLstate = NORMAL_APP_START;
            break;
        }
    }

    if(OTA_UPDATE_REQUESTED == enu_BLstate){
        LOG("Starting OTA update!\r\n");
        GPIOPinWrite(LEDS_GPIO_BASE, BOARD_LED_PINS, BLUE_LED);
        DELAY_S(1); /* Delay for a bit. */

        GPIOPinWrite(LEDS_GPIO_BASE, BOARD_LED_PINS, GREEN_LED);
        DELAY_S(1); /* Delay for a bit. */

        GPIOPinWrite(LEDS_GPIO_BASE, BOARD_LED_PINS, BLUE_LED);
        DELAY_S(1); /* Delay for a bit. */


        if(OTA_update() == RETVAL_SUCCESS){
            LOG("OTA update success!\r\n");
            GPIOPinWrite(LEDS_GPIO_BASE, BOARD_LED_PINS, GREEN_LED);
            DELAY_S(3); /* Delay for a bit. */
            SysCtlReset();
        }
        else{
            LOG("OTA update failed!\r\n");
            while(1);
        }
    }
    else if(NORMAL_APP_START == enu_BLstate){
        // Here we jump to the main app.
        jump_to_app();
    }
}
