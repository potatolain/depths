#include "source/menus/game_over.h"
#include "source/graphics/palettes.h"
#include "source/configuration/system_constants.h"
#include "source/globals.h"
#include "source/neslib_asm/neslib.h"
#include "source/configuration/game_states.h"
#include "source/menus/text_helpers.h"
#include "source/menus/input_helpers.h"

CODE_BANK(PRG_BANK_GAME_OVER);

#include "graphics/game_over.h"

#define FATE_COUNT 10
const unsigned char* unfortunateFates[] = {
//  "                         "
    "passing away in an       ",
    "unexpected accident      ",

    "passing away quietly in  ",
    "my sleep.",

    "passing away from a      ",
    "terminal disease.        ",

    "passing away in the shock",
    "of an earthquake.        ",

    "being swept away in a    ",
    "freak hurricane.         ",

    "hust... passing away,    ",
    "for no obvious reason.",

    "passing away from a heart",
    "attack.",

    "a plane's engines        ",
    "failing at 30,000 feet.  ",

    "a voice saying it would  ",
    "easier to pass away.     ",

    "a voice suggesting that  ",
    "no one wants me.        "

};

void draw_game_over_screen(void) {
    set_vram_update(NULL);
    ppu_off();

    // We reuse the title palette here, though we have the option of making our own if needed.
    pal_bg(titlePalette);
	pal_spr(titlePalette);
    scroll(0, 0);

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU);

    vram_adr(NAMETABLE_A);
    vram_unrle(game_over);

    tempChar1 = (rand8() % FATE_COUNT)<<1;
    vram_adr(0x2163);


    for (i = 0; i != 25; ++i) {
        tempChar3 = unfortunateFates[tempChar1][i];
        if (tempChar3 == NULL) {
            break;
        }
        vram_put(tempChar3 - 0x20);
    }

    vram_adr(0x2183);
    ++tempChar1;

    for (i = 0; i != 25; ++i) {
        tempChar3 = unfortunateFates[tempChar1][i];
        if (tempChar3 == NULL) {
            break;
        }
        vram_put(tempChar3 - 0x20);
    }



    // We purposely leave sprites off, so they do not clutter the view. 
    // This means all menu drawing must be done with background tiles - if you want to use sprites (eg for a menu item),
    // you will have to hide all sprites, then put them back after. 
    ppu_on_bg();

}