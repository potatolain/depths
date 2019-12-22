#define PRG_BANK_MENU_INPUT_HELPERS 2

// Helper method to wait for the player to press start.
void wait_for_start(void);

// Same as above, but kick it back to start in KIOSK mode after a set time.
void wait_for_start_and_boot(void);