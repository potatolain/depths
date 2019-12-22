#include "source/neslib_asm/neslib.h"
#include "source/sprites/player.h"
#include "source/library/bank_helpers.h"
#include "source/globals.h"
#include "source/map/map.h"
#include "source/configuration/game_states.h"
#include "source/configuration/system_constants.h"
#include "source/sprites/collision.h"
#include "source/sprites/sprite_definitions.h"
#include "source/sprites/map_sprites.h"
#include "source/menus/error.h"
#include "source/graphics/hud.h"
#include "source/graphics/game_text.h"
#include "source/menus/error.h"

CODE_BANK(PRG_BANK_PLAYER_SPRITE);

// Some useful global variables
ZEROPAGE_DEF(int, playerXPosition);
ZEROPAGE_DEF(int, playerYPosition);
ZEROPAGE_DEF(int, playerXVelocity);
ZEROPAGE_DEF(int, playerYVelocity);
ZEROPAGE_DEF(unsigned char, playerControlsLockTime);
ZEROPAGE_DEF(unsigned char, playerInvulnerabilityTime);
ZEROPAGE_DEF(unsigned char, playerDirection);
ZEROPAGE_DEF(unsigned char, playerMaxStamina);

ZEROPAGE_DEF(unsigned char, inSlowStuff);

// Huge pile of temporary variables
#define rawXPosition tempChar1
#define rawYPosition tempChar2
#define rawTileId tempChar3
#define collisionTempX tempChar4
#define collisionTempY tempChar5
#define collisionTempXRight tempChar6
#define collisionTempYBottom tempChar7

#define tempSpriteCollisionX tempInt1
#define tempSpriteCollisionY tempInt2

#define collisionTempXInt tempInt3
#define collisionTempYInt tempInt4

#define currentMapSpriteIndex tempChar1


//                                "                              "


const unsigned char* preserverText[] = {


                                "Huh? This live preserver has a"
                                "small bottle of pills on it.  "
                                "                              "

                                "wait... these are my pills! My"
                                "name is on the bottle. How did"
                                "they get here?                "

                                "Weird... as I collect them, I "
                                "feel I can stay afloat just a "
                                "little longer."
                                
                                ,


                                "Touching this raft, I feel    "
                                "memories of a close friend    "
                                "come flooding back.           "

                                "I have to press on for the    "
                                "them. They wouldn't want me to"
                                "drown out here.               "

                                "Remembering this, I feel I can"
                                "keep going even farther!"
                                
                                ,


                                "Did... did this life raft just"
                                "meow at me?                   "
                                "                              "

                                "It seems to have a face. It   "
                                "looks just like my cat. She's "
                                "waiting for me at home.       "

                                "I need to stay afloat for her."
                                "She can't take care of        "
                                "herself!                      "
                                
                                "Another life preserver in my  "
                                "collection always helps.      "
                                "                              "
                                
                                "Come on Sadie <3"
                                
                                ,

                                "This raft has a game          "
                                "cartridge pattern on it...    "
                                "                              "
                                
                                "It reminds me of a goal of    "
                                "mine. I want to make a game   "
                                "that changes someone's life.  "

                                "Nothing I have made has done  "
                                "that yet. I have to stay      "
                                "afloat to keep trying!        "

                                "Remembering this, I find my   "
                                "will to carry on increasing.  "
                                
                                ,

                                "Another preserver... I am     "
                                "building quite the collection."
                                "                              "

                                "The letters on this; those are"
                                "my mother's initials. What    "
                                "would she think of me drowning"

                                "out here?                     "
                                "                              "
                                "                              "

                                "I have to stay afloat for her."
                                "                              "
                                "                              "

                                "I must.                       "
                                "                              "
                                "                              "

                                "This little reminder gave me  "
                                "just a bit more strength.     "
                                "                              "
                                "I will stay afloat!           "
                                
};

const unsigned char* startFirstPlayText = 
                                "Where am I? I don't recall how"
                                "I got here...                 "
                                "                              "
                                
                                "I'm in the middle of the ocean"
                                "I have to stay afloat...      ";

const unsigned char* startPostGameOverText = 
                                "Somehow I found myself back   "
                                "here, still alive and afloat. "
                                "                              "

                                "I can't let myself get dragged"
                                "to the bottom like that!      "
                                "                              "

                                "I need to try to stay afloat a"
                                "bit longer...";


// If I throw an npc in the wrong spot, it'll still be usefulish.
const unsigned char* errorText = "Stay afloat, human!";

const unsigned char* introductionText = 
                                "Hello Human! How did you get  " 
                                "here?                         "
                                "                              "

                                "Humans don't belong in the    "
                                "ocean. You don't have gills!  "
                                "You just tread water...       "
                                
                                "There is a life preserver     "
                                "in front of me. It may help   "
                                "you stay afloat longer...     "
                                
                                "Stay afloat, human!";

const unsigned char* wetText = 
                                "Human! The water is very deep "
                                "here. Please be sure to rest  "
                                "before you go too far.        "

                                "That driftwood behind you is  "
                                "safe to use.                  "
                                "                              "
                                "Stay afloat, human!";

