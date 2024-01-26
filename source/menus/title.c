#include "source/neslib_asm/neslib.h"
#include "source/menus/title.h"
#include "source/globals.h"
#include "source/configuration/game_states.h"
#include "source/configuration/system_constants.h"
#include "source/menus/text_helpers.h"
#include "source/graphics/palettes.h"
#include "source/configuration/game_info.h"
#include "source/graphics/fade_animation.h"
#include "source/menus/input_helpers.h"

CODE_BANK(PRG_BANK_TITLE);

#include "graphics/title_1.h"
#include "graphics/prestart.h"

#include "graphics/newtitle.h"

unsigned char titlePhase;

#define PRINT_NUM(x, y, num) vram_adr(NTADR_A(x, y)); vram_put('0' + ((num % 1000) / 100) - 0x20); vram_put('0' + ((num % 100) / 10) - 0x20); vram_put('0' + ((num % 10)) - 0x20);

void reset_game_stats(void) {

	playCount = 0;
	winCount = 0;

	for (i = 0; i != 10; i++) {
		checkpointHits[i] = 0;
	}

	for (i = 0; i != 3; ++i) {
		difficultyStartCount[i] = 0;
		difficultyWinCount[i] = 0;
	}
	
}

void dump_stats(void) {
    ppu_off();
    clear_screen_with_border();
    // We reuse the title palette here, though we have the option of making our own if needed.
    pal_bg(titlePalette);
	pal_spr(titlePalette);
    scroll(0, 0);

	// set_chr_bank_0(CHR_BANK_MENU);
    //set_chr_bank_1(CHR_BANK_MENU);
	load_chr_bank_menu();

    put_str(NTADR_A(3, 2), " Game Stats ");

	put_str(NTADR_A(4, 5), "Game Start Count");
	put_str(NTADR_A(4, 6), "Game complete count");
	put_str(NTADR_A(4, 8), "Checkpoint hits");

	for (i = 0; i != 10; ++i) {
		put_str(NTADR_A(5, 9 + i), "#");
		vram_put(('0' + i) - 0x20);
		PRINT_NUM(26, 9+i, checkpointHits[i]);
	}
	
	put_str(NTADR_A(4, 22), "Play/win counts");
	put_str(NTADR_A(5, 23), "Peaceful");
	put_str(NTADR_A(5, 24), "Normal");
	put_str(NTADR_A(5, 25), "Hard");

	PRINT_NUM(26, 5, playCount);
	PRINT_NUM(26, 6, winCount);

	PRINT_NUM(20, 23, difficultyStartCount[0]);
	PRINT_NUM(20, 24, difficultyStartCount[1]);
	PRINT_NUM(20, 25, difficultyStartCount[2]);

	PRINT_NUM(26, 23, difficultyWinCount[0]);
	PRINT_NUM(26, 24, difficultyWinCount[1]);
	PRINT_NUM(26, 25, difficultyWinCount[2]);




    // We purposely leave sprites off, so they do not clutter the view. 
    // This means all menu drawing must be done with background tiles - if you want to use sprites (eg for a menu item),
    // you will have to hide all sprites, then put them back after. 
    ppu_on_bg();

	banked_call(PRG_BANK_MENU_INPUT_HELPERS, wait_for_start_and_boot);
	reset();

}

void draw_title_screen(void) {
	set_vram_update(NULL);
	titlePhase = 0;
    ppu_off();
	pal_bg(titlePalette);
	pal_spr(titlePalette);

	// set_chr_bank_0(CHR_BANK_MENU);
    // set_chr_bank_1(CHR_BANK_MENU);
	load_chr_bank_menu();
	oam_clear();

	vram_adr(NAMETABLE_A);
	vram_unrle(prestart);
	sfx_play(SFX_CLICKY, SFX_CHANNEL_1);
    
	ppu_on_all();
	gameState = GAME_STATE_TITLE_INPUT;
}

const unsigned char titleSprites[] = {
	10*8, 14*8, 0xc4, 0,
	14*8, 12*8, 0xc0, 0,
	10*8, 10*8, 0x6e, 2, 
	20*8, 10*8, 0x6e, 2,
};

void draw_title_screen_real(void) {
	fade_out();
    ppu_off();
	pal_bg(titleAniPalette);
	pal_spr(mainSpritePalette);

	// set_chr_bank_0(CHR_BANK_MENU);
    // set_chr_bank_1(CHR_BANK_SPRITES);
	load_chr_bank_menu();
	// clear_screen();
	oam_clear();
	vram_adr(NAMETABLE_A);
	vram_unrle(newtitle);

    
    // put_str(NTADR_A(12, 5), gameName);
	
	//put_str(NTADR_A(2, 25), gameAuthorContact);

	put_str(NTADR_A(2, 26), "https://depths.nes.science");
	
	vram_adr(NTADR_A(3, 28));
	vram_put(0xeb);
	
	put_str(NTADR_A(4, 28), currentYear);
	put_str(NTADR_A(9, 28), "Sarah Parker");

	// put_str(NTADR_A(10, 16), "Press Start!");
	put_str(NTADR_A(10, 20), "Peaceful Game");
	put_str(NTADR_A(10, 22), " Normal Game");
	// put_str(NTADR_A(10, 18), " Hard Mode");
#if DEBUG == 1
	put_str(NTADR_A(10, 24), "Stat dump");
	put_str(NTADR_A(10, 26), "Reset Stats");
	put_str(NTADR_A(0, 1), "DEBUG MODE ENABLED");
#endif

	ppu_on_all();
	music_play(SONG_TITLE);
	fade_in();
}

