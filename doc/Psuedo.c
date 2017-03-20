

int main() {
    if (pin PA15 == 0) {
        // Bootloader
        upload_code_to_flash();
        decrypt_firmware();
        upload_code_device();
        if(check_crc()){
            // valid
            upload_code_w25();
        } else {
            // invalid
            discard_code_flash();
        }
    } else {
        // App code
        read_flash_code();
        if(check_crc()) {
            run_app();
        }else {
            copy_from_flash();
            if(check_crc){
                run_app();
            }
        }
    }

    while(1);;
}