const unsigned char* bufText = 
                                "The rocks ahead form a maze,  "
                                "human! It is very dangerous!  "
                                "                              "

                                "Follow the bottom path, then  "
                                "go down at the first chance   "
                                "you get!                      "
                                
                                "Please keep treading water,   "
                                "human.";

const unsigned char* lostText = 
                                "What are you doing out here,  "
                                "human? Didn't you hear my     "
                                "warning?                      "

                                "This path goes nowhere, and   "
                                "is extremely dangerous. You   "
                                "nearly drowned!               "

                                "I followed you after seeing   "
                                "you make the wrong turn. I    "
                                "am sorry I can't help further."

                                "This path meets up with the   "
                                "other one below. Just keep    "
                                "going. Be careful!";

const unsigned char* stormySeas = 
                                "The sea in front of you is    "
                                "churning a lot! You'll have   "
                                "to fight the current to move. "

                                "Be careful, human! Don't let  "
                                "it pull you down!";
const unsigned char* nothingHere =
                                "Sorry, human. There is nothing"
                                "here but this driftwood...    "
                                "                              "
                                
                                "You're really getting         "
                                "resillient. I didn't think you"
                                "could even make it here.      "
                                
                                "You'll have to turn around    "
                                "now. Try to get back to more  "
                                "driftwood. It's a long swim.  "

                                "Stay hopeful, human!";

const unsigned char* toTheLeftToTheLeft =  // sorry
                                "Human! There is another raft  "
                                "just to the left of here. You "
                                "look like you need it!        "

                                "You are getting adept at      "
                                "staying afloat. Don't give up!"
                                "                              "

                                "Things are going to get tough,"
                                "but I think you are tougher.  "
                                "                              "

                                "Stay afloat, human!";

const unsigned char* aGameEh = 
                                "I saw a smile as you saw that "
                                "raft. Does the symbol mean    "
                                "something special to you?     "

                                "I see. Human, I hope you one  "
                                "day make that happen. But     "
                                "to do it,you must stay afloat!"

                                "You can do it, human!";

const unsigned char* veryChoppySeas =
                                "Human! The seas ahead are even"
                                "more choppy than before!      "
                                "                              "
                                
                                "It's scaring even me. And I   "
                                "live in these seas.           "
                                "                              "

                                "Please, try to stay afloat... "
                                "You've come this far!         "; 

const unsigned char* gettingClose =
                                "Human, there is one last      "
                                "life preserver ahead. Grab it!"
                                "                              "

                                "Stay alive, human!";


const unsigned char* straightShot = 
                                "Human, your stamina is higher "
                                "than I thought possible. You  "
                                "just might be able to make it "

                                "across this channel.          "
                                "                              "
                                "                              "

                                "Just swim straight there, and "
                                "avoid the current! It's a long"
                                "swim, but can do it.          "

                                "I believe in you, human!";

const unsigned char* nearlyThere =
                                "You made it!                  "
                                "                              "
                                "                              "
                                
                                "Human, that is the furthest I "
                                "have seen person swim without "
                                "a break. You should be proud. "
                                
                                "Swim down from here. There is "
                                "a huge raft you may be able   "
                                "to rest on.                   "

                                "Keep floating, human.         "
                                "                              "
                                "Farewell!";


const unsigned char* lookADriftwood = 
                                "Did you see the weird pattern "
                                "on that driftwood? It might   "
                                "have a story behind it!       "

                                "(Press the A button in front  "
                                "of it)                        "
                                "                              "

                                "These can help you stay       "
                                "afloat. Every little thing    "
                                "helps.                        "
                                
                                "Stay afloat, human!"; 

const unsigned char* stormBrew = 
                                "A storm seems to have rolled  "
                                "in! I'd better be careful!";



const unsigned char* get_npc_text() {
    switch (playerOverworldPosition) {

        case 11:
            return bufText;

        case 14:
            return lostText;

        case 18:
            return nearlyThere;

        case 20:
            return wetText;

        case 27:
            return introductionText;

        case 28:
            return lookADriftwood;

        case 29:
            return stormySeas;

        case 34:
            return aGameEh;
        case 35:
            return toTheLeftToTheLeft;

        case 53:
            return nothingHere;

        case 54:
            return veryChoppySeas;

        case 56:
            return straightShot;

        case 57:
            return gettingClose;

        default:
            return errorText;
    }
}

const unsigned char* get_preserver_text() {
    switch (playerOverworldPosition) {
        case 11:
            return preserverText[1];
        case 27:
            return preserverText[0];
        case 29: 
            return preserverText[2];
        case 34:
            return preserverText[3];
        case 56:
            return preserverText[4];
        default:
            return errorText;

    }
}

const unsigned char* woodError = 
                                "The design on this driftwood  "
                                "reminds me of something, but I"
                                "can't remember what...";

