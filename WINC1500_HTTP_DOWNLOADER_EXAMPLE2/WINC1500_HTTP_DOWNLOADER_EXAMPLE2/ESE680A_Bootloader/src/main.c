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




#include <board.h>
#include "at25dfx.h"


#define FLASH_SPI_SERCOM          SERCOM1
#define FLASH_SPI_CLOCK_SPEED     12000000 ///< Hz
#define FLASH_SPI_CLK_SOURCE      GCLK_GENERATOR_0


#define SERIALFLASH_SPI_MODULE      SERCOM1
#define SERIALFLASH_SPI_MUX_SETTING SPI_SIGNAL_MUX_SETTING_E
#define SERIALFLASH_SPI_PINMUX_PAD0 PINMUX_PA16C_SERCOM1_PAD0 /// MISO
#define SERIALFLASH_SPI_PINMUX_PAD1 PINMUX_UNUSED
#define SERIALFLASH_SPI_PINMUX_PAD2 PINMUX_PA18C_SERCOM1_PAD2 /// MOSI
#define SERIALFLASH_SPI_PINMUX_PAD3 PINMUX_PA19C_SERCOM1_PAD3 /// SCK
#define SERIALFLASH_SPI_CS          PIN_PA07                  /// CS


//! Select the SPI module AT25DFx is connected to
#define AT25DFX_SPI                 SERIALFLASH_SPI_MODULE

/** AT25DFx device type */
#define AT25DFX_MEM_TYPE            AT25DFX_081A

#define AT25DFX_SPI_PINMUX_SETTING  SERIALFLASH_SPI_MUX_SETTING
#define AT25DFX_SPI_PINMUX_PAD0     SERIALFLASH_SPI_PINMUX_PAD0
#define AT25DFX_SPI_PINMUX_PAD1     SERIALFLASH_SPI_PINMUX_PAD1
#define AT25DFX_SPI_PINMUX_PAD2     SERIALFLASH_SPI_PINMUX_PAD2
#define AT25DFX_SPI_PINMUX_PAD3     SERIALFLASH_SPI_PINMUX_PAD3

#define AT25DFX_CS                  SERIALFLASH_SPI_CS

//! SPI master speed in Hz.
#define AT25DFX_CLOCK_SPEED         1200000  /// 12000000



//! [buffers]
#define AT25DFX_BUFFER_SIZE  (10)

