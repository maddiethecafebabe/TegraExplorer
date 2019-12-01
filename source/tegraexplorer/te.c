#include <stdio.h>
#include <string.h>
#include "te.h"
#include "gfx.h"
#include "../utils/util.h"
#include "tools.h"
#include "fs.h"

extern bool sd_mount();
extern void sd_unmount();
extern int launch_payload(char *path);
bool sd_mounted;

menu_item mainmenu[MAINMENU_AMOUNT] = {
    {"[SD:/] SD CARD", COLOR_GREEN, SD_CARD, 1},
    {"[EMMC:/] ?", COLOR_GREEN, EMMC, 1},
    {"\nMount/Unmount SD", COLOR_WHITE, MOUNT_SD, 1},
    {"Tools", COLOR_VIOLET, TOOLS, 1},
    {"\nCredits", COLOR_WHITE, CREDITS, 1},
    {"Exit", COLOR_WHITE, EXIT, 1}
};

menu_item shutdownmenu[7] = {
    {"-- EXIT --\n", COLOR_ORANGE, -1, 0},
    {"Back", COLOR_WHITE, -1, 1},
    {"\nReboot to RCM", COLOR_VIOLET, REBOOT_RCM, 1},
    {"Reboot normally", COLOR_ORANGE, REBOOT_NORMAL, 1},
    {"Power off\n", COLOR_BLUE, POWER_OFF, 1},
    {"Reboot to Hekate", COLOR_GREEN, HEKATE, -1},
    {"Reboot to Atmosphere", COLOR_GREEN, AMS, -1}
};

menu_item toolsmenu[3] = {
    {"-- TOOLS --\n", COLOR_VIOLET, -1, 0},
    {"Back", COLOR_WHITE, -1, 1},
    {"\nDisplay Console Info", COLOR_GREEN, DISPLAY_INFO, 1}
};

void fillmainmenu(){
    int i;

    for (i = 0; i < MAINMENU_AMOUNT; i++){
        switch (i + 1) {
            case 1:
                if (sd_mounted)
                    mainmenu[i].property = 1;
                else
                    mainmenu[i].property = -1;
                break;
            case 3:
                if (sd_mounted){
                    mainmenu[i].property = 2;
                    strcpy(mainmenu[i].name, "\nUnmount SD");
                }
                else {
                    mainmenu[i].property = 1;
                    strcpy(mainmenu[i].name, "\nMount SD");
                }
                break;
        }
    }
}

void te_main(){
    int res;

    sd_mounted = sd_mount();

    while (1){
        fillmainmenu();
        res = makemenu(mainmenu, MAINMENU_AMOUNT);

        switch(res){
            case SD_CARD:
                filemenu("SD:/");
                break;
            case EMMC:
                break;
            case MOUNT_SD:
                if (sd_mounted){
                    sd_mounted = false;
                    sd_unmount();
                } 
                else
                    sd_mounted = sd_mount();

                break;

            case TOOLS:
                res = makemenu(toolsmenu, 3);

                if (res == DISPLAY_INFO)
                    displayinfo();
                
                break;
            
            case CREDITS:
                message(CREDITS_MESSAGE, COLOR_WHITE);
                break;

            case EXIT:
                if (sd_mounted){  
                    if (checkfile("/bootloader/update.bin"))
                        shutdownmenu[5].property = 1;
                    else
                        shutdownmenu[5].property = -1;

                    if (checkfile("/atmosphere/reboot_payload.bin"))
                        shutdownmenu[6].property = 1;
                    else
                        shutdownmenu[6].property = -1;
                }
                else {
                    shutdownmenu[5].property = -1;
                    shutdownmenu[6].property = -1;
                }

                res = makemenu(shutdownmenu, 7);

                switch(res){
                    case REBOOT_RCM:
                        reboot_rcm();
                    
                    case REBOOT_NORMAL:
                        reboot_normal();

                    case POWER_OFF:
                        power_off();

                    case HEKATE:
                        launch_payload("/bootloader/update.bin");
                    
                    case AMS:
                        launch_payload("/atmosphere/reboot_payload.bin");
                } //todo declock bpmp

                break;
        }
    }
}