const unsigned char* woodGame = 
                                "This driftwood has a pattern  "
                                "like a controller on it...    "
                                "                              "

                                "This reminds me that I still  "
                                "need to find out what happens "
                                "to Chloe Price.               "

                                "One more reason to keep       "
                                "treading water...";

const unsigned char* woodGame2 = 
                                "This driftwood has a pattern  "
                                "like a controller on it...    "
                                "                              "
                                
                                "It reminds me that I still    "
                                "never beat Final Fantasy 9.   "
                                "                              "
                                
                                "I guess I can keep floating   "
                                "until I play through it.";

const unsigned char* woodBook = 
                                "This driftwood has a pattern  "
                                "like a book on it...          "
                                "                              "

                                "I have so many books piled up "
                                "to read...                    "
                                "                              "

                                "I'll never get to them if I   "
                                "get pulled under today.       "
                                "                              "

                                "I guess I'll have to stay     "
                                "afloat...";

const unsigned char* woodBook2 =
                                "This driftwood has a pattern  "
                                "like a book on it...          "
                                "                              "

                                "Touching this reminds me that "
                                "I still haven't read \"What we "
                                "Did Before\" yet.              "

                                "I need to find time for that! "
                                "                              "
                                "                              "

                                "I Just have to keep treading  "
                                "water...";

const unsigned char* woodTv = 
                                "This driftwood has a pattern  "
                                "like a TV on it...            "
                                "                              "

                                "I still have so many series   "
                                "to catch up on.               "
                                "                              "
                                
                                "Maybe it's not the best       "
                                "reason to stay afloat, but    "
                                "it's still something.";

const unsigned char* woodTv2 = 
                                "This driftwood has a pattern  "
                                "like a TV on it...            "
                                "                              "
                                
                                "It reminds me, I still haven't"
                                "caught up on Dr. Who. I still "
                                "need to see how Jodie         "
                                
                                "Whittaker portrays the doctor "
                                "                              "
                                "                              "

                                "Maybe I'll watch it soon. I'll"
                                "have to float on to do it.";

const unsigned char* woodMusic = 
                                "This driftwood has a pattern  "
                                "like a musical note on it...  "
                                "                              "

                                "Touching this reminds me that "
                                "Tom Shear will probably       "
                                "release a new album soon.     "

                                "It would be a shame to never  "
                                "hear it. I guess I'll keep    "
                                "treading water for now...";

const unsigned char* woodMusic2 = 
                                "This driftwood has a pattern  "
                                "like a musical note on it...  "
                                "                              "

                                "Touching this, I realize I    "
                                "I still haven't seen Daughter "
                                "live.                         "

                                "Maybe they will come to my    "
                                "country some day. I have to   "
                                "keep floating and see!";

const unsigned char* woodPeople = 
                                "This driftwood has a pattern  "
                                "like a face on it...          "
                                "                              "

                                "It sort of reminds me of my   "
                                "crush. I still haven't asked  "
                                "them out.                     "

                                "I guess I should find out how "
                                "they feel about me. If I keep "
                                "floating, maybe I can...";

const unsigned char* woodPeople2 =
                                "This driftwood has a pattern  "
                                "like a face on it...          "
                                "                              "

                                "Touching it, I recall the     "
                                "huge party my friends are     "
                                "throwing next month.          "

                                "I wouldn't want to miss that! "
                                "I guess I'll have to carry on.";


//"                              ";
const unsigned char* get_wood_text() {

    if (currentMapSpriteData[(currentMapSpriteIndex) + 13] == 0) {
        switch (currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TILE_ID]) {
            case 0xa0:
                return woodBook;
            case 0xa2:
                return woodGame;
            case 0xa4:
                return woodMusic;
            case 0xa6:
                return woodPeople;
            case 0xa8:
                return woodTv;

            default: 
                return woodError;
        }
    } else {

        switch (currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TILE_ID]) {
            case 0xa0:
                return woodBook2;
            case 0xa2:
                return woodGame2;
            case 0xa4:
                return woodMusic2;
            case 0xa6:
                return woodPeople2;
            case 0xa8:
                return woodTv2;

            default: 
                return woodError;
        }

    }
}

void trigger_game_start_text(void) {
    if (hasGameOvered) {
        trigger_game_text(startPostGameOverText);
    } else {
        trigger_game_text(startFirstPlayText);
    }
}

