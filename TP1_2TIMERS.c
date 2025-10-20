#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

uint32_t frec_muestreo = 5;
uint32_t conteo_match = 0;
uint32_t conv_enable = 1;
uint32_t modo = 0;
uint32_t index = 0;
uint16_t muestra = 0;
int16_t datos_q15[512];

void Apago_Leds();
void Prendo_Rojo();
void MostrarDAC();

int main(void) {

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitLEDsPins();
	BOARD_InitBUTTONsPins();
	BOARD_InitDACPins();
	BOARD_InitADCPins();
    BOARD_InitBootPeripherals();
    CTIMER_StopTimer(CTIMER1);

#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    BOARD_InitDebugConsole();
#endif

    while(1) {}
    return 0 ;
}

void Apago_Leds(){
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_GREEN_GPIO, BOARD_INITLEDSPINS_LED_GREEN_PIN, 1);
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_BLUE_GPIO, BOARD_INITLEDSPINS_LED_BLUE_PIN, 1);
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_RED_GPIO, BOARD_INITLEDSPINS_LED_RED_PIN, 1);
}

void Prendo_Rojo(){
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_GREEN_GPIO, BOARD_INITLEDSPINS_LED_GREEN_PIN, 1);
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_BLUE_GPIO, BOARD_INITLEDSPINS_LED_BLUE_PIN, 1);
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_RED_GPIO, BOARD_INITLEDSPINS_LED_RED_PIN, 0);
}

void Prendo_Azul(){
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_GREEN_GPIO, BOARD_INITLEDSPINS_LED_GREEN_PIN, 1);
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_BLUE_GPIO, BOARD_INITLEDSPINS_LED_BLUE_PIN, 0);
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_RED_GPIO, BOARD_INITLEDSPINS_LED_RED_PIN, 1);
}

void Prendo_Verde(){
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_GREEN_GPIO, BOARD_INITLEDSPINS_LED_GREEN_PIN, 0);
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_BLUE_GPIO, BOARD_INITLEDSPINS_LED_BLUE_PIN, 1);
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_RED_GPIO, BOARD_INITLEDSPINS_LED_RED_PIN, 1);
}

void Prendo_Violeta(){
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_GREEN_GPIO, BOARD_INITLEDSPINS_LED_GREEN_PIN, 1);
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_BLUE_GPIO, BOARD_INITLEDSPINS_LED_BLUE_PIN, 0);
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_RED_GPIO, BOARD_INITLEDSPINS_LED_RED_PIN, 0);
}

void Prendo_Cyan(){
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_GREEN_GPIO, BOARD_INITLEDSPINS_LED_GREEN_PIN, 0);
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_BLUE_GPIO, BOARD_INITLEDSPINS_LED_BLUE_PIN, 0);
	GPIO_PinWrite(BOARD_INITLEDSPINS_LED_RED_GPIO, BOARD_INITLEDSPINS_LED_RED_PIN, 1);
}

void Match_44k(uint32_t flag){

	// Si el muestreo está habilitado, según la frecuencia seleccionada, gatilla el ADC con la periodicidad correspondiente
		conteo_match ++;
		switch(frec_muestreo){
		case(2):
		if(conteo_match==2){
			if(conv_enable == 1){
				LPADC_DoSoftwareTrigger(ADC0, 1U);
			}
			conteo_match = conteo_match % 2;
			MostrarDAC();
		}
		break;
		case(3):
		if(conv_enable == 1){
				LPADC_DoSoftwareTrigger(ADC0, 1U);
		}
		MostrarDAC();
		break;
		}
}

void Match_48k(uint32_t flag){

	// Si el muestreo está habilitado, según la frecuencia seleccionada, gatilla el ADC con la periodicidad correspondiente
		conteo_match++;
		switch(frec_muestreo){
		case(0):
				if(conteo_match==6){
					if(conv_enable == 1){
						LPADC_DoSoftwareTrigger(ADC0, 1U);
					}
					conteo_match = conteo_match % 6;
					MostrarDAC();
				}
		break;
		case(1):
				if(conteo_match==3){
					if(conv_enable == 1){
							LPADC_DoSoftwareTrigger(ADC0, 1U);
					}
					conteo_match = conteo_match % 3;
					MostrarDAC();
				}
		break;
		case(4):
				if(conv_enable == 1){
					LPADC_DoSoftwareTrigger(ADC0, 1U);
				}
				MostrarDAC();
		break;
		}
}

