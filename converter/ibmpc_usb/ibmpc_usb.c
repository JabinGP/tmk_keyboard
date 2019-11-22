/*
Copyright 2019 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <stdbool.h>
#include "action.h"
#include "print.h"
#include "util.h"
#include "debug.h"
#include "ibmpc.h"
#include "host.h"
#include "led.h"
#include "matrix.h"
#include "timer.h"


static void matrix_make(uint8_t code);
static void matrix_break(uint8_t code);

int8_t process_cs2(void);


/*
 * Matrix Array usage:
 * 'Scan Code Set 2' is assigned into 256(32x8)cell matrix.
 * Hmm, it is very sparse and not efficient :(
 *
 * Notes:
 * Both 'Hanguel/English'(F1) and 'Hanja'(F2) collide with 'Delete'(E0 71) and 'Down'(E0 72).
 * These two Korean keys need exceptional handling and are not supported for now. Sorry.
 *
 *    8bit wide
 *   +---------+
 *  0|         |
 *  :|   XX    | 00-7F for normal codes(without E0-prefix)
 *  f|_________|
 * 10|         |
 *  :|  E0 YY  | 80-FF for E0-prefixed codes
 * 1f|         |     (<YY>|0x80) is used as matrix position.
 *   +---------+
 *
 * Exceptions:
 * 0x83:    F7(0x83) This is a normal code but beyond  0x7F.
 * 0xFC:    PrintScreen
 * 0xFE:    Pause
 */
static uint8_t matrix[MATRIX_ROWS];
#define ROW(code)      (code>>3)
#define COL(code)      (code&0x07)

static int16_t read_wait(uint16_t wait_ms)
{
    uint16_t start = timer_read();
    int16_t code;
    while ((code = ibmpc_host_recv()) == -1 && timer_elapsed(start) < wait_ms);
    return code;
}

static uint16_t read_keyboard_id(void)
{
    uint16_t id = 0;
    int16_t  code = 0;

    // Disable
    code = ibmpc_host_send(0xF5);

    // Read ID
    code = ibmpc_host_send(0xF2);
    if (code == -1)  return 0xFFFF;     // XT or No keyboard
    if (code != 0xFA) return 0xFFFE;    // Broken PS/2?
    
    code = read_wait(1000);
    if (code == -1)  return 0x0000;     // AT
    id = (code & 0xFF)<<8;

    code = read_wait(1000);
    id |= code & 0xFF;

    // Enable
    code = ibmpc_host_send(0xF4);

    return id;
}

void matrix_init(void)
{
    debug_enable = true;
    ibmpc_host_init();

    // hard reset for XT keyboard
    IBMPC_RESET();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) matrix[i] = 0x00;

    return;
}

/*
 * keyboard recognition
 *
 * 1. Send F2 to get keyboard ID
 *      a. no ACK(FA): XT keyobard
 *      b. ACK but no ID: 84-key AT keyboard CodeSet2
 *      c. ID is AB 83: PS/2 keyboard CodeSet2
 *      d. ID is BF BF: Terminal keyboard CodeSet3
 *      e. error on recv: maybe broken PS/2
 */