// NOTE: This uses tempChar1 through tempChar3; the caller must not use these.
void update_player_sprite(void) {
    // Calculate the position of the player itself, then use these variables to build it up with 4 8x8 NES sprites.
    rawXPosition = (playerXPosition >> PLAYER_POSITION_SHIFT);
    rawYPosition = (playerYPosition >> PLAYER_POSITION_SHIFT);
    rawTileId = PLAYER_SPRITE_TILE_ID + playerDirection;

    if (playerXVelocity != 0 || playerYVelocity != 0) {
        // Does some math with the current NES frame to add either 2 or 0 to the tile id, animating the sprite.
        rawTileId += ((frameCount >> SPRITE_ANIMATION_SPEED_DIVISOR) & 0x01) << 1;
    }
    
    if (playerInvulnerabilityTime && frameCount & PLAYER_INVULNERABILITY_BLINK_MASK) {
        // If the player is invulnerable, we hide their sprite about half the time to do a flicker animation.
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId, 0x00, PLAYER_SPRITE_INDEX);
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId + 1, 0x00, PLAYER_SPRITE_INDEX+4);
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId + 16, 0x00, PLAYER_SPRITE_INDEX+8);
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId + 17, 0x00, PLAYER_SPRITE_INDEX+12);

    } else {
        oam_spr(rawXPosition, rawYPosition, rawTileId, 0x00, PLAYER_SPRITE_INDEX);
        oam_spr(rawXPosition + NES_SPRITE_WIDTH, rawYPosition, rawTileId + 1, 0x00, PLAYER_SPRITE_INDEX+4);
        oam_spr(rawXPosition, rawYPosition + NES_SPRITE_HEIGHT, rawTileId + 16, 0x00, PLAYER_SPRITE_INDEX+8);
        oam_spr(rawXPosition + NES_SPRITE_WIDTH, rawYPosition + NES_SPRITE_HEIGHT, rawTileId + 17, 0x00, PLAYER_SPRITE_INDEX+12);
    }

}

void handle_player_movement(void) {
    // Using a variable, so we can change the velocity based on pressing a button, having a special item,
    // or whatever you like!
    int maxVelocity = PLAYER_MAX_VELOCITY;
    lastControllerState = controllerState;
    controllerState = pad_poll(0);

#if IS_KIOSK == 1
    if (controllerState == lastControllerState) {
        ++resetTimer;
    } else {
        resetTimer = 0;
    }
    // How long to wait until dumping, frames * seconds * minutes
    if (resetTimer > (60 * 60 * TIMEOUT_MINUTES)) {
        reset();
    }
#endif

    // If Start is pressed now, and was not pressed before...
    if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
        gameState = GAME_STATE_PAUSED;
        return;
    }
    #if DEBUG == 1
        if (controllerState & PAD_SELECT && !(lastControllerState & PAD_SELECT)) {
            isStorming = !isStorming;
        }
    #endif

    if (inSlowStuff) {
        if (!(frameCount & 0x0f)) {
            sfx_play(SFX_CURRENTS, SFX_CHANNEL_4);
        }
        maxVelocity >>= 2;
        if (isStorming) {
            maxVelocity >>= 1;
        }
    }
    

    if (playerControlsLockTime) {
        // If your controls are locked, just tick down the timer until they stop being locked. Don't read player input.
        playerControlsLockTime--;
    } else {
        if (controllerState & PAD_RIGHT && playerXVelocity >= 0) {
            // If you're moving right, and you're not at max, speed up.
            if (playerXVelocity < maxVelocity) {
                playerXVelocity += PLAYER_VELOCITY_ACCEL;
            // If you're over max somehow, we'll slow you down a little.
            } else if (playerXVelocity > maxVelocity) {
                playerXVelocity -= PLAYER_VELOCITY_ACCEL;
            }
        } else if (controllerState & PAD_LEFT && playerXVelocity <= 0) {
            // If you're moving left, and you're not at max, speed up.
            if (ABS(playerXVelocity) < maxVelocity) {
                playerXVelocity -= PLAYER_VELOCITY_ACCEL;
            // If you're over max, slow you down a little...
            } else if (ABS(playerXVelocity) > maxVelocity) { 
                playerXVelocity += PLAYER_VELOCITY_ACCEL;
            }
        } else if (playerXVelocity != 0) {
            // Not pressing anything? Let's slow you back down...
            if (playerXVelocity > 0) {
                playerXVelocity -= PLAYER_VELOCITY_ACCEL;
            } else {
                playerXVelocity += PLAYER_VELOCITY_ACCEL;
            }
        }

        if (controllerState & PAD_UP && playerYVelocity <= 0) {
            if (ABS(playerYVelocity) < maxVelocity) {
                playerYVelocity -= PLAYER_VELOCITY_ACCEL;
            } else if (ABS(playerYVelocity) > maxVelocity) {
                playerYVelocity += PLAYER_VELOCITY_ACCEL;
            }
        } else if (controllerState & PAD_DOWN && playerYVelocity >= 0) {
            if (playerYVelocity < maxVelocity) {
                playerYVelocity += PLAYER_VELOCITY_ACCEL;
            } else if (playerYVelocity > maxVelocity) {
                playerYVelocity -= PLAYER_VELOCITY_ACCEL;
            }
        } else { 
            if (playerYVelocity > 0) {
                playerYVelocity -= PLAYER_VELOCITY_ACCEL;
            } else if (playerYVelocity < 0) {
                playerYVelocity += PLAYER_VELOCITY_ACCEL;
            }
        }
    }

    // While we're at it, tick down the invulnerability timer if needed
    if (playerInvulnerabilityTime) {
        playerInvulnerabilityTime--;
    }

    // This will knock out the player's speed if they hit anything.
    test_player_tile_collision();
    handle_player_sprite_collision();

    rawXPosition = (playerXPosition >> PLAYER_POSITION_SHIFT);
    rawYPosition = (playerYPosition >> PLAYER_POSITION_SHIFT);
        if (rawXPosition > SCREEN_EDGE_RIGHT) {
        // We use sprite direction to determine which direction to scroll in, so be sure this is set properly.
        if (playerInvulnerabilityTime) {
            playerXPosition -= playerXVelocity;
            rawXPosition = (playerXPosition >> PLAYER_POSITION_SHIFT);
        } else {
            playerDirection = SPRITE_DIRECTION_RIGHT;
            gameState = GAME_STATE_SCREEN_SCROLL;
            playerOverworldPosition++;
        }
    } else if (rawXPosition < SCREEN_EDGE_LEFT) {
        if (playerInvulnerabilityTime) {
            playerXPosition -= playerXVelocity;
            rawXPosition = (playerXPosition >> PLAYER_POSITION_SHIFT);
        } else {
            playerDirection = SPRITE_DIRECTION_LEFT;
            gameState = GAME_STATE_SCREEN_SCROLL;
            playerOverworldPosition--;
        }
    } else if (rawYPosition > SCREEN_EDGE_BOTTOM) {
        if (playerInvulnerabilityTime) {
            playerYPosition -= playerYVelocity;
            rawYPosition = (playerYPosition >> PLAYER_POSITION_SHIFT);
        } else {
            playerDirection = SPRITE_DIRECTION_DOWN;
            gameState = GAME_STATE_SCREEN_SCROLL;
            playerOverworldPosition += 8;
        }
    } else if (rawYPosition < SCREEN_EDGE_TOP) {
        if (playerInvulnerabilityTime) {
            playerYPosition -= playerYVelocity;
            rawYPosition = (playerYPosition >> PLAYER_POSITION_SHIFT);
        } else {
            playerDirection = SPRITE_DIRECTION_UP;
            gameState = GAME_STATE_SCREEN_SCROLL;
            playerOverworldPosition -= 8;
        }
    }

    if (!isRecovery && (frameCount & 0x07) == 0x07) {
        
        if ((gameDifficulty != GAME_DIFFICULTY_PEACEFUL) || ((frameCount & 0x15) == 0x15)) {
            playerStamina--;
            if (playerStamina == 48) {
                sfx_play(SFX_DROWN1, SFX_CHANNEL_4);
            } else if (playerStamina == 32) {
                sfx_play(SFX_DROWN2, SFX_CHANNEL_2);
            } else if (playerStamina == 16) {
                sfx_play(SFX_DROWN3, SFX_CHANNEL_1);
            }

            if (playerStamina == 0 || playerStamina > 240) {
                gameState = GAME_STATE_GAME_OVER;
                music_stop();
                sfx_play(SFX_GAMEOVER, SFX_CHANNEL_1);
                return;
            }
        }

    }

}

