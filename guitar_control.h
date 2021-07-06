#ifdef __cplusplus
extern "C"
{
#endif

#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libusb-1.0/libusb.h>

#define VENDOR_ID	0x1bad	// Harmonix
#define PRODUCT_ID	0x3430	// Fender Mustang Pro-Guitar Wii
#define REPORT_SIZE 0x001C	// Size of Reports
#define ERR_EXIT(errcode) do { printf("   %s\n", libusb_strerror((enum libusb_error)errcode)); /*return -1;*/ } while (0)
#define CALL_CHECK_CLOSE(fcall, hdl) do { int _r=fcall; if (_r < 0) { libusb_close(hdl); ERR_EXIT(_r); } } while (0)

int mic_on(libusb_device_handle * handle);

int light_on(libusb_device_handle * handle);

int turn_off_sensor(libusb_device_handle * handle);

#ifdef __cplusplus
}
#endif