static enum { NONE, PC_XT, PC_AT, PC_TERMINAL, OTHER } keyboard_kind = NONE;
static uint16_t keyboard_id = 0x0000;
uint8_t matrix_scan(void)
{
    // scan code reading states
    static enum {
        INIT,
        WAIT_STARTUP,
        READ_ID,
        LED_SET,
        LOOP,
        END
    } state = INIT;
    static uint16_t last_time;

 
    if (ibmpc_error) {
        xprintf("err: %02X\n", ibmpc_error);

        // when recv error, neither send error nor buffer full
        if (!(ibmpc_error & (IBMPC_ERR_SEND | IBMPC_ERR_FULL))) {
            // keyboard init again
            if (state == LOOP) {
                xprintf("init\n");
                state = INIT;
            }
        }

        // clear or process error
        ibmpc_error = IBMPC_ERR_NONE;
    }

    int16_t code;
    switch (state) {
        case INIT:
            ibmpc_protocol = IBMPC_PROTOCOL_AT;
            keyboard_kind = NONE;
            keyboard_id = 0x0000;
            last_time = timer_read();
            state = WAIT_STARTUP;
            matrix_clear();
            break;
        case WAIT_STARTUP:
            // read and ignore BAT code and other codes when power-up
            code = ibmpc_host_recv();
            if (timer_elapsed(last_time) > 1000) {
                state = READ_ID;
            }
            break;
        case READ_ID:
            keyboard_id = read_keyboard_id();
            if (ibmpc_error) {
                xprintf("err: %02X\n", ibmpc_error);
                ibmpc_error = IBMPC_ERR_NONE;
            }
            xprintf("ID: %04X\n", keyboard_id);
            if (0xAB00 == (keyboard_id & 0xFF00)) {
                // CodeSet2 PS/2
                keyboard_kind = PC_AT;
            } else if (0xBF00 == (keyboard_id & 0xFF00)) {
                // CodeSet3 Terminal
                keyboard_kind = PC_TERMINAL;
            } else if (0x0000 == keyboard_id) {
                // CodeSet2 AT
                keyboard_kind = PC_AT;
            } else if (0xFFFF == keyboard_id) {
                // CodeSet1 XT
                keyboard_kind = PC_XT;
            } else if (0xFFFE == keyboard_id) {
                // CodeSet2 PS/2 fails to response?
                keyboard_kind = PC_AT;
            } else if (0x00FF == keyboard_id) {
                // Mouse is not supported
                xprintf("Mouse: not supported\n");
                keyboard_kind = NONE;
            } else {
                keyboard_kind = PC_AT;
            }

            // protocol
            if (keyboard_kind == PC_XT) {
                xprintf("kbd: XT\n");
                ibmpc_protocol = IBMPC_PROTOCOL_XT;
            } else if (keyboard_kind == PC_AT) {
                xprintf("kbd: AT\n");
                ibmpc_protocol = IBMPC_PROTOCOL_AT;
            } else if (keyboard_kind == PC_TERMINAL) {
                xprintf("kbd: Terminal\n");
                ibmpc_protocol = IBMPC_PROTOCOL_AT;
            } else {
                xprintf("kbd: Unknown\n");
                ibmpc_protocol = IBMPC_PROTOCOL_AT;
            }
            state = LED_SET;
            break;
        case LED_SET:
            led_set(host_keyboard_leds());
            state = LOOP;
        case LOOP:
            switch (keyboard_kind) {
                case PC_AT:
                    process_cs2();
                    break;
                case PC_XT:
                    break;
                case PC_TERMINAL:
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return 1;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & (1<<col));
}

inline
uint8_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop(matrix[i]);
    }
    return count;
}


inline
static void matrix_make(uint8_t code)
{
    if (!matrix_is_on(ROW(code), COL(code))) {
        matrix[ROW(code)] |= 1<<COL(code);
    }
}

inline
static void matrix_break(uint8_t code)
{
    if (matrix_is_on(ROW(code), COL(code))) {
        matrix[ROW(code)] &= ~(1<<COL(code));
    }
}

void matrix_clear(void)
{
    for (uint8_t i=0; i < MATRIX_ROWS; i++) matrix[i] = 0x00;
}

void led_set(uint8_t usb_led)
{
    if (keyboard_kind != PC_AT) return;

    uint8_t ibmpc_led = 0;
    if (usb_led &  (1<<USB_LED_SCROLL_LOCK))
        ibmpc_led |= (1<<IBMPC_LED_SCROLL_LOCK);
    if (usb_led &  (1<<USB_LED_NUM_LOCK))
        ibmpc_led |= (1<<IBMPC_LED_NUM_LOCK);
    if (usb_led &  (1<<USB_LED_CAPS_LOCK))
        ibmpc_led |= (1<<IBMPC_LED_CAPS_LOCK);
    ibmpc_host_set_led(ibmpc_led);
}


