/*
 * Userdefine.h
 *
 *  Author: Shveta Gupta and Aasif Versi
 */


#ifndef USERDEFINE_H_
#define USERDEFINE_H_

#define HIGH 1
#define LOW 0
#define EXECUTING 1
#define IDLE 0

/* global variable declarations*/
uint8_t FlagBootStatus; //flag for bootloader status. While running the bootloader, keep the flag set/executing
/* global variable declarations*/

typedef struct StatusPage {

    uint8_t CurrentImage;   //firmware version of executing firmware image
    uint8_t StoredImage;    //firmware version of the non-executing image in FLASH
    uint8_t FirmwareVersion1; //version numbers of both the firmware images
    uint8_t FirmwareVersion2;
    uint8_t ImageSize1;  //size alloted for the firmware images in FLASH
    uint8_t ImageSize2;
    } ExternalFlashStatus;

/* structures for headers of two firmware image files*/
typedef struct HeaderFW1 {

    uint8_t FWVersionNumber;
    uint8_t CRC;
    } HeaderFW1, HeaderFW2, BootStatus ;



#endif /* USERDEFINE_H_ */
