#include "calthread.h"
#include "guitar_control.h"

void CalThread::run()
{
    int result;
    int nb_ifaces, iface;
    int flag = 0;
    struct libusb_device_handle* handle;
    unsigned char databuf[HID_REPORT_SIZE];
    libusb_device *dev = NULL;
    struct libusb_config_descriptor *conf_desc;
    double average = 0;
    double video_elapsed_time_ms[VIDEO_SAMPLE_SIZE];
    double audio_elapsed_time_ms[AUDIO_SAMPLE_SIZE];
    QString buff = "0";

    // Initialize LibUSB
    result = libusb_init( NULL );
    if (result < 0) {	// Failed to initialize.
        printf( "Error initializing libusb: %s\n", libusb_error_name( result ) );
        fflush(stdout);
        exit(-1);
    }

    // Set debugging output to max level
    libusb_set_option( NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING );

    // Open the device
    handle = libusb_open_device_with_vid_pid( NULL, VENDOR_ID, PRODUCT_ID );
    if (!handle) {	//Failed to open.
        printf("Error finding USB device\n");
        fflush(stdout);
        libusb_exit(NULL);
        exit(-2);
    }

    if (handle == NULL) {
        emit DisplayText("ERROR FINDING USB DEVICE. Click Quit to Exit.");

    } else {
        dev = libusb_get_device(handle);


        CALL_CHECK_CLOSE(libusb_get_config_descriptor(dev, 0, &conf_desc), handle);
        nb_ifaces = conf_desc->bNumInterfaces;

        libusb_set_auto_detach_kernel_driver(handle, 1);
        for (iface = 0; iface < nb_ifaces; iface++)
        {
            result = libusb_claim_interface(handle, iface);
            if (result != LIBUSB_SUCCESS) {
                perror("Failed.\n");
            }
        }
        result = turn_off_sensor(handle);
        if (result < 0){
            printf("Error while turning off sensor.");
            fflush(stdout);
        }

        // AUDIO CALIBRATION STARTS HERE

        emit DisplayText("-=- WELCOME TO AUTO-CALIBRATION PoC -=-\n\n\nAudio calibration: \nHold your Guitar Controller up to your sound system's speaker, then press A.");
        fflush(stdout);
        do {
            libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
            if (databuf[0] == 0x02)
                flag = 1;
            usleep(8000);
        } while (!flag);
        flag = 0;

        emit DisplayText("PERFORMING AUDIO CALIBRATION...\n");
        fflush(stdout);

        result = mic_on(handle);
        if (result < 0) {
            printf("Error while turning on mic.");
            fflush(stdout);
        }
        sleep(1); // Giving time to turn on the mic.

        for (int i = 0; i < AUDIO_SAMPLE_SIZE; i++) {
            auto t_start = std::chrono::high_resolution_clock::now();

            PlaySoundEffect();
            printf("Chirp %d of %d\n", i+1, AUDIO_SAMPLE_SIZE);
            fflush(stdout);

            do {
                libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
                if (databuf[15] == 0x3F || databuf[15] == 0x5F) //The chirp generates 3F and 5F when picked up by the controller.
                    flag = 1;
                usleep(8000);
            } while (!flag);
            flag = 0;
            databuf[15] = 0x00; // Reset the byte

            // Needs something incase the chirps arent picked up. What's important here is that we have a large enough sample size.
            // So if the chirp isn't picked up, we can ignore it and try another.

            auto t_end = std::chrono::high_resolution_clock::now();

            audio_elapsed_time_ms[i] = std::chrono::duration<double, std::milli>(t_end-t_start).count();
            average += audio_elapsed_time_ms[i];
            sleep(1);
        }
        average = average / AUDIO_SAMPLE_SIZE;

        buff = QString::number(average);
        buff.prepend("Audio Latency is : ");
        buff.append(" ms. \n\nPress Button A to continue.");

        emit DisplayText(buff) ;
        fflush(stdout);

        do {
            libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
            if (databuf[0] == 0x02)
                flag = 1;
            usleep(8000);
        } while (!flag);
        flag = 0;
        databuf[0] = 0x00;

        result = turn_off_sensor(handle);
        if (result < 0)
            printf("Error while turning off sensor.");
        fflush(stdout);

        do {
            libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
            if (databuf[0] == 0x00)
                flag = 1;
            usleep(8000);
        } while (!flag);
        flag = 0;
        // End of Audio Calibration. Start of Video Calibration.

        emit DisplayText("Video calibration: \nHold your Guitar Controller up close to the monitor, \nwith the front of the controller facing the screen, then press A.\n") ;
        fflush(stdout);

        do {
            libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
            if (databuf[0] == 0x02)
                flag = 1;
            usleep(8000);
        } while (!flag);
        flag = 0;
        average = 0;

        emit DisplayText("PERFORMING VIDEO CALIBRATION...\n");
        fflush(stdout);

        result = light_on(handle);
        if (result < 0)
            printf("Error while turning on mic.");
        fflush(stdout);
        sleep(1);

        for (int i = 0; i < VIDEO_SAMPLE_SIZE; i++){
            auto t_start = std::chrono::high_resolution_clock::now();

            emit FlashWhite();

            printf("Flash %d of %d\n", i + 1, VIDEO_SAMPLE_SIZE);
            fflush(stdout);

            do {
                libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
                if (databuf[16] == 0x32 ) // OR 0x3F
                    flag = 1;
                usleep(8000);
            } while (!flag);
            auto t_end = std::chrono::high_resolution_clock::now();
            emit ReturnBlack();
            flag = 0;
            databuf[16] = 0x00;

            // Needs something incase the chirps arent picked up. What's important here is that we have a large enough sample size.
            // So if the chirp isn't picked up, we can ignore it and try another.

            video_elapsed_time_ms[i] = std::chrono::duration<double, std::milli>(t_end-t_start).count();
            average += video_elapsed_time_ms[i];
            do {
                libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
                if (databuf[16] == 0x00 ) // Black screen is 0x00
                    flag = 1;
                usleep(8000);
            } while (!flag);
            flag = 0;

        }
        average = average / VIDEO_SAMPLE_SIZE;


        buff = QString::number(average);
        buff.prepend("Video Latency is : ");
        buff.append(" ms. \n\nPress Button A to end calibration.");
        emit DisplayText(buff);

        do {
            libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
            if (databuf[0] == 0x02)
                flag = 1;
            usleep(8000);
        } while (!flag);

        result = turn_off_sensor(handle);
        if (result < 0){
            printf("Error while turning off sensor.");
            fflush(stdout);
        }

        // END OF CALIBRATION PROCESS. Releasing interface.

        for (int iface = 0; iface < nb_ifaces; iface++) {
            printf("Releasing interface %d...\n", iface);
            fflush(stdout);
            libusb_release_interface(handle, iface);
        }
        emit DisplayText("Click Quit to close program.");
    }
}

CalThread::CalThread()
{
    my_chirpSound = new QSoundEffect;
    my_chirpSound->setSource(QUrl::fromLocalFile(":/resources/sound/beep.wav"));
}

CalThread::~CalThread()
{
    delete my_chirpSound;
    my_chirpSound = nullptr; // C++ 11
}

void CalThread::PlaySoundEffect()
{
    my_chirpSound->play();
}