void test_player_tile_collision(void) {

	if (playerYVelocity != 0) {
        collisionTempYInt = playerYPosition + PLAYER_Y_OFFSET_EXTENDED + playerYVelocity;
        collisionTempXInt = playerXPosition + PLAYER_X_OFFSET_EXTENDED;

		collisionTempY = ((collisionTempYInt) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
		collisionTempX = ((collisionTempXInt) >> PLAYER_POSITION_SHIFT);

        collisionTempYInt += PLAYER_HEIGHT_EXTENDED;
        collisionTempXInt += PLAYER_WIDTH_EXTENDED;

        collisionTempYBottom = ((collisionTempYInt) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
        collisionTempXRight = (collisionTempXInt) >> PLAYER_POSITION_SHIFT;
        
        // Due to how we are calculating the sprite's position, there is a slight chance we can exceed the area that our
        // map takes up near screen edges. To compensate for this, we clamp the Y position of our character to the 
        // window between 0 and 192 pixels, which we can safely test collision within.

        // If collisionTempY is > 240, it can be assumed we calculated a position less than zero, and rolled over to 255
        if (collisionTempY > 240) {
            collisionTempY = 0;
        }
        // The lowest spot we can test collision is at pixel 192 (the 12th 16x16 tile). If we're past that, bump ourselves back.
        if (collisionTempYBottom > 190) { 
            collisionTempYBottom = 190;
        }

		if (playerYVelocity < 0) {
            // We're going up - test the top left, and top right
			if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempY)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempY)], 1)) {
                playerYVelocity = 0;
                playerControlsLockTime = 0;
            } 
            if (!playerControlsLockTime) {
                playerDirection = SPRITE_DIRECTION_UP;
            }
		} else {
            // Okay, we're going down - test the bottom left and bottom right
			if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempYBottom)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempYBottom)], 1)) {
                playerYVelocity = 0;
                playerControlsLockTime = 0;

			}
            if (!playerControlsLockTime) {
                playerDirection = SPRITE_DIRECTION_DOWN;
            }
		}
    }

	if (playerXVelocity != 0) {
        collisionTempXInt = playerXPosition + PLAYER_X_OFFSET_EXTENDED + playerXVelocity;
        collisionTempYInt = playerYPosition + PLAYER_Y_OFFSET_EXTENDED + playerYVelocity;
		collisionTempX = (collisionTempXInt) >> PLAYER_POSITION_SHIFT;
		collisionTempY = ((collisionTempYInt) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;

        collisionTempXInt += PLAYER_WIDTH_EXTENDED;
        collisionTempYInt += PLAYER_HEIGHT_EXTENDED;

        collisionTempYBottom = ((collisionTempYInt) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
        collisionTempXRight = ((collisionTempXInt) >> PLAYER_POSITION_SHIFT);


        // Depending on how far to the left/right the player is, there's a chance part of our bounding box falls into
        // the next column of tiles on the opposite side of the screen. This if statement prevents those collisions.
        if (collisionTempX > 2 && collisionTempX < 238) {
            if (playerXVelocity < 0) {
                // Okay, we're moving left. Need to test the top-left and bottom-left
                if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempY)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempYBottom)], 1)) {
                    playerXVelocity = 0;
                    playerControlsLockTime = 0;

                }
                if (!playerControlsLockTime) {
                    playerDirection = SPRITE_DIRECTION_LEFT;
                }
            } else {
                // Going right - need to test top-right and bottom-right
                if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempY)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempYBottom)], 1)) {
                    playerXVelocity = 0;
                    playerControlsLockTime = 0;

                }
                if (!playerControlsLockTime) {
                    playerDirection = SPRITE_DIRECTION_RIGHT;
                }
            }
        }
	}

    collisionTempX += 6;
    collisionTempY += 6;
    // NOTE: This is a HACK
    collisionTempXRight = currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempY)] & 0x3f;

    if (collisionTempXRight > 15 && collisionTempXRight < 23) {
        inSlowStuff = 1;
    } else {
        inSlowStuff = 0;
    }

    playerXPosition += playerXVelocity;
    playerYPosition += playerYVelocity;

}