/*
 * PS/2 Scan Code Set 2: Exceptional Handling
 *
 * There are several keys to be handled exceptionally.
 * The scan code for these keys are varied or prefix/postfix'd
 * depending on modifier key state.
 *
 * Keyboard Scan Code Specification:
 *     http://www.microsoft.com/whdc/archive/scancode.mspx
 *     http://download.microsoft.com/download/1/6/1/161ba512-40e2-4cc9-843a-923143f3456c/scancode.doc
 *
 *
 * 1) Insert, Delete, Home, End, PageUp, PageDown, Up, Down, Right, Left
 *     a) when Num Lock is off
 *     modifiers | make                      | break
 *     ----------+---------------------------+----------------------
 *     Ohter     |                    <make> | <break>
 *     LShift    | E0 F0 12           <make> | <break>  E0 12
 *     RShift    | E0 F0 59           <make> | <break>  E0 59
 *     L+RShift  | E0 F0 12  E0 F0 59 <make> | <break>  E0 59 E0 12
 *
 *     b) when Num Lock is on
 *     modifiers | make                      | break
 *     ----------+---------------------------+----------------------
 *     Other     | E0 12              <make> | <break>  E0 F0 12
 *     Shift'd   |                    <make> | <break>
 *
 *     Handling: These prefix/postfix codes are ignored.
 *
 *
 * 2) Keypad /
 *     modifiers | make                      | break
 *     ----------+---------------------------+----------------------
 *     Ohter     |                    <make> | <break>
 *     LShift    | E0 F0 12           <make> | <break>  E0 12
 *     RShift    | E0 F0 59           <make> | <break>  E0 59
 *     L+RShift  | E0 F0 12  E0 F0 59 <make> | <break>  E0 59 E0 12
 *
 *     Handling: These prefix/postfix codes are ignored.
 *
 *
 * 3) PrintScreen
 *     modifiers | make         | break
 *     ----------+--------------+-----------------------------------
 *     Other     | E0 12  E0 7C | E0 F0 7C  E0 F0 12
 *     Shift'd   |        E0 7C | E0 F0 7C
 *     Control'd |        E0 7C | E0 F0 7C
 *     Alt'd     |           84 | F0 84
 *
 *     Handling: These prefix/postfix codes are ignored, and both scan codes
 *               'E0 7C' and 84 are seen as PrintScreen.
 *
 * 4) Pause
 *     modifiers | make(no break code)
 *     ----------+--------------------------------------------------
 *     Other     | E1 14 77 E1 F0 14 F0 77
 *     Control'd | E0 7E E0 F0 7E
 *
 *     Handling: Both code sequences are treated as a whole.
 *               And we need a ad hoc 'pseudo break code' hack to get the key off
 *               because it has no break code.
 *
 * PS/2 Resources
 * --------------
 * [1] The PS/2 Mouse/Keyboard Protocol
 * http://www.computer-engineering.org/ps2protocol/
 * Concise and thorough primer of PS/2 protocol.
 *
 * [2] Keyboard and Auxiliary Device Controller
 * http://www.mcamafia.de/pdf/ibm_hitrc07.pdf
 * Signal Timing and Format
 *
 * [3] Keyboards(101- and 102-key)
 * http://www.mcamafia.de/pdf/ibm_hitrc11.pdf
 * Keyboard Layout, Scan Code Set, POR, and Commands.
 *
 * [4] PS/2 Reference Manuals
 * http://www.mcamafia.de/pdf/ibm_hitrc07.pdf
 * Collection of IBM Personal System/2 documents.
 *
 * [5] TrackPoint Engineering Specifications for version 3E
 * https://web.archive.org/web/20100526161812/http://wwwcssrv.almaden.ibm.com/trackpoint/download.html
 */