void draw_title1(void) {
	set_vram_update(NULL);
	fade_out();
    ppu_off();
	pal_bg(titlePalette);
	pal_spr(titlePalette);
	oam_clear();

	vram_adr(NAMETABLE_A);
	vram_unrle(title_1);

	resetTimer = 0;
	ppu_on_all();
	fade_in();

}

#define TITLE_AREA(x, y, tileId) screenBuffer[i++] = MSB(NTADR_A(x, y)); \
	screenBuffer[i++] = LSB(NTADR_A(x, y)); \
	screenBuffer[i++] = tileId;

void handle_title_input(void) {
	tempChar9 = pad_trigger(0);


	++tempChara;

	#if IS_KIOSK
	if (titlePhase == 0 && tempChar9 == (PAD_START | PAD_UP | PAD_A)) {
		controllerState = tempChar9;
		dump_stats();
	}
	#endif
	if (titlePhase == 0 && tempChara == 192) {
		++titlePhase;
		draw_title_screen_real();
	}


	if (titlePhase == 1) {
		if ((tempChar9 & PAD_UP) && gameDifficulty != GAME_DIFFICULTY_PEACEFUL) {
			--gameDifficulty;
			sfx_play(SFX_CLICKY, SFX_CHANNEL_1);
		} else if ((tempChar9 & PAD_DOWN) && gameDifficulty != GAME_DIFFICULTY_MAX) {
			++gameDifficulty;
			sfx_play(SFX_CLICKY, SFX_CHANNEL_1);
		}

		i = 0;
		TITLE_AREA(8, 20, gameDifficulty == GAME_DIFFICULTY_PEACEFUL ? 0xe2 : 0x00);
		TITLE_AREA(9, 22, gameDifficulty == GAME_DIFFICULTY_NORMAL ? 0xe2: 0x00);
// Stripped out because these mess up the text at the bottom, since they override it (because lazy)
#if DEBUG == 1
		TITLE_AREA(8, 24, gameDifficulty == GAME_DIFFICULTY_STAT_DUMP ? 0xe2 : 0x00);
		TITLE_AREA(8, 26, gameDifficulty == GAME_DIFFICULTY_RESET_STATS ? 0xe2: 0x00);
#endif
		screenBuffer[i++] = NT_UPD_EOF;
		set_vram_update(screenBuffer);
		for (i = 0; i != 16; i += 4) {
			if (titleSprites[i+2] != 0x6e) {
				tempChar8 = ((frameCount >> 6) & 0x01) * 2;
			} else {
				tempChar8 = 0;
			}

			oam_spr(titleSprites[i], titleSprites[i+1], titleSprites[i+2] + tempChar8, titleSprites[i+3], (i<<2));
			oam_spr(titleSprites[i]+8, titleSprites[i+1], titleSprites[i+2]+1 + tempChar8, titleSprites[i+3], (i<<2)+4);
			oam_spr(titleSprites[i], titleSprites[i+1]+8, titleSprites[i+2]+16 + tempChar8, titleSprites[i+3], (i<<2)+8);
			oam_spr(titleSprites[i]+8, titleSprites[i+1]+8, titleSprites[i+2]+17 + tempChar8, titleSprites[i+3], (i<<2)+12);
		}

		++resetTimer;

		// Kick back to the first load screen every 2 mins or so.
		// NOTE: Something's making this only run every other frame, so I'm doing every 30 "frames" instead of 60
		if (resetTimer > (60 * 60 * 1)) {
			reset();
		}
	}

	if (titlePhase == 1 && tempChar9 & (PAD_START|PAD_A)) {

#if DEBUG == 1
		if (gameDifficulty == GAME_DIFFICULTY_RESET_STATS) {
			reset_game_stats();
			reset();
		} else if (gameDifficulty == GAME_DIFFICULTY_STAT_DUMP) {
			dump_stats();
		}
#endif

		++titlePhase;
		sfx_play(SFX_CLICKY, SFX_CHANNEL_1);
		music_play(SONG_WIN); // ocean

		draw_title1();
		return;
	}
#if IS_KIOSK == 1
	if (titlePhase == 2) {
		++resetTimer;

		// Kick back to the first load screen every 2 mins or so.
		// NOTE: Something's making this only run every other frame, so I'm doing every 30 "frames" instead of 60
		if (resetTimer > (60 * 60 * 1)) {
			reset();
		}

	}
#endif


	if (titlePhase == 2 && tempChar9 & (PAD_START|PAD_A)) {
		gameState = GAME_STATE_POST_TITLE;
		sfx_play(SFX_CLICKY, SFX_CHANNEL_1);

		return;
	}

	if ((frameCount >> 6) & 0x01) {
		bank_bg(0);
	} else {
		bank_bg(1);
	}

}