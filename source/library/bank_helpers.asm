.feature c_comments
.linecont +

.define NO_CHR_BANK 255

.segment "ZEROPAGE"
    nmiChrTileBank: .res 1
    .exportzp nmiChrTileBank

.segment "CODE"

.export _set_prg_bank, _get_prg_bank, _set_chr_bank_0, _set_chr_bank_1
.export _set_nmi_chr_tile_bank, _unset_nmi_chr_tile_bank
.export _set_mirroring

; UNROM quirk.. you have to write the value to a location that stores that same value. So, lets set up a small LUT
.segment "RODATA"
bank_lookup: 
    .byte 0, 1, 2, 3, 4, 5, 6

.segment "CODE"

_set_prg_bank:
    tax
    ; Grab old bank and store it temporarily in a. We don't touch it again, so we'll return it!
    lda BP_BANK
    pha
    ; Store new bank into BP_BANK
    stx BP_BANK

    lda bank_lookup, x
    sta bank_lookup, x 

    pla

    rts

_get_prg_bank:
    lda BP_BANK
    rts

_set_chr_bank_0:
    ; FIXME
    ; mmc1_register_write MMC1_CHR0
    rts

_set_chr_bank_1:
    ; FIXME
    ; mmc1_register_write MMC1_CHR1
    rts

; Both of these just set/unset a varible used in `neslib.asm` to trigger this during nmi.
_set_nmi_chr_tile_bank: 
    sta nmiChrTileBank
    rts

_unset_nmi_chr_tile_bank:
    lda #NO_CHR_BANK
    sta nmiChrTileBank
    rts

_set_mirroring:
    ; FIXME: Remove all use
    ; Limit this to mirroring bits, so we can add our bytes safely.
    ; and #%00000011
    ; Now, set this to have 4k chr banking, and not mess up which prg bank is which.
    ; ora #%00011100
    ; Bombs away!
    ; mmc1_register_write MMC1_CTRL
    rts