void GPIO0_INT_0_IRQHANDLER(void) {

	/* Get pin flags 0 */
	uint32_t pin_flags0 = GPIO_GpioGetInterruptChannelFlags(GPIO0, 0U);

	// No hace nada hasta que se presione el botón que comienza el muestreo
	// Actualiza la frecuencia de muestreo, eligiendo el clock correspondiente en cada caso y encendiendo el led correspondiente
		if(conv_enable){
			if(frec_muestreo != 5){
				switch(modo){
				case(0):
					Prendo_Azul();
					frec_muestreo = 1;//16kHz
				break;
				case(1):
					Prendo_Verde();
					frec_muestreo = 2;//22kHz
					CTIMER_StopTimer(CTIMER0);
					CTIMER_Reset(CTIMER1);
					CTIMER_StartTimer(CTIMER1);
				break;
				case(2):
					Prendo_Violeta();
					frec_muestreo = 3;//44kHz
				break;
				case(3):
					Prendo_Cyan();
					frec_muestreo = 4;//48kHz
					CTIMER_StopTimer(CTIMER1);
					CTIMER_Reset(CTIMER0);
					CTIMER_StartTimer(CTIMER0);
				break;
				case(4):
					Prendo_Rojo();
					frec_muestreo = 0;//8kHz
				}
				modo = (modo + 1)%5;	// Actualiza el buffer circular
				conteo_match = 0;		// Reinicia el valor de esta variable
			}
		}
	/* Clear pin flags 0 */
	GPIO_GpioClearInterruptChannelFlags(GPIO0, pin_flags0, 0U);
	  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
	     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
	  #if defined __CORTEX_M && (__CORTEX_M == 4U)
	    __DSB();
	  #endif
}

void GPIO0_INT_1_IRQHANDLER(void) {

  /* Get pin flags 1 */
  uint32_t pin_flags1 = GPIO_GpioGetInterruptChannelFlags(GPIO0, 1U);

  // Prepara el sistema para la adquisición de datos (solo la primera vez que se presiona el botón al encender el sistema)
	  if(frec_muestreo == 5){
		  frec_muestreo = 0;
		  conteo_match = 0;
		  Apago_Leds();
		  Prendo_Rojo();
	  }
	  else{
		  conv_enable = (conv_enable + 1)%2; //Cambia la bandera para habilitar el muestreo
		  conteo_match = 0;
	  }
  /* Clear pin flags 1 */
  GPIO_GpioClearInterruptChannelFlags(GPIO0, pin_flags1, 1U);

  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}

void ADC0_IRQHANDLER(void) {

  uint32_t trigger_status_flag;
  uint32_t status_flag;
  /* Trigger interrupt flags */
  trigger_status_flag = LPADC_GetTriggerStatusFlags(ADC0_PERIPHERAL);
  /* Interrupt flags */
  status_flag = LPADC_GetStatusFlags(ADC0_PERIPHERAL);
  /* Clears trigger interrupt flags */
  LPADC_ClearTriggerStatusFlags(ADC0_PERIPHERAL, trigger_status_flag);
  /* Clears interrupt flags */
  LPADC_ClearStatusFlags(ADC0_PERIPHERAL, status_flag);

  	// Si la conversión del ADC está terminada, se guarda el dato en el buffer ajustado a q15
		static lpadc_conv_result_t res;
		if(LPADC_GetConvResult(ADC0, &res, 0U)){

			muestra = (uint16_t)res.convValue;

			int16_t muestra_q15 = (int16_t)(muestra - 32768);

			datos_q15[index] = muestra_q15;
		}

  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}

void MostrarDAC(void){
	uint16_t valor_dac = (uint16_t)(datos_q15[index] + 32768);
	DAC_SetData(DAC0, valor_dac >> 4);	//Se ajusta a 12 bits el dato de formato q15 para el DAC
	index = (index + 1) % 512; 		//Se actualiza el índice de recorrido del buffer
}
