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
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
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
struct usart_module usart_instance;
void configure_usart(void)
{
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);
	config_usart.baudrate    = 9600;
	config_usart.mux_setting = USART_RX_3_TX_2_XCK_3;
	config_usart.pinmux_pad0 = PINMUX_UNUSED;
	config_usart.pinmux_pad1 = PINMUX_UNUSED;
	config_usart.pinmux_pad2 = PINMUX_PB10D_SERCOM4_PAD2;
	config_usart.pinmux_pad3 = PINMUX_PB11D_SERCOM4_PAD3;
	//while (usart_init(&usart_instance,
	//SERCOM4, &config_usart) != STATUS_OK) {
	//}
	stdio_serial_init(&usart_instance, SERCOM4, &config_usart);
	usart_enable(&usart_instance);
}
int main (void)
{
	system_init();
	delay_init();
	configure_usart();
	
	 //uint8_t string[] = "Hello World!\r\n";
	 //usart_write_buffer_wait(&usart_instance, string, sizeof(string));
//

	int x = 0;
	x += 2;
	x += 4;
	x += 6;
	if (x == 10) {
		x = 4;
	}
	
	while(true) {
		 //uint8_t string[] = "Hello World!\r\n";
		 //usart_write_buffer_wait(&usart_instance, string, sizeof(string));
		 printf("hello\r\n");
		 delay_ms(500);
	}
	/* Insert application code here, after the board has been initialized. */
}