static uint8_t read_buffer[AT25DFX_BUFFER_SIZE];
static uint8_t write_buffer[AT25DFX_BUFFER_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//! [buffers]

//! [driver_instances]
struct spi_module at25dfx_spi;
struct at25dfx_chip_module at25dfx_chip;
//! [driver_instances]

// MEMORY MAP
// Code goest to ~ 0x3500 budget until 0x0FFF
// BOOT Status 4100 -> 41FF
// APP 5000 -> TOP
#define BOOT_STATUS_ADDR 0x4100 //boot status address in nvm
#define BASE_CODE_ADDR 0x5000 //nvm application start address
#define TOP_CODE_ADDR 0x8000  //nvm application end address
#define FLASH_FIRMWARE_HEADER_ADDR 0x2000  //address of header for fw1 in external flash
#define BASE_FLASH_CODE_ADDR 0x3000  //start address of fw 1 in external flash
#define TOP_FLASH_CODE_ADDR 0x6000   //start address of fw2 in in external flash
#define FLASH_TOP_FIRMWARE_HEADER_ADDR  0x5000  //address of header for fw2 in external flash
#define INTEGRITY_CHECK 0xdead
#define APP_START_ADDR 0x5000 //start address of application code in nvm (pointer to jump to in the application code)
typedef struct bs {
	uint32_t integrity_check;
	uint8_t signature[3];
	int8_t executing_image;
	int8_t downlaoded_image;	
	uint8_t arr[NVMCTRL_PAGE_SIZE-9]  
}boot_status;

boot_status default_boot_status = { .integrity_check = INTEGRITY_CHECK, .signature = {-1,-1,-1}, .executing_image = 1, .downlaoded_image = -1 };
	
typedef struct fh {
	uint16_t firmware_version;
	uint32_t checksum;
}firmware_header;

firmware_header default_firmware_header = { .firmware_version = -1, .checksum = -1 };
firmware_header fw_header;

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
//! [init_function]
static void at25dfx_init(void)
{
	//! [config_instances]
	struct at25dfx_chip_config at25dfx_chip_config;
	struct spi_config at25dfx_spi_config;
	//! [config_instances]

	//! [spi_setup]
	at25dfx_spi_get_config_defaults(&at25dfx_spi_config);
	at25dfx_spi_config.mode_specific.master.baudrate = AT25DFX_CLOCK_SPEED;
	at25dfx_spi_config.mux_setting = AT25DFX_SPI_PINMUX_SETTING;
	at25dfx_spi_config.pinmux_pad0 = AT25DFX_SPI_PINMUX_PAD0;
	at25dfx_spi_config.pinmux_pad1 = AT25DFX_SPI_PINMUX_PAD1;
	at25dfx_spi_config.pinmux_pad2 = AT25DFX_SPI_PINMUX_PAD2;
	at25dfx_spi_config.pinmux_pad3 = AT25DFX_SPI_PINMUX_PAD3;

	spi_init(&at25dfx_spi, AT25DFX_SPI, &at25dfx_spi_config);
	spi_enable(&at25dfx_spi);
	
	//! [spi_setup]

	//! [chip_setup]
	at25dfx_chip_config.type = AT25DFX_MEM_TYPE;
	at25dfx_chip_config.cs_pin = AT25DFX_CS;

	at25dfx_chip_init(&at25dfx_chip, &at25dfx_spi, &at25dfx_chip_config);

	//! [chip_setup]
}
//! [init_function]


void configure_nvm(void)
{
	struct nvm_config config_nvm;
	nvm_get_config_defaults(&config_nvm);
	config_nvm.manual_page_write = false;
	nvm_set_config(&config_nvm);
}
struct usart_module usart_instance;


int main (void)
{
	enum status_code error_code;
	//Set up NVM
	configure_nvm();
	system_init();
	delay_init();
	system_interrupt_enable_global();
	at25dfx_init();
	////TODO system clock
	//// Initialization, there are no firmwares and no boot_status
	////Disable watchdog timer
	configure_wdt();
	
	
	
	boot_status status;

	// read out existing boot status
	do
    {
        error_code = nvm_read_buffer(
                BOOT_STATUS_ADDR,
                (void *) &status, NVMCTRL_PAGE_SIZE);
    } while (error_code == STATUS_BUSY);
	if (status.integrity_check != INTEGRITY_CHECK) {
		// boot_status is not valid;
		status = default_boot_status;
		do
		{
			error_code = nvm_erase_row(
					BOOT_STATUS_ADDR);
		} while (error_code == STATUS_BUSY);
		do
		{
			error_code = nvm_write_buffer(
					BOOT_STATUS_ADDR,
					(void *) &status, NVMCTRL_PAGE_SIZE);
		} while (error_code == STATUS_BUSY);
		NVIC_SystemReset();
	} 
	else if (status.downlaoded_image != -1)
	{			
			// Clear out application code that is currently in nvm
			char arr [64] = {0};
			for (int addr = BASE_CODE_ADDR; addr < TOP_CODE_ADDR; addr += 64) {
				nvm_write_buffer(addr, (void *) arr, sizeof (char) * 64);
			} //end of for loop
			
			// Need to copy program from external flash to nvm
			at25dfx_chip_wake(&at25dfx_chip);
			if (at25dfx_chip_check_presence(&at25dfx_chip) != STATUS_OK) {
				// Handle missing or non-responsive device
			} //end if
			at25dfx_chip_set_global_sector_protect(&at25dfx_chip, false);
			at25dfx_chip_set_sector_protect(&at25dfx_chip, 0x10000, false);
			
			int flash_addr = BASE_FLASH_CODE_ADDR;
			for (int addr = BASE_CODE_ADDR; addr < TOP_CODE_ADDR; addr += 64) {
				at25dfx_chip_read_buffer(&at25dfx_chip, flash_addr, (void *) arr, sizeof (char) * 64);
				nvm_write_buffer(addr, (void *) arr, sizeof (char) * 64);  
				flash_addr += 64;
			} //end for
			
			// read firmware header
			firmware_header flash_header;
			at25dfx_chip_read_buffer(&at25dfx_chip, flash_addr, (void *) &flash_header, sizeof (firmware_header));
			
			// Done with reading from flash
			at25dfx_chip_set_global_sector_protect(&at25dfx_chip, true);
			at25dfx_chip_set_sector_protect(&at25dfx_chip, 0x10000, true);
			at25dfx_chip_sleep(&at25dfx_chip);
			
			// compare with calculated CRC
			uint32_t crc;
			dsu_crc32_init();
			if (STATUS_OK == dsu_crc32_cal(BASE_CODE_ADDR, BASE_CODE_ADDR-TOP_CODE_ADDR, &crc)) 
			{
					// GOOD
				
					//image is correctly loaded into nvm. Update the boot status
					status.integrity_check = INTEGRITY_CHECK;
					status.executing_image  = fw_header.firmware_version;	
					do
					{
						error_code = nvm_erase_row(
						BOOT_STATUS_ADDR);
					} while (error_code == STATUS_BUSY);
					
					do
					{
						error_code = nvm_write_buffer(
						BOOT_STATUS_ADDR,
						(void *) &status, NVMCTRL_PAGE_SIZE);
					} while (error_code == STATUS_BUSY);
				
				//NVIC_SystemReset();
			} //end of if
			else 
			{
				// BAD -- cpoying golden image
				// something about copying an older working version of the code
				//flush the nvm and read the second image from flash
				// Clear out application code that is currently in nvm
				char arr [64] = {0};
				for (int addr = BASE_CODE_ADDR; addr < TOP_CODE_ADDR; addr += 64) {
					nvm_write_buffer(addr, (void *) arr, sizeof (char) * 64);
				}
				// Need to copy program from external flash to nvm
				at25dfx_chip_wake(&at25dfx_chip);
				if (at25dfx_chip_check_presence(&at25dfx_chip) != STATUS_OK) {
					// Handle missing or non-responsive device
				}
				at25dfx_chip_set_global_sector_protect(&at25dfx_chip, false);
				at25dfx_chip_set_sector_protect(&at25dfx_chip, 0x10000, false);
				
				int flash_addr = TOP_FLASH_CODE_ADDR;
				for (int addr = BASE_CODE_ADDR; addr < TOP_CODE_ADDR; addr += 64) {
					at25dfx_chip_read_buffer(&at25dfx_chip, flash_addr, (void *) arr, sizeof (char) * 64);
					nvm_write_buffer(addr, (void *) arr, sizeof (char) * 64);
					flash_addr += 64;
				}
				// read firmware header
				firmware_header flash_header;
				at25dfx_chip_read_buffer(&at25dfx_chip, flash_addr, (void *) &flash_header, sizeof (firmware_header));
				
				// Done with reading from flash
				at25dfx_chip_set_global_sector_protect(&at25dfx_chip, true);
				at25dfx_chip_set_sector_protect(&at25dfx_chip, 0x10000, true);
				at25dfx_chip_sleep(&at25dfx_chip);
				
				// compare with calculated CRC
				uint32_t crc;
				dsu_crc32_init();
				if (STATUS_OK == dsu_crc32_cal(BASE_CODE_ADDR, BASE_CODE_ADDR-TOP_CODE_ADDR, &crc)) {
						// GOOD
					
						//image is correctly loaded into nvm. Update the boot status
						status.integrity_check = INTEGRITY_CHECK;
						status.executing_image  = fw_header.firmware_version;
						do
						{
							error_code = nvm_erase_row(
							BOOT_STATUS_ADDR);
						} while (error_code == STATUS_BUSY);
						do
						{
							error_code = nvm_write_buffer(
							BOOT_STATUS_ADDR,
							(void *) &status, NVMCTRL_PAGE_SIZE);
						} while (error_code == STATUS_BUSY);
					
						//NVIC_SystemReset();
					
					} 
					else {
						// BAD
						//both the firmware versions in external flash are corrupted.
						//update boot status to default.
							status = default_boot_status;
							do
							{
								error_code = nvm_erase_row(
								BOOT_STATUS_ADDR);
							} while (error_code == STATUS_BUSY);
							do
							{
								error_code = nvm_write_buffer(
								BOOT_STATUS_ADDR,
								(void *) &status, NVMCTRL_PAGE_SIZE);
							} while (error_code == STATUS_BUSY);
							NVIC_SystemReset();
					}//end golden image copying
				}
			} //end of status = downloaded_image
		 else if (status.executing_image != -1)
		  {
			// we are running an application
			/* Pointer to the Application Section */
			void (*application_code_entry)(void);
			
			/* Rebase the Stack Pointer */
			//__set_MSP(*(uint32_t *) APP_START_ADDR);
			
			/* Rebase the vector table base address */
			SCB->VTOR = ((uint32_t) APP_START_ADDR & SCB_VTOR_TBLOFF_Msk);
			
			/* Load the Reset Handler address of the application */
			application_code_entry = (void (*)(void))(unsigned *)(*(unsigned *) (APP_START_ADDR+ 4));
			
			/* Jump to user Reset Handler in the application */
			application_code_entry();

		}//end of application code
		else
		{
			/* do nothing*/
		}
	
}