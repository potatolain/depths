#include "source/globals.h"
#include "source/configuration/game_states.h"
#include "source/neslib_asm/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/menus/input_helpers.h"

CODE_BANK(PRG_BANK_MENU_INPUT_HELPERS);
void wait_for_start(void) {
    while (1) {
        lastControllerState = controllerState;
        controllerState = pad_poll(0);

        // If Start is pressed now, and was not pressed before...
        if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
            break;
        }

        ppu_wait_nmi();

    }
}

void wait_for_start_and_boot(void) {
    resetTimer = 0;

    // HACKISH - custom loop to kick the user out after a few minutes.
    while (1) {
        lastControllerState = controllerState;
        controllerState = pad_poll(0);

        // If Start is pressed now, and was not pressed before...
        if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
            break;
        }

        #if IS_KIOSK == 1
            ++resetTimer;

            // How long to wait until dumping, frames * seconds * minutes
            if (resetTimer > (60 * 60 * TIMEOUT_MINUTES)) {
                reset();
            }
        #endif

        ppu_wait_nmi();

    }
}