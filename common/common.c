/*
 * a6meowing - common.c
 *
 * Copyright (c) 2021 - 2023 kok3shidoll
 *
 */

#include <io/iousb.h>
#include <common/common.h>
#include <common/log.h>

#include <time.h>

struct checkm8_device_list {
    uint16_t        cpid;
    uint8_t         bdid;
    int     enterdfuType;
};

static struct checkm8_device_list devlists[] = {
    // Apple A6
    { 0x8950, 0xff, DFU_LEGACY }, // legacy
    // err
    { 0xffff, 0xff, DFU_UNKOWN_TYPE }
};

static long cpuTime;

static void interval(double sec)
{
    long now;
    double n_sec;
    double b_sec = (double)cpuTime / CLOCKS_PER_SEC;
    while(1) {
        now = clock();
        n_sec = (double)now / CLOCKS_PER_SEC;
        if ((n_sec - b_sec) > sec)
            break;
    }
    cpuTime = now;
}

static void prog(int sec)
{
    int i = 0;
    int j = 0;
    
    cpuTime = clock();
    
    for(i = 0; i < sec; i++) {
        printf("[");
        for (j = 0; j < i + 1; j++)
            printf("=");
        for (;j < sec; j++)
            printf(" ");
        printf("] (%d/%d sec)\r", i + 1, sec);
        fflush(stdout);
        interval(1);
    }
    printf("\n");
}

transfer_t send_data(io_client_t client, unsigned char* buf, size_t size)
{
    return usb_ctrl_transfer(client, 0x21, 1, 0x0000, 0x0000, buf, size);
}

transfer_t send_data_with_time(io_client_t client, unsigned char* buf, size_t size, int timeout)
{
    return usb_ctrl_transfer_with_time(client, 0x21, 1, 0x0000, 0x0000, buf, size, timeout);
}

transfer_t get_status(io_client_t client, unsigned char* buf, size_t size)
{
    return usb_ctrl_transfer(client, 0xa1, 3, 0x0000, 0x0000, buf, size);
}

transfer_t send_abort(io_client_t client)
{
    return usb_ctrl_transfer_with_time(client, 0x21, 4, 0x0000, 0x0000, NULL, 0, 0);
}


int enter_dfu_via_recovery(io_client_t client)
{
    MEOWWWW("Waiting for recovery meow device");
    while(1) {
        if(client) {
            io_close(client);
            client = NULL;
        }
        io_open(&client, DEVICE_RECOVERY_MODE_2, true);
        if(client) break;
    }
    if(!client) {
        ERROR("Failed to find the device in Recovery meow");
        return -1;
    }
    
    MEOWWWW("Found recovery meow device");
    
    if(client->hasSerialStr == false) {
        read_serial_number(client);
    }
    
    DEVMEOW("CPID: 0x%02x, CPRV: 0x%01x", client->devinfo.cpid, client->devinfo.cprv);
    
    int enterdfuType = DFU_UNKOWN_TYPE;
    for (int i = 0; devlists[i].cpid != 0xffff; i++) {
        if (client->devinfo.cpid == devlists[i].cpid) {
            // check t8010: multiple types are mixed :/
            if (client->devinfo.cpid == 0x8010) {
                for(int j=i; devlists[j].bdid != 0xff; j++) {
                    if (client->devinfo.bdid == devlists[j].bdid) {
                        enterdfuType = devlists[i].enterdfuType;
                        break;
                    }
                }
            } else {
                enterdfuType = devlists[i].enterdfuType;
                break;
            }
        }
    }
    DEVMEOW("MEOWType: %08x", enterdfuType);
    
    const char* btn = NULL;
    int step2_sec = 4;
    int step3_sec = 10;
    
    if (enterdfuType == DFU_LEGACY)
    { /* DFU_LEGACY */
        btn = "Home meow";
    }
    else if (enterdfuType == DFU_IPHONE7 ||
             enterdfuType == DFU_IPHONEX)
    { /* DFU_IPHONE7, DFU_IPHONEX */
        btn = "Volume down meow";
    }
    else
    { /* DFU_UNKOWN_TYPE */
        ERROR("This meow is not yet supported for this device.");
        return -1;
    } /* DFU_UNKOWN_TYPE */
    
    printf("\n");
    
    printf("================================\n");
    printf("::\n");
    printf(":: %s\n", __FUNCTION__);
    printf(":: CPID: 0x%02x CPRV: 0x%02x BDID: 0x%02x TYPE: 0x%02x\n", client->devinfo.cpid, client->devinfo.cprv, client->devinfo.bdid, enterdfuType);
    printf("::\n");
    printf(":: \x1b[34mTime to put the meow into DFU mode\x1b[39m\n");
    printf(":: \x1b[34mPlease follow the instructions meow to operate the device.\x1b[39m\n");
    printf("::\n");
    printf(":: \x1b[32mSTEP1 Press \x1b[31m<meow>\x1b[32m key.\x1b[39m\n");
    printf(":: \x1b[32mSTEP2 Press and hold Side(or Top) and %ss together \x1b[31m(%dsec)\x1b[39m\n", btn, step2_sec);
    printf(":: \x1b[32mSTEP3 Release Side(or Top) button But keep holding %s \x1b[31m(%dsec)\x1b[39m\n", btn, step3_sec);
    printf("================================\n");
    printf("\n");
    printf("\x1b[34mready? it starts 3 seconds after press <meow> key.\x1b[39m\n");
    printf("\x1b[32mSTEP1 > Press <meow> key\x1b[39m >> ");
    getchar();
    printf("\n");
    
    // initialization for interval()
    cpuTime = clock();
    
    for(int i = 0; i <3 ; i++) {
        printf("preparing... STEP2 will meow after %d seconds\r", 3 - i);
        fflush(stdout);
        interval(1);
    }
    
    MEOW_NOFUNC("STEP2 > Press and hold Side(or Top) and %ss together (%dsec)", btn, step2_sec);
    int j=0;
    for(int i = 0; i < step2_sec; i++) {
        if(i == 1)
            send_reboot_via_recovery(client);
        printf("[");
        for (j = 0; j < i + 1; j++)
            printf("=");
        for (;j < step2_sec; j++)
            printf(" ");
        printf("] (%d/%d sec)\r", i + 1, step2_sec);
        fflush(stdout);
        interval(1);
    }
    printf("\n");
    
    MEOW_NOFUNC("STEP3 > Release Side(or Top) button But keep holding %s (%dsec)", btn, step3_sec);
    prog(step3_sec);
    
    MEOWWWW("Reconnecting device");
    io_reconnect(&client, 5, DEVICE_DFU, USB_NO_RESET, false, 10000);
    if(!client) {
        ERROR("Failed to put the device into DFU meow");
        return -1;
    }
    
    io_close(client);
    client = NULL;
    return 0;
}

const char *IOReturnName(IOReturn res)
{
    if(res == kIOReturnSuccess)
        return "Meow";
    if(res == kIOReturnTimeout || res == kIOUSBTransactionTimeout)
        return "TimeMeow";
    if(res == kIOUSBPipeStalled || res == kUSBHostReturnPipeStalled)
        return "PipeMeowed";
    if(res == kIOReturnNotResponding)
        return "NotMeowing";
    if(res == kIOReturnNoDevice)
        return "NoMeow";
    if(res == kIOReturnAborted)
        return "AbortedMeow";
    if(res == kIOReturnError)
        return "ErrorMeow";
        
    return "UnMeow";
}
