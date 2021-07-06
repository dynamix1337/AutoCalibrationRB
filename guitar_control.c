#include "guitar_control.h"

static unsigned char activate_mic[] =    {0xE9, 0x00, 0x83, 0x1B, 0x00, 0x00, 0x00, 0x02};
static unsigned char activate_light[] =  {0xE9, 0x00, 0x83, 0x1B, 0x00, 0x00, 0x00, 0x01};
static unsigned char reset[] =           {0xE9, 0x00, 0x83, 0x1B, 0x00, 0x00, 0x00, 0x00};

static unsigned char the_rest[4][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00},
    {0x00, 0x00, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0xE9, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

int mic_on(libusb_device_handle * handle)
{
    int result, i;
    unsigned char get_buff[REPORT_SIZE];

    result = libusb_control_transfer(handle, 0x21, 0x09, 0x0300, 0x0000, activate_mic, sizeof(activate_mic), 0);
    if (result < 0) {	// Failed to initialize.
        printf( "Error sending control transfer: %s\n", libusb_error_name( result ) );
        exit(-1);
    }

    for (i = 0; i < 4; i++) {
        result = libusb_control_transfer(handle, 0x21, 0x09, 0x0300, 0x0000, the_rest[i], sizeof(the_rest[i]), 0);
        if (result < 0) {	// Failed to initialize.
            printf( "Error sending control transfer: %s\n", libusb_error_name( result ) );
            exit(-1);
        }
        usleep(1473); // To give the device a chance to do what it does
    }

    result = libusb_control_transfer(handle, 0xA1, 0x01, 0x0300, 0x0000, get_buff, REPORT_SIZE, 0);
    if (result < 0) {	// Failed to initialize.
        printf( "Error sending control transfer: %s\n", libusb_error_name( result ) );
        exit(-1);
    }
    usleep(1473);
    return 0;
}

int light_on(libusb_device_handle * handle)
{
    int result, i;
    unsigned char get_buff[REPORT_SIZE];

    result = libusb_control_transfer(handle, 0x21, 0x09, 0x0300, 0x0000, activate_light, sizeof(activate_light), 0);
    if (result < 0) {	// Failed to initialize.
        printf( "Error sending control transfer: %s\n", libusb_error_name( result ) );
        exit(-1);
    }

    for (i = 0; i < 4; i++) {
        result = libusb_control_transfer(handle, 0x21, 0x09, 0x0300, 0x0000, the_rest[i], sizeof(the_rest[i]), 0);
        if (result < 0) {	// Failed to initialize.
            printf( "Error sending control transfer: %s\n", libusb_error_name( result ) );
            exit(-1);
        }
        usleep(1473); // To give the device a chance to do what it does
    }

    result = libusb_control_transfer(handle, 0xA1, 0x01, 0x0300, 0x0000, get_buff, REPORT_SIZE, 0);
    if (result < 0) {	// Failed to initialize.
        printf( "Error sending control transfer: %s\n", libusb_error_name( result ) );
        exit(-1);
    }
    usleep(1473);
    return 0;
}

int turn_off_sensor(libusb_device_handle * handle)
{
    int result, i;
    unsigned char get_buff[REPORT_SIZE];

    result = libusb_control_transfer(handle, 0x21, 0x09, 0x0300, 0x0000, reset, sizeof(reset),0);
    if (result < 0) {	// Failed to initialize.
        printf( "Error sending control transfer: %s\n", libusb_error_name( result ) );
        exit(-1);
    }

    for (i = 0; i < 4; i++) {

        result = libusb_control_transfer(handle, 0x21, 0x09, 0x0300, 0x0000, the_rest[i], sizeof(the_rest[i]), 0);
        if (result < 0) {	// Failed to initialize.
            printf( "Error sending control transfer: %s\n", libusb_error_name( result ) );
            exit(-1);
        }
        usleep(1473); // To give the device a chance to do what it does
    }

    result = libusb_control_transfer(handle, 0xA1, 0x01, 0x0300, 0x0000, get_buff, REPORT_SIZE, 0);
    if (result < 0) {	// Failed to initialize.
        printf( "Error sending control transfer: %s\n", libusb_error_name( result ) );
        exit(-1);
    }
    usleep(1473);
    return 0;
}
