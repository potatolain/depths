// This file contains global variables that can be accessed from anywhere. 
// You will probably have a lot of variables in here, as the NES has very limited RAM.
// It is often a good idea to reuse a variable for multiple small things, rather than have a bunch. 
// (Especially if you put that variable in zeropage)
#include "source/library/bank_helpers.h"

// If you create variables as ZEROPAGE using the ZEROPAGE_DEF method, be sure to also set them as
// externs using the ZEROPAGE_EXTERN method below. This replaces the regular extern format.
ZEROPAGE_EXTERN(unsigned char, gameState);

ZEROPAGE_EXTERN(unsigned char, i);
ZEROPAGE_EXTERN(unsigned char, j);
ZEROPAGE_EXTERN(unsigned char, playerHealth);
ZEROPAGE_EXTERN(unsigned char, playerlifePreserverCount);
ZEROPAGE_EXTERN(unsigned char, playerMaxHealth);
ZEROPAGE_EXTERN(unsigned char, currentWorldId);

ZEROPAGE_EXTERN(unsigned char, playerStamina);

ZEROPAGE_ARRAY_EXTERN(unsigned char, screenBuffer, 0x25);

ZEROPAGE_EXTERN(unsigned char, controllerState);
ZEROPAGE_EXTERN(unsigned char, lastControllerState);

// Bit of a special case - this is exported from assembly in crt0.asm - it's the number of frames since the system started.
// Note: DO NOT try to alter this with your code; you will almost certainly cause unusual behavior.
ZEROPAGE_EXTERN(unsigned char, frameCount);
ZEROPAGE_EXTERN(unsigned char, everyOtherCycle);

ZEROPAGE_EXTERN(unsigned char, waveDirection);
ZEROPAGE_EXTERN(unsigned char, wavePosition);
extern unsigned char hasGameOvered;
extern unsigned int lastSaveXPosition;
extern unsigned int lastSaveYPosition; 
extern unsigned char lastSaveOverworldPosition;

ZEROPAGE_EXTERN(unsigned char, chrBankTiles);

ZEROPAGE_EXTERN(unsigned char, isStorming);
ZEROPAGE_EXTERN(unsigned char, isRecovery);
ZEROPAGE_EXTERN(unsigned char, gameDifficulty);

#define GAME_DIFFICULTY_PEACEFUL 0
#define GAME_DIFFICULTY_NORMAL 1
// #define GAME_DIFFICULTY_HARD 2

#define GAME_DIFFICULTY_STAT_DUMP 2
#define GAME_DIFFICULTY_RESET_STATS 3



// A few temp variables we'll reuse a lot... be sure not to expect the state of these to stick around
// for *any* function call. If in doubt, create a new variable. These have the advantage of being in 
// the zeropage, (and thus faster) but there is a real risk of creating side-effects in other functions
// that use this variable if you don't take extreme care. Also, NEVER use these in the nmi routine.
ZEROPAGE_EXTERN(unsigned char, tempChar1);
ZEROPAGE_EXTERN(unsigned char, tempChar2);
ZEROPAGE_EXTERN(unsigned char, tempChar3);
ZEROPAGE_EXTERN(unsigned char, tempChar4);
ZEROPAGE_EXTERN(unsigned char, tempChar5);
ZEROPAGE_EXTERN(unsigned char, tempChar6);
ZEROPAGE_EXTERN(unsigned char, tempChar7);
ZEROPAGE_EXTERN(unsigned char, tempChar8);
ZEROPAGE_EXTERN(unsigned char, tempChar9);
ZEROPAGE_EXTERN(unsigned char, tempChara);


ZEROPAGE_EXTERN(int, tempInt1);
ZEROPAGE_EXTERN(int, tempInt2);
ZEROPAGE_EXTERN(int, tempInt3);
ZEROPAGE_EXTERN(int, tempInt4);
ZEROPAGE_EXTERN(int, tempInt5);
ZEROPAGE_EXTERN(int, tempInt6);

ZEROPAGE_EXTERN(long, resetTimer)

extern const unsigned int bitToByte[8];

SRAM_EXTERN(unsigned int, playCount);
SRAM_EXTERN(unsigned int, winCount);
SRAM_ARRAY_EXTERN(unsigned int, checkpointHits, 10);
SRAM_ARRAY_EXTERN(unsigned int, difficultyStartCount, 3);
SRAM_ARRAY_EXTERN(unsigned int, difficultyWinCount, 3);


// TODO: There might be a nicer home for these
// Get the absolute value of a number, the lazy person's way.
#define ABS(x) ((x) > 0 ? x : 0-(x))

// Set a memory address to a value. Very, very raw.
#define SET_ADDR_VALUE(addr, value) *(unsigned char*)(addr) = value


#define DEBUG 0
#define IS_KIOSK 0

// In kiosk mode, how long until we kick you out if you don't do any inputs. Applies to both the "you died" screen and game w/ no input at all.
#define TIMEOUT_MINUTES 10

#if DEBUG == 0
#define GAME_DIFFICULTY_MAX GAME_DIFFICULTY_NORMAL
#else 
#define GAME_DIFFICULTY_MAX GAME_DIFFICULTY_RESET_STATS
#endif