// matrix positions for exceptional keys
#define F7             (0x83)
#define PRINT_SCREEN   (0xFC)
#define PAUSE          (0xFE)
int8_t process_cs2(void)
{
    // scan code reading states
    static enum {
        INIT,
        F0,
        E0,
        E0_F0,
        // Pause
        E1,
        E1_14,
        E1_14_77,
        E1_14_77_E1,
        E1_14_77_E1_F0,
        E1_14_77_E1_F0_14,
        E1_14_77_E1_F0_14_F0,
        // Control'd Pause
        E0_7E,
        E0_7E_E0,
        E0_7E_E0_F0,
    } state = INIT;

    // NOTE: devide Pause into E1_14_77(make) and E1_F0_14_F0_77(break)?
    // 'pseudo break code' hack
    if (matrix_is_on(ROW(PAUSE), COL(PAUSE))) {
        matrix_break(PAUSE);
    }

    uint16_t code = ibmpc_host_recv();
    if (code == -1) {
        return 0;
    }

    switch (state) {
        case INIT:
            switch (code) {
                case 0xE0:
                    state = E0;
                    break;
                case 0xF0:
                    state = F0;
                    break;
                case 0xE1:
                    state = E1;
                    break;
                case 0x83:  // F7
                    matrix_make(F7);
                    state = INIT;
                    break;
                case 0x84:  // Alt'd PrintScreen
                    matrix_make(PRINT_SCREEN);
                    state = INIT;
                    break;
                case 0x00:  // Overrun [3]p.26
                    matrix_clear();
                    clear_keyboard();
                    xprintf("!CS2_OVERRUN!\n");
                    state = INIT;
                    break;
                case 0xAA:  // Self-test passed
                case 0xFC:  // Self-test failed
                    // reset or plugin-in new keyboard
                    state = INIT;
                    return -1;
                    break;
                default:    // normal key make
                    if (code < 0x80) {
                        matrix_make(code);
                    } else {
                        matrix_clear();
                        clear_keyboard();
                        xprintf("!CS2_INIT!\n");
                    }
                    state = INIT;
            }
            break;
        case E0:    // E0-Prefixed
            switch (code) {
                case 0x12:  // to be ignored
                case 0x59:  // to be ignored
                    state = INIT;
                    break;
                case 0x7E:  // Control'd Pause
                    state = E0_7E;
                    break;
                case 0xF0:
                    state = E0_F0;
                    break;
                default:
                    if (code < 0x80) {
                        matrix_make(code|0x80);
                    } else {
                        matrix_clear();
                        clear_keyboard();
                        xprintf("!CS2_E0!\n");
                    }
                    state = INIT;
            }
            break;
        case F0:    // Break code
            switch (code) {
                case 0x83:  // F7
                    matrix_break(F7);
                    state = INIT;
                    break;
                case 0x84:  // Alt'd PrintScreen
                    matrix_break(PRINT_SCREEN);
                    state = INIT;
                    break;
                default:
                    if (code < 0x80) {
                        matrix_break(code);
                    } else {
                        matrix_clear();
                        clear_keyboard();
                        xprintf("!CS2_F0!\n");
                    }
                    state = INIT;
            }
            break;
        case E0_F0: // Break code of E0-prefixed
            switch (code) {
                case 0x12:  // to be ignored
                case 0x59:  // to be ignored
                    state = INIT;
                    break;
                default:
                    if (code < 0x80) {
                        matrix_break(code|0x80);
                    } else {
                        matrix_clear();
                        clear_keyboard();
                        xprintf("!CS2_E0_F0!\n");
                    }
                    state = INIT;
            }
            break;
        // following are states of Pause
        case E1:
            switch (code) {
                case 0x14:
                    state = E1_14;
                    break;
                default:
                    state = INIT;
            }
            break;
        case E1_14:
            switch (code) {
                case 0x77:
                    state = E1_14_77;
                    break;
                default:
                    state = INIT;
            }
            break;
        case E1_14_77:
            switch (code) {
                case 0xE1:
                    state = E1_14_77_E1;
                    break;
                default:
                    state = INIT;
            }
            break;
        case E1_14_77_E1:
            switch (code) {
                case 0xF0:
                    state = E1_14_77_E1_F0;
                    break;
                default:
                    state = INIT;
            }
            break;
        case E1_14_77_E1_F0:
            switch (code) {
                case 0x14:
                    state = E1_14_77_E1_F0_14;
                    break;
                default:
                    state = INIT;
            }
            break;
        case E1_14_77_E1_F0_14:
            switch (code) {
                case 0xF0:
                    state = E1_14_77_E1_F0_14_F0;
                    break;
                default:
                    state = INIT;
            }
            break;
        case E1_14_77_E1_F0_14_F0:
            switch (code) {
                case 0x77:
                    matrix_make(PAUSE);
                    state = INIT;
                    break;
                default:
                    state = INIT;
            }
            break;
        // Following are states of Control'd Pause
        case E0_7E:
            if (code == 0xE0)
                state = E0_7E_E0;
            else
                state = INIT;
            break;
        case E0_7E_E0:
            if (code == 0xF0)
                state = E0_7E_E0_F0;
            else
                state = INIT;
            break;
        case E0_7E_E0_F0:
            if (code == 0x7E)
                matrix_make(PAUSE);
            state = INIT;
            break;
        default:
            state = INIT;
    }
    return 0;
}