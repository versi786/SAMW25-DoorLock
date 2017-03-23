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

#define boot_status_ADDR 0x1f00
#define INTEGRITY_CHECK 0xdead
#define APP_START_ADDR 0x2000
typedef struct bs {
	uint32_t integrity_check;
	uint32_t signature[3];
	uint8_t executing_image;
	uint8_t downlaoded_image;	
}boot_status;

boot_status default_boot_status = { .integrity_check = INTEGRITY_CHECK, .signature = {-1,-1,-1}, .executing_image = -1, .downlaoded_image = -1 };
	
typedef struct fh {
	uint16_t firmware_version;
	uint32_t checksum;
}firmware_header;

boot_status default_boot_status = { .integrity_check = INTEGRITY_CHECK, .signature = {-1,-1,-1}, .executing_image = -1, .downlaoded_image = -1 };

void configure_wdt(void)
{
 /* Create a new configuration structure for the Watchdog settings and fill
 * with the default module settings. */
 struct wdt_conf config_wdt;
 wdt_get_config_defaults(&config_wdt);
 /* Set the Watchdog configuration settings */
 config_wdt.enable = false;
 //config_wdt.timeout_period = WDT_PERIOD_2048CLK;
 //config_wdt.always_on = false;
 /* Initialize and enable the Watchdog with the user settings */
 wdt_set_config(&config_wdt);
}

void configure_nvm(void)
{
	struct nvm_config config_nvm;
	nvm_get_config_defaults(&config_nvm);
	config_nvm.manual_page_write = false;
	nvm_set_config(&config_nvm);
}

int main (void)
{
	system_init();

	// Initialization, there are no firmares and no boot_status
	//Disable watchdog timer
	configure_wdt();
	//Set up NVM Driver
	configure_nvm();
	boot_status status;
	nvm_read_buffer(boot_status_ADDR, (void *) &status, sizeof(status));
	if (status.integrity_check != INTEGRITY_CHECK) {
		// boot_status is not valid;
		status = default_boot_status;
		nvm_write_buffer(boot_status_ADDR, (void *) &status, sizeof(status));
	} else {
		if (status.downlaoded_image != -1){ {
			// Need to copy program from external flash to nvm
			
			
		}else if (status.executing_image != -1) {
			// we are running an application
			/* Pointer to the Application Section */
			void (*application_code_entry)(void);
			
			/* Rebase the Stack Pointer */
			__set_MSP(*(uint32_t *) APP_START_ADDR);
			
			/* Rebase the vector table base address */
			SCB->VTOR = ((uint32_t) APP_START_ADDR & SCB_VTOR_TBLOFF_Msk);
			
			/* Load the Reset Handler address of the application */
			application_code_entry = (void (*)(void))(unsigned *)(*(unsigned *)
			(APP_START_ADDR+ 4));
			
			/* Jump to user Reset Handler in the application */
			application_code_entry();

		} else {
			// no application to run
		}
		
	}
	
	
}
