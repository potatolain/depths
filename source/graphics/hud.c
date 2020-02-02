#include "source/neslib_asm/neslib.h"
#include "source/graphics/hud.h"
#include "source/globals.h"

#define LIFE_PRES_TXT 0xdf
#define STAMINA_TXT 0xd9
#define BAR_IMG 0xd0

CODE_BANK(PRG_BANK_HUD);

void draw_hud(void) {
    vram_adr(NAMETABLE_A + HUD_POSITION_START);
    for (i = 0; i != 160; ++i) {
        vram_put(HUD_TILE_BLANK);
    }
    vram_put(HUD_TILE_BORDER_BL);
    for (i = 0; i != 30; ++i) {
        vram_put(HUD_TILE_BORDER_HORIZONTAL);
    }
    vram_put(HUD_TILE_BORDER_BR);

    vram_adr(NAMETABLE_A + HUD_ATTRS_START);
    for (i = 0; i != 16; ++i) {
        vram_put(0xff);
    }

    vram_adr(NAMETABLE_A + HUD_HEART_START - 32);

    for (i = 0; i != 6; ++i) {
        vram_put(STAMINA_TXT + i);
    }
    vram_put(STAMINA_TXT + 2); // a
    
    // Left of preserver icons
    // vram_adr(0x2b60);
    // vram_put(0xa0);

    vram_adr(NAMETABLE_A + HUD_KEY_START - 40);
    vram_put(LIFE_PRES_TXT + 0);
    vram_put(LIFE_PRES_TXT + 1);
    vram_put(LIFE_PRES_TXT + 2);
    vram_put(LIFE_PRES_TXT + 3);
    vram_put(LIFE_PRES_TXT + 2);
    vram_put(LIFE_PRES_TXT + 1);
    vram_put(LIFE_PRES_TXT + 4);
    vram_put(LIFE_PRES_TXT + 2);
    vram_put(LIFE_PRES_TXT + 1);
    vram_put(LIFE_PRES_TXT + 3);

    
}

// HACK: use the buffer from game_text
extern unsigned char buffer[132];

void draw_hud_oneshot(void) {
    i = 0;
    buffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START - 32) | NT_UPD_HORZ;
    buffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START - 32);
    buffer[i++] = 7;
    for (j = 0; j != 6; ++j) {
        buffer[i++] = STAMINA_TXT + j;
    }
    buffer[i++] = STAMINA_TXT + 2; // a


    buffer[i++] = MSB(NAMETABLE_A + HUD_KEY_START - 40) | NT_UPD_HORZ;
    buffer[i++] = LSB(NAMETABLE_A + HUD_KEY_START - 40);
    buffer[i++] = 10;

    buffer[i++] = LIFE_PRES_TXT + 0;
    buffer[i++] = LIFE_PRES_TXT + 1;
    buffer[i++] = LIFE_PRES_TXT + 2;
    buffer[i++] = LIFE_PRES_TXT + 3;
    buffer[i++] = LIFE_PRES_TXT + 2;
    buffer[i++] = LIFE_PRES_TXT + 1;
    buffer[i++] = LIFE_PRES_TXT + 4;
    buffer[i++] = LIFE_PRES_TXT + 2;
    buffer[i++] = LIFE_PRES_TXT + 1;
    buffer[i++] = LIFE_PRES_TXT + 3;
    
    buffer[i++] = NT_UPD_EOF;

    set_vram_update(buffer);

}

void update_hud(void) {
    // This sets up screenBuffer to print x hearts, then x more empty hearts. 
    // You give it the address, tell it the direction to write, then follow up with
    // Ids, ending with NT_UPD_EOF
    
    // We use i for the index on screen buffer, so we don't have to shift things around
    // as we add values. 
    i = 0;
    tempChar1 = playerStamina >> 1;
    screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START+1) | NT_UPD_HORZ;
    screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START+1);
    screenBuffer[i++] = 8;

    for (j = 0; j != 8; ++j) {
        if (tempChar1 > 7) {
            screenBuffer[i++] = BAR_IMG+8;
            tempChar1 -= 8;
        } else {
            screenBuffer[i++] = BAR_IMG+tempChar1;
            tempChar1 = 0;
        }
    }

    // Next, draw the preserver count, using the key tile, and our key count variable
    screenBuffer[i++] = MSB(NAMETABLE_A + 0x379) | NT_UPD_HORZ;
    screenBuffer[i++] = LSB(NAMETABLE_A + 0x379);
    screenBuffer[i++] = 2;
    screenBuffer[i++] = HUD_TILE_NUMBER;
    screenBuffer[i++] = HUD_TILE_NUMBER + playerlifePreserverCount;



    tempChar1 = 0x04;
    tempChar2 = 0x22;
    if (playerStamina < 17) {
        tempChar1 = 0x06;
        tempChar2 = 0x16;
    } else if (playerStamina < 33) {
        tempChar1 = 0x16;
        tempChar2 = 0x26;
    } else if (playerStamina < 49) {
        tempChar1 = 0x26;
        tempChar2 = 0x37;
    }
    screenBuffer[i++] = MSB(0x3f0d) | NT_UPD_HORZ;
    screenBuffer[i++] = LSB(0x3f0d);
    screenBuffer[i++] = 2;
    screenBuffer[i++] = tempChar1;
    screenBuffer[i++] = tempChar2;
    screenBuffer[i++] = NT_UPD_EOF;
    set_vram_update(screenBuffer);

}