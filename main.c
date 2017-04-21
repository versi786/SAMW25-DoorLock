/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * This is a bare minimum user application template.
 *
 * For documentation of the board, go \ref group_common_boards "here" for a link
 * to the board-specific documentation.
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# Basic usage of on-board LED and button
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <port.h>
#include <delay.h>
#include <io.h>
#include <stdio.h>
#include <clock.h>
#include <systick_counter.h>
#include <stdio_serial.h>
#include <usart.h>
//#include <extint.h>
//#include <avr/interrupt.h>
//#include <system_interrupt.h>
//#include <interrupt/interrupt_avr.h>

#define YES	1
#define NO	1
//#define TRIGPIN port_pin_set_config(PORTA, 20)
#define TRIGPIN PIN_PA20
#define ECHOPIN PIN_PA21

int flag_ECHOPIN_Transition = 0;
int duration  =0;

//ISR(INT0_vect)
//{
	static unsigned long long count_duration =0;
	
	
	void configure_port_pins(void)
	{
		struct port_config config_port_pin;
		port_get_config_defaults(&config_port_pin);
		config_port_pin.direction = PORT_PIN_DIR_INPUT;
		config_port_pin.input_pull = PORT_PIN_PULL_UP;
		port_pin_set_config(ECHOPIN, &config_port_pin);
		config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	//	config_port_pin.output_pull = PORT_PIN_PULL_UP;
		port_pin_set_config(TRIGPIN, &config_port_pin);
		
		
	}
	
//	if(PORTB == HIGH)
//	{
//		count_duration ++;
//		flag_ECHOPIN_Transition = NO;
//	}
//	else
//	{
//		duration  = count_duration; //duration of ECHO pin being high
//		flag_ECHOPIN_Transition = YES;
//	}
//}

int main (void)
{
	system_init();
	delay_init();
	system_interrupt_enable_global();
	//configure_usart();
	configure_port_pins();

	//DDRD = 1<<PA21_ECHO;		// Set PA21 as input (Using for interupt INT0)

//	GICR = 1<<INT0;					// Enable INT0
//	MCUCR = 0<<ISC01 | 1<<ISC00;	// Trigger INT0 on rising and falling edge
	
//	sei();				//Enable Global Interrupt

	/* Insert application code here, after the board has been initialized. */

	/* This skeleton code simply sets the LED to the state of the button. */
	while (1) {
		//set TRIG as output
		//set echo as input
		//make TRIG low
		//delay
		//make TRIG HIGH
		//delay
		
		//_extint_enable();
		
		//system_pinmux_pin_set_config(TRIGPIN, PORT_PIN_DIR_OUTPUT) ;
		//system_pinmux_pin_set_config(ECHOPIN, PORT_PIN_DIR_INPUT) ;
		port_pin_set_output_level(TRIGPIN, LOW);
		delay_us(5);
		port_pin_set_output_level(TRIGPIN, HIGH);
		//start timer for ECHO pin
	    delay_us(10);
		port_pin_set_output_level(TRIGPIN, LOW);
	//	delay_us(10);
		//duration is time for which echo pin is HIGH
	//	bool statusECHO;
	//	statusECHO = port_pin_get_input_level(ECHOPIN);
		//printf("%s\n", statusECHO ? "true" : "false");
	//	printf("%d\n", (int *)&statusECHO);
		if(port_pin_get_input_level(ECHOPIN) == HIGH)
		{
			count_duration ++;
		}
		else
		{
			if(count_duration !=0)
			{
				duration = count_duration;
				flag_ECHOPIN_Transition = YES;
			}
		}
		if(flag_ECHOPIN_Transition == YES)
		{
			printf("duration is %d",duration);
			//_extint_disable();
		}

		/* Is button pressed? */
		if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			/* Yes, so turn LED on. */
			port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
		} else {
			/* No, so turn LED off. */
			port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
		}
	}
}