ZEROPAGE_DEF(unsigned char, spriteType);

void handle_player_sprite_collision(void) {
    isRecovery = 0;
    // We store the last sprite hit when we update the sprites in `map_sprites.c`, so here all we have to do is react to it.
    if (lastPlayerSpriteCollisionId != NO_SPRITE_HIT) {
        currentMapSpriteIndex = lastPlayerSpriteCollisionId<<MAP_SPRITE_DATA_SHIFT;
        spriteType = currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE];
        switch (spriteType) {
            case SPRITE_TYPE_HEALTH:
                // This if statement ensures that we don't remove hearts if you don't need them yet.
                if (playerHealth < playerMaxHealth) {
                    playerHealth += currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_HEALTH];
                    if (playerHealth > playerMaxHealth) {
                        playerHealth = playerMaxHealth;
                    }
                    // Hide the sprite now that it has been taken.
                    currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE] = SPRITE_TYPE_OFFSCREEN;

                    // Play the heart sound!
                    sfx_play(SFX_HEART, SFX_CHANNEL_3);

                    // Mark the sprite as collected, so we can't get it again.
                    currentMapSpritePersistance[playerOverworldPosition] |= bitToByte[lastPlayerSpriteCollisionId];
                }
                break;
            case SPRITE_TYPE_KEY:
                if (playerlifePreserverCount < MAX_LIFE_PRESERVER_COUNT) {
                    crash_error("Keys not implemented", "How did you do this?", "???", playerlifePreserverCount);
                    playerlifePreserverCount++;
                    currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE] = SPRITE_TYPE_OFFSCREEN;
                    playerStamina = playerMaxStamina;

                    sfx_play(SFX_KEY, SFX_CHANNEL_3);

                    // Mark the sprite as collected, so we can't get it again.
                    currentMapSpritePersistance[playerOverworldPosition] |= bitToByte[lastPlayerSpriteCollisionId];
                }
                break;
            case SPRITE_TYPE_REGULAR_ENEMY:
            case SPRITE_TYPE_INVULNERABLE_ENEMY:

                if (playerInvulnerabilityTime) {
                    return;
                }
                playerHealth -= currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_DAMAGE]; 
                // Since playerHealth is unsigned, we need to check for wraparound damage. 
                // NOTE: If something manages to do more than 16 damage at once, this might fail.
                if (playerHealth == 0 || playerHealth > 240) {
                    gameState = GAME_STATE_GAME_OVER;
                    music_stop();
                    sfx_play(SFX_GAMEOVER, SFX_CHANNEL_1);
                    return;
                }
                // Knock the player back
                playerControlsLockTime = PLAYER_DAMAGE_CONTROL_LOCK_TIME;
                playerInvulnerabilityTime = PLAYER_DAMAGE_INVULNERABILITY_TIME;
                if (playerDirection == SPRITE_DIRECTION_LEFT) {
                    // Punt them back in the opposite direction
                    playerXVelocity = PLAYER_MAX_VELOCITY;
                    // Reverse their velocity in the other direction, too.
                    playerYVelocity = 0 - playerYVelocity;
                } else if (playerDirection == SPRITE_DIRECTION_RIGHT) {
                    playerXVelocity = 0-PLAYER_MAX_VELOCITY;
                    playerYVelocity = 0 - playerYVelocity;
                } else if (playerDirection == SPRITE_DIRECTION_DOWN) {
                    playerYVelocity = 0-PLAYER_MAX_VELOCITY;
                    playerXVelocity = 0 - playerXVelocity;
                } else { // Make being thrown downward into a catch-all, in case your direction isn't set or something.
                    playerYVelocity = PLAYER_MAX_VELOCITY;
                    playerXVelocity = 0 - playerXVelocity;
                }
                sfx_play(SFX_HURT, SFX_CHANNEL_2);

                
                break;
            case SPRITE_TYPE_DOOR: 
                // Doors without locks are very simple - they just open! Hide the sprite until the user comes back...
                // note that we intentionally *don't* store this state, so it comes back next time.
                currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE] = SPRITE_TYPE_OFFSCREEN;
                break;
            case SPRITE_TYPE_LOCKED_DOOR:
                // First off, do you have a key? If so, let's just make this go away...
                crash_error("Locked doors aren't implemented", "How did you do this?", "???", playerlifePreserverCount);
                if (playerlifePreserverCount > 0) {
                    playerlifePreserverCount--;
                    currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE] = SPRITE_TYPE_OFFSCREEN;

                    // Mark the door as gone, so it doesn't come back.
                    currentMapSpritePersistance[playerOverworldPosition] |= bitToByte[lastPlayerSpriteCollisionId];

                    break;
                }
                // So you don't have a key...
                // Okay, we collided with a door before we calculated the player's movement. After being moved, does the 
                // new player position also collide? If so, stop it. Else, let it go.

                // Calculate position...
                // HACK: Just let tile collision handle this bit.
                /*
                tempSpriteCollisionX = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X + 1]) << 8));
                tempSpriteCollisionY = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y + 1]) << 8));

                // Are we colliding?
                // NOTE: We take a bit of a shortcut here and assume all doors are 16x16 (the hard-coded 16 value below)
                if (
                    playerXPosition < tempSpriteCollisionX + (16 << PLAYER_POSITION_SHIFT) &&
                    playerXPosition + PLAYER_WIDTH_EXTENDED > tempSpriteCollisionX &&
                    playerYPosition < tempSpriteCollisionY + (16 << PLAYER_POSITION_SHIFT) &&
                    playerYPosition + PLAYER_HEIGHT_EXTENDED > tempSpriteCollisionY
                ) {
                    playerXPosition -= playerXVelocity;
                    playerYPosition -= playerYVelocity;
                    playerControlsLockTime = 0;
                }
                */
                break;
            case SPRITE_TYPE_ENDGAME:
                gameState = GAME_STATE_CREDITS;
                break;
            case SPRITE_TYPE_NPC:
                isRecovery = 1;
                // Okay, we collided with this NPC before we calculated the player's movement. After being moved, does the 
                // new player position also collide? If so, stop it. Else, let it go.

                // HACK: Just let tile collision handle this bit.
                /*

                // Calculate position...
                tempSpriteCollisionX = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X + 1]) << 8));
                tempSpriteCollisionY = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y + 1]) << 8));
                // Are we colliding?
                // NOTE: We take a bit of a shortcut here and assume all NPCs are 16x16 (the hard-coded 16 value below)
                if (
                    playerXPosition < tempSpriteCollisionX + (16 << PLAYER_POSITION_SHIFT) &&
                    playerXPosition + PLAYER_WIDTH_EXTENDED > tempSpriteCollisionX &&
                    playerYPosition < tempSpriteCollisionY + (16 << PLAYER_POSITION_SHIFT) &&
                    playerYPosition + PLAYER_HEIGHT_EXTENDED > tempSpriteCollisionY
                ) {
                    playerXPosition -= playerXVelocity;
                    playerYPosition -= playerYVelocity;
                    playerControlsLockTime = 0;
                }
*/
                if (controllerState & PAD_A && !(lastControllerState & PAD_A)) {
                    // Show the text for the player on the first screen
                    trigger_game_text(get_npc_text());
                }
                break;
            case SPRITE_TYPE_PRESERVER:
                // Okay, we collided with this NPC before we calculated the player's movement. After being moved, does the 
                // new player position also collide? If so, stop it. Else, let it go.

                // Calculate position...
                // HACK: Just let tile collision handle this bit.
                /*

                tempSpriteCollisionX = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X + 1]) << 8));
                tempSpriteCollisionY = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y + 1]) << 8));
                // Are we colliding?
                // NOTE: We take a bit of a shortcut here and assume all NPCs are 16x16 (the hard-coded 16 value below)
                if (
                    playerXPosition < tempSpriteCollisionX + (16 << PLAYER_POSITION_SHIFT) &&
                    playerXPosition + PLAYER_WIDTH_EXTENDED > tempSpriteCollisionX &&
                    playerYPosition < tempSpriteCollisionY + (16 << PLAYER_POSITION_SHIFT) &&
                    playerYPosition + PLAYER_HEIGHT_EXTENDED > tempSpriteCollisionY
                ) {
                    playerXPosition -= playerXVelocity;
                    playerYPosition -= playerYVelocity;
                    playerControlsLockTime = 0;
                }
*/
                if (playerlifePreserverCount < MAX_LIFE_PRESERVER_COUNT) {
                    // Show the text for the player on the first screen
                    lastSaveXPosition = playerXPosition;
                    lastSaveYPosition = playerYPosition;
                    lastSaveOverworldPosition = playerOverworldPosition;

                    trigger_game_text(get_preserver_text());
                    ++checkpointHits[playerlifePreserverCount];


                    ++playerlifePreserverCount;
                    currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE] = SPRITE_TYPE_DRIFTWOOD;
                    currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_SIZE_PALETTE] = SPRITE_SIZE_16PX_16PX | SPRITE_PALETTE_2;
                    currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TILE_ID] = spriteDefinitions[(DRIFTWOOD_ID<<SPRITE_DEF_SHIFT)+SPRITE_DEF_POSITION_TILE_ID];
                    reset_player_stamina();

                    sfx_play(SFX_KEY, SFX_CHANNEL_3);

                    // Mark the sprite as collected, so we can't get it again.
                    currentMapSpritePersistance[playerOverworldPosition] |= bitToByte[lastPlayerSpriteCollisionId];
                }



                break;

            case SPRITE_TYPE_DRIFTWOOD:
            case SPRITE_TYPE_DRIFTWOOD_NPC:
                isRecovery = 1;
                if (playerStamina < playerMaxStamina) {
                    ++playerStamina;
                } else {
                    playerStamina = playerMaxStamina;
                }

                if (lastPlayerSpriteCollisionId != lastTurnLastPlayerSpriteCollisionId) {
                    sfx_play(SFX_REFILL, SFX_CHANNEL_2);
                }

                // HACK: Just let tile collision handle this bit.
                /*

                // Calculate position...
                tempSpriteCollisionX = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X + 1]) << 8));
                tempSpriteCollisionY = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y + 1]) << 8));

                // Are we colliding?
                // NOTE: We take a bit of a shortcut here and assume all doors are 16x16 (the hard-coded 16 value below)
                if (
                    playerXPosition < tempSpriteCollisionX + (16 << PLAYER_POSITION_SHIFT) &&
                    playerXPosition + PLAYER_WIDTH_EXTENDED > tempSpriteCollisionX &&
                    playerYPosition < tempSpriteCollisionY + (16 << PLAYER_POSITION_SHIFT) &&
                    playerYPosition + PLAYER_HEIGHT_EXTENDED > tempSpriteCollisionY
                ) {
                    playerXPosition -= playerXVelocity;
                    playerYPosition -= playerYVelocity;
                    playerControlsLockTime = 0;
                }
*/
                if (!isStorming && playerOverworldPosition == 41) {
                    isStorming = 1;
                    sfx_play(SFX_BOOM, SFX_CHANNEL_1);
                    
                    delay(2);
                    pal_bright(5);
                    delay(2);
                    pal_bright(6);
                    delay(2);
                    pal_bright(7);
                    delay(4);
                    pal_bright(6);
                    delay(2);
                    pal_bright(5);
                    delay(2);
                    pal_bright(4);

                    trigger_game_text(stormBrew);

                    controllerState |= PAD_A;
                    lastControllerState |= PAD_A;

                    music_play(SONG_SADERWORLD);
                }

                if (spriteType == SPRITE_TYPE_DRIFTWOOD_NPC && controllerState & PAD_A && !(lastControllerState & PAD_A)) {
                    // Show the text for the player on the first screen
                    trigger_game_text(get_wood_text());
                }



                break;
            case SPRITE_TYPE_WAVE:
                if (playerInvulnerabilityTime) {
                    return;
                }

                playerStamina -= (gameDifficulty == GAME_DIFFICULTY_PEACEFUL ? 4 : 8);
                playerInvulnerabilityTime = PLAYER_DAMAGE_INVULNERABILITY_TIME;

                if (playerStamina == 0 || playerStamina > 240) {
                    gameState = GAME_STATE_GAME_OVER;
                    music_stop();
                    sfx_play(SFX_GAMEOVER, SFX_CHANNEL_1);
                    return;
                }


                break;
        }

    }
}


void reset_player_stamina(void) {
    playerMaxStamina = (PLAYER_START_MAX_STAMINA + (playerlifePreserverCount<<3));

    if (gameDifficulty == GAME_DIFFICULTY_NORMAL) {
        playerMaxStamina += 16;
    } else if (gameDifficulty == GAME_DIFFICULTY_PEACEFUL) {
        // TODO: Slow down reduction time for hard, leave this alone.
        playerMaxStamina += 32;
    }
    playerStamina = playerMaxStamina;
}
