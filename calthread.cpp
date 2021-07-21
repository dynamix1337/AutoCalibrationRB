#include "calthread.h"

void CalThread::run()
{
    int result, nb_ifaces, i;
    double video_elapsed_time_ms[VIDEO_SAMPLE_SIZE], audio_elapsed_time_ms[AUDIO_SAMPLE_SIZE];
    unsigned char databuf[HID_REPORT_SIZE];
    struct libusb_device_handle *handle;
    struct libusb_config_descriptor *conf_desc;
    int flag = 0, progress_value = 0;
    double audio_lat = 0, video_lat = 0;
    libusb_device *dev = NULL;
    QString buff = "0";

    // Initialize LibUSB
    result = libusb_init(NULL);
    if (result < 0) {	// Failed to initialize.
        printf( "Error initializing libusb: %s\n", libusb_error_name(result) );
        fflush(stdout);
    }

    // Set debugging output to max level
    libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING);

    // Open the device
    handle = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
    if (!handle) {	//Failed to open.
        printf("Error finding USB device\n");
        fflush(stdout);
        libusb_exit(NULL);
    }

    if (handle == NULL) {   // Dongle not plugged
        emit DisplayText("ERROR FINDING USB DEVICE. Click Quit to Exit.");

    } else {
        dev = libusb_get_device(handle);


        CALL_CHECK_CLOSE(libusb_get_config_descriptor(dev, 0, &conf_desc), handle);
        nb_ifaces = conf_desc->bNumInterfaces;

        libusb_set_auto_detach_kernel_driver(handle, 1);
        for (i = 0; i < nb_ifaces; i++)
        {
            result = libusb_claim_interface(handle, i);
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

        emit DisplayText("Audio calibration: <br>Hold your Guitar Controller up to your sound system's speaker, then press <b><font color=green>A<\font></b> to begin.");

        do {
            libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
            if (databuf[0] == 0x02)
                flag = 1;
            usleep(8000);
        } while (!flag);
        flag = 0;

        emit DisplayText("PERFORMING AUDIO CALIBRATION...");

        result = mic_on(handle);
        if (result < 0) {
            printf("Error while turning on mic.");
            fflush(stdout);
        }
        sleep(1); // Giving time to turn on the mic.

        for (i = 0; i < AUDIO_SAMPLE_SIZE; i++) {
            auto t_start = std::chrono::high_resolution_clock::now();

            PlaySoundEffect();

            do {
                libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
                if (databuf[15] == 0x3F || databuf[15] == 0x5F) //The chirp generates 3F and 5F when picked up by the controller.
                    flag = 1;
                usleep(8000);
            } while (!flag);
            auto t_end = std::chrono::high_resolution_clock::now();
            progress_value += 10;
            emit progressBar_valueChanged(progress_value);
            flag = 0;
            databuf[15] = 0x00; // Reset the byte

            /* A method of checking data would be good to add. */

            audio_elapsed_time_ms[i] = std::chrono::duration<double, std::milli>(t_end-t_start).count();
            /* // For Debug
            printf("%f\n",audio_elapsed_time_ms[i]);
            fflush(stdout); */
            audio_lat += audio_elapsed_time_ms[i];
            do {
                libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
                if (databuf[15] == 0x7F ) // Initial state is 7F
                    flag = 1;
                usleep(8000);
            } while (!flag);
            flag = 0;
            sleep(1);
        }
        audio_lat = audio_lat / AUDIO_SAMPLE_SIZE;

        buff = "Audio Latency is : " + QString::number(audio_lat)+ " ms. <br><br>Press Button <b><font color=green>A<\font></b> to continue to video calibration.";

        emit DisplayText(buff) ;

        do {
            libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
            if (databuf[0] == 0x02)
                flag = 1;
            usleep(8000);
        } while (!flag);
        flag = 0;
        databuf[0] = 0x00;

        result = turn_off_sensor(handle);
        if (result < 0) {
            printf("Error while turning off sensor.");
            fflush(stdout);
        }

        do {
            libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
            if (databuf[0] == 0x00)
                flag = 1;
            usleep(8000);
        } while (!flag);
        flag = 0;

        // End of Audio Calibration. Start of Video Calibration.
        progress_value = 0;
        emit progressBar_valueChanged(progress_value);
        emit DisplayText("Video calibration: <br>Hold your Guitar Controller up close to the monitor, <br>with the front of the controller facing the screen, then press <b><font color=green>A<\font></b> to begin.") ;

        do {
            libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
            if (databuf[0] == 0x02)
                flag = 1;
            usleep(8000);
        } while (!flag);
        flag = 0;

        emit DisplayText("PERFORMING VIDEO CALIBRATION...");

        result = light_on(handle);
        if (result < 0) {
            printf("Error while turning on mic.");
            fflush(stdout);
        }
        sleep(1);

        for (i = 0; i < VIDEO_SAMPLE_SIZE; i++){
            auto t_start = std::chrono::high_resolution_clock::now();

            emit FlashWhite();

            do {
                libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
                if (databuf[16] == 0x32 || databuf[16] == 0x7F) //My white screen registers a 0x32
                    flag = 1;
                usleep(8000);
            } while (!flag);
            auto t_end = std::chrono::high_resolution_clock::now();
            progress_value += 10;
            emit progressBar_valueChanged(progress_value);
            emit ReturnBlack();
            flag = 0;
            databuf[16] = 0x00;

            /* A method of checking data would be good to add. */

            video_elapsed_time_ms[i] = std::chrono::duration<double, std::milli>(t_end-t_start).count();
            /* //For Debug
            printf("%f\n",video_elapsed_time_ms[i]);
            fflush(stdout); */
            video_lat += video_elapsed_time_ms[i];
            do {
                libusb_interrupt_transfer(handle, 0x81, databuf, HID_REPORT_SIZE, NULL, URB_TIMEOUT);
                if (databuf[16] == 0x00 || databuf[16] == 0x0A) // Black screen is 0x00 or 0A (depending on screens brightness)
                    flag = 1;
                usleep(8000);
            } while (!flag);
            flag = 0;
            usleep(500000);

        }
        video_lat = video_lat / VIDEO_SAMPLE_SIZE;
        buff = "Video Latency is: " + QString::number(video_lat) + " ms. <br><br>Press Button <b><font color=green>A<\font></b> to end calibration.";
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

        for (i = 0; i < nb_ifaces; i++) {
            libusb_release_interface(handle, i);
        }
        buff = "Audio latency: " + QString::number(audio_lat) + " ms<br>Video latency: " + QString::number(video_lat) + " ms<br><br>Click Quit to close program.";
        emit DisplayText(buff);
    }
}

CalThread::CalThread()
{
    my_chirpSound = new QSoundEffect;
    my_chirpSound->setSource(QUrl("file:sound/beep.wav"));
}

CalThread::~CalThread()
{
    delete my_chirpSound;
    my_chirpSound = nullptr;
}

void CalThread::PlaySoundEffect()
{
    my_chirpSound->play();
}
