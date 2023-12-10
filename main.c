/*
 * a6meowing - main.c
 *
 * Copyright (c) 2021 - 2023 kok3shidoll
 *
 */

#include <io/iousb.h>
#include <common/common.h>
#include <common/log.h>

#include <getopt.h>

io_client_t client;
bool debug_enabled = false;

int a6meowing(io_client_t client);

static void meow_list(void)
{
    printf("Device list:\n");
    printf("\t\x1b[36ms5l8950x\x1b[39m - \x1b[35mApple A6\x1b[39m\n");
}

static void meow_usage(char** argv)
{
    printf("Usage: %s [option]\n", argv[0]);
    
    printf("  -h, --help\t\t\t\x1b[36mmeow usage\x1b[39m\n");
    printf("  -l, --list\t\t\t\x1b[36mmeow list of supported devices\x1b[39m\n");
    printf("  -c, --cleandfu\t\t\x1b[36mmeow cleandfu\x1b[39m\n");
    printf("  -d, --debug\t\t\t\x1b[36menable meow log\x1b[39m\n");
    printf("\n");
}

int main(int argc, char** argv)
{
    
    bool useRecovery = false;

    MEOW_NOFUNC("================================");
    MEOW_NOFUNC("::");
    MEOW_NOFUNC(":: a6meowing v1.0.0 for macosx");
    MEOW_NOFUNC("::");
    MEOW_NOFUNC(":: (c) 2020-2023 kok3shidoll");
    MEOW_NOFUNC("::");
    MEOW_NOFUNC(":: BUILD_STYLE: %s", "RELEASE");
    MEOW_NOFUNC("::");
    MEOW_NOFUNC(":: ---- made by ----");
    MEOW_NOFUNC(":: kok3shidoll (meow)");
    MEOW_NOFUNC(":: ---- thanks to ----");
    MEOW_NOFUNC(":: checkra1n");
    MEOW_NOFUNC("================================");
    MEOW_NOFUNC("");
    
    int opt = 0;
    static struct option longopts[] = {
        { "help",           no_argument,       NULL, 'h' },
        { "list",           no_argument,       NULL, 'l' },
        { "cleandfu",       no_argument,       NULL, 'c' },
        { "debug",          no_argument,       NULL, 'd' },
        { NULL, 0, NULL, 0 }
    };
    
    const char *opsStr = "hlvd";
    
    while ((opt = getopt_long(argc, argv, opsStr, longopts, NULL)) > 0) {
        switch (opt) {
            case 'h':
                meow_usage(argv);
                return 0;
                
            case 'l':
                meow_list();
                return 0;
                
            case 'd':
                debug_enabled = true;
                DEVMEOW("Enabled meowing log");
                break;
                
            case 'c':
                useRecovery = true;
                break;
                
            default:
                meow_usage(argv);
                return -1;
        }
    }
    
    if(useRecovery) {
        if(enter_dfu_via_recovery(client)) {
            return -1;
        }
    }
    
    MEOWWWW("Waiting for DFU meow device");
    while(get_device(DEVICE_DFU, true)) {
        sleep(1);
    }
    
    MEOWWWW("Found DFU meow device");
    sleep(1);
    
    if(client->hasSerialStr == false) {
        read_serial_number(client); // For iOS 10 and lower
    }
    
    DEVMEOW("CPID: 0x%02x, CPRV:0x%02x ", client->devinfo.cpid, client->devinfo.cprv);
    
    if(client->hasSerialStr != true) {
        ERROR("Serial number was not meow");
        return -1;
    }
    
    a6meowing(client);
    
    return 0;
}

