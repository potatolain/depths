#include "source/menus/credits.h"
#include "source/graphics/palettes.h"
#include "source/configuration/system_constants.h"
#include "source/globals.h"
#include "source/neslib_asm/neslib.h"
#include "source/configuration/game_states.h"
#include "source/menus/text_helpers.h"
#include "source/configuration/game_info.h"

CODE_BANK(PRG_BANK_CREDITS_MENU);

#include "graphics/end1.h"
#include "graphics/end2.h"
#include "graphics/end3.h"

#include "graphics/postv2_1.h"
#include "graphics/postv2_2.h"
#include "graphics/postv2_3.h"
#include "graphics/postv2_4.h"
#include "graphics/postv2_5.h"

const unsigned char* endPiece[] = {
    end1,
    end2,
    //end3,
    postv2_1,
    postv2_2,
    postv2_3,
    postv2_4,
    postv2_5
};


void draw_win_screen(unsigned char id) {
    ppu_off();
    set_vram_update(NULL);

    // We reuse the title palette here, though we have the option of making our own if needed.
    if (id == 6) {
        pal_bg(titleAniPalette);
        pal_spr(mainSpritePalette);
    } else {
        pal_bg(titlePalette);
	    pal_spr(titlePalette);
    }
    scroll(0, 0);

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_SPRITES);

    vram_adr(NAMETABLE_A);
    vram_unrle(endPiece[id]);

    // Hide all existing sprites
    oam_clear();
    ppu_on_all();

}

void draw_credits_screen(void) {
    ppu_off();
    clear_screen_with_border();
    // We reuse the title palette here, though we have the option of making our own if needed.
    pal_bg(titlePalette);
	pal_spr(titlePalette);
    scroll(0, 0);

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_SPRITES);

    // Add whatever you want here; NTADR_A just picks a position on the screen for you. Your options are 0, 0 to 32, 30
    put_str(NTADR_A(12, 2), " Credits ");

    put_str(NTADR_A(3, 6), "Game Design, Music + Art");
    put_str(NTADR_A(5, 8), gameAuthor);

    put_str(NTADR_A(3, 11), "Inspired by a post by");
    put_str(NTADR_A(5, 13), "Anna Borges");

    // put_str(NTADR_A(2, 16), "Artwork");
    // put_str(NTADR_A(4, 18), "Refresh Games (OpenGameArt)");

    put_str(NTADR_A(9, 21), "Copyright");
    put_str(NTADR_A(19, 21), currentYear);

    put_str(NTADR_A(7, 23), "Christopher Parker");
    put_str(NTADR_A(8, 25), "aka @cppchriscpp");


    // Hide all existing sprites
    oam_clear();
    ppu_on_all();
}