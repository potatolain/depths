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

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU);

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

	banked_call(PRG_BANK_MENU_INPUT_HELPERS, wait_for_start);
	reset();

}

void draw_title_screen(void) {
	set_vram_update(NULL);
	titlePhase = 0;
    ppu_off();
	pal_bg(titlePalette);
	pal_spr(titlePalette);

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU);
	oam_clear();

	vram_adr(NAMETABLE_A);
	vram_unrle(prestart);
	sfx_play(SFX_CLICKY, SFX_CHANNEL_1);
    
	ppu_on_all();
	gameState = GAME_STATE_TITLE_INPUT;
}

void draw_title_screen_real(void) {
	fade_out();
    ppu_off();
	pal_bg(titlePalette);
	pal_spr(titlePalette);

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU);
	clear_screen();
	oam_clear();

    
    put_str(NTADR_A(12, 5), gameName);
	
	put_str(NTADR_A(2, 26), gameAuthorContact);
	
	put_str(NTADR_A(2, 28), "Copyright");
	put_str(NTADR_A(12, 28), currentYear);
	put_str(NTADR_A(17, 28), gameAuthor);

	// put_str(NTADR_A(10, 16), "Press Start!");
	put_str(NTADR_A(10, 14), "Peaceful Game");
	put_str(NTADR_A(10, 16), " Normal Game");
	// put_str(NTADR_A(10, 18), " Hard Mode");
#if DEBUG == 1
	put_str(NTADR_A(10, 20), "Stat dump");
	put_str(NTADR_A(10, 22), "Reset Stats");
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


	vram_adr(NAMETABLE_A);
	vram_unrle(title_1);

	ppu_on_all();
	fade_in();

}

void handle_title_input(void) {

	++tempChara;
	if (titlePhase == 0 && tempChara == 192) {
		++titlePhase;
		draw_title_screen_real();
	}
	tempChar9 = pad_trigger(0);


	if (titlePhase == 1) {
		if ((tempChar9 & PAD_UP) && gameDifficulty != GAME_DIFFICULTY_PEACEFUL) {
			--gameDifficulty;
			sfx_play(SFX_CLICKY, SFX_CHANNEL_1);
		} else if ((tempChar9 & PAD_DOWN) && gameDifficulty != GAME_DIFFICULTY_MAX) {
			++gameDifficulty;
			sfx_play(SFX_CLICKY, SFX_CHANNEL_1);
		}

		screenBuffer[0] = MSB(NTADR_A(8, 14)) | NT_UPD_VERT;
		screenBuffer[1] = LSB(NTADR_A(8, 14));
#if DEBUG == 1
		screenBuffer[2] = 9;
#else
		screenBuffer[2] = 5;
#endif
		screenBuffer[3] = gameDifficulty == GAME_DIFFICULTY_PEACEFUL ? 0xe2 : 0x00;
		screenBuffer[4] = 0x00;
		screenBuffer[5] = gameDifficulty == GAME_DIFFICULTY_NORMAL ? 0xe2 : 0x00;
		screenBuffer[6] = 0x00;
		screenBuffer[7] = gameDifficulty == GAME_DIFFICULTY_HARD ? 0xe2 : 0x00;
		screenBuffer[8] = NT_UPD_EOF;

#if DEBUG == 1
		screenBuffer[8] = 0x00;
		screenBuffer[9] = gameDifficulty == GAME_DIFFICULTY_STAT_DUMP ? 0xe2 : 0x00;

		screenBuffer[10] = 0x00;
		screenBuffer[11] = gameDifficulty == GAME_DIFFICULTY_RESET_STATS ? 0xe2 : 0x00;

		screenBuffer[12] = NT_UPD_EOF;
#endif 
		set_vram_update(screenBuffer);
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
	if (titlePhase == 2 && tempChar9 & (PAD_START|PAD_A)) {
		gameState = GAME_STATE_POST_TITLE;
		sfx_play(SFX_CLICKY, SFX_CHANNEL_1);

		return;
	}
}