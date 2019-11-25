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
#ifndef UNIMAP_TRANS_H
#define UNIMAP_TRANS_H

#include <stdint.h>
#include <avr/pgmspace.h>
#include "unimap.h"
#include "action.h"
#include "ibmpc_usb.h"


#define UNIMAP_PS2( \
            K68,K69,K6A,K6B,K6C,K6D,K6E,K6F,K70,K71,K72,K73,                                     \
    K29,    K3A,K3B,K3C,K3D,K3E,K3F,K40,K41,K42,K43,K44,K45,      K46,K47,K48,      K01,K02,K03, \
    K35,K1E,K1F,K20,K21,K22,K23,K24,K25,K26,K27,K2D,K2E,K74,K2A,  K49,K4A,K4B,  K53,K54,K55,K56, \
    K2B,K14,K1A,K08,K15,K17,K1C,K18,K0C,K12,K13,K2F,K30,    K31,  K4C,K4D,K4E,  K5F,K60,K61,K57, \
    K39,K04,K16,K07,K09,K0A,K0B,K0D,K0E,K0F,K33,K34,        K28,                K5C,K5D,K5E,K66, \
    K79,K64,K1D,K1B,K06,K19,K05,K11,K10,K36,K37,K38,    K75,K7D,      K52,      K59,K5A,K5B,K58, \
    K78,K7B,K7A,K77,        K2C,        K76,K00,K7E,K7F,K65,K7C,  K50,K51,K4F,  K32,K62,K63,K67  \
) UNIMAP ( \
            K68,K69,K6A,K6B,K6C,K6D,K6E,K6F,K70,K71,K72,K73,                                     \
    K29,    K3A,K3B,K3C,K3D,K3E,K3F,K40,K41,K42,K43,K44,K45,      K46,K47,K48,      K01,K02,K03, \
    K35,K1E,K1F,K20,K21,K22,K23,K24,K25,K26,K27,K2D,K2E,K74,K2A,  K49,K4A,K4B,  K53,K54,K55,K56, \
    K2B,K14,K1A,K08,K15,K17,K1C,K18,K0C,K12,K13,K2F,K30,    K31,  K4C,K4D,K4E,  K5F,K60,K61,K57, \
    K39,K04,K16,K07,K09,K0A,K0B,K0D,K0E,K0F,K33,K34,    K32,K28,                K5C,K5D,K5E,K66, \
    K79,K64,K1D,K1B,K06,K19,K05,K11,K10,K36,K37,K38,    K75,K7D,      K52,      K59,K5A,K5B,K58, \
    K78,K7B,K7A,K77,        K2C,        K76,K00,K7E,K7F,K65,K7C,  K50,K51,K4F,      K62,K63,K67  \
)

/* Mapping to Universal keyboard layout
 *
 * Universal keyboard layout
 *         ,-----------------------------------------------.
 *         |F13|F14|F15|F16|F17|F18|F19|F20|F21|F22|F23|F24|
 * ,---.   |-----------------------------------------------|     ,-----------.     ,-----------.
 * |Esc|   |F1 |F2 |F3 |F4 |F5 |F6 |F7 |F8 |F9 |F10|F11|F12|     |PrS|ScL|Pau|     |VDn|VUp|Mut|
 * `---'   `-----------------------------------------------'     `-----------'     `-----------'
 * ,-----------------------------------------------------------. ,-----------. ,---------------.
 * |  `|  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|JPY|Bsp| |Ins|Hom|PgU| |NmL|  /|  *|  -|
 * |-----------------------------------------------------------| |-----------| |---------------|
 * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|  \  | |Del|End|PgD| |  7|  8|  9|  +|
 * |-----------------------------------------------------------| `-----------' |---------------|
 * |CapsL |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '| ^a|Retn|               |  4|  5|  6|KP,|
 * |-----------------------------------------------------------|     ,---.     |---------------|
 * |Shft|  <|  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /| RO|Shift |     |Up |     |  1|  2|  3|KP=|
 * |-----------------------------------------------------------| ,-----------. |---------------|
 * |Ctl|Gui|Alt|MHEN|     Space      |HENK|KANA|Alt|Gui|App|Ctl| |Lef|Dow|Rig| |  #|  0|  .|Ent|
 * `-----------------------------------------------------------' `-----------' `---------------'
 */


/*
 * Scan Code Set 1:
 * ,-------.  ,--------------------------------------------------------------------------.
 * | F1| F2|  |Esc|  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|  BS  |NumLck |ScrLck |
 * |-------|  |--------------------------------------------------------------------------|
 * | F3| F4|  | Tab |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ] |   |  7|  8|  9|  -|
 * |-------|  |------------------------------------------------------|Ent|---------------|
 * | F5| F6|  | Ctrl |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|  `|   |  4|  5|  6|   |
 * |-------|  |----------------------------------------------------------------------|   |
 * | F7| F8|  |Shif|  \|  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /|Shift|  *|  1|  2|  3|  +|
 * |-------|  |----------------------------------------------------------------------|   |
 * | F9|F10|  |  Alt  |               Space                  |CapsLck|   0   |   .   |   |
 * `-------'  `--------------------------------------------------------------------------'
 *
 * ,-------.  ,--------------------------------------------------------------------------.
 * | 3B| 3C|  | 01| 02| 03| 04| 05| 06| 07| 08| 09| 0A| 0B| 0C| 0D|  0E  |  45   |  46   |
 * |-------|  |--------------------------------------------------------------------------|
 * | 3D| 3E|  | 0F  | 10| 11| 12| 13| 14| 15| 16| 17| 18| 19| 1A| 1B |   | 47| 48| 49| 4A|
 * |-------|  |------------------------------------------------------| 1C|---------------|
 * | 3F| 40|  | 1D   | 1E| 1F| 20| 21| 22| 23| 24| 25| 26| 27| 28| 29|   | 4B| 4C| 4D|   |
 * |-------|  |----------------------------------------------------------------------|   |
 * | 41| 42|  | 2A | 2B| 2C| 2D| 2E| 2F| 30| 31| 32| 33| 34| 35|  36 | 37| 4F| 50| 51| 4E|
 * |-------|  |----------------------------------------------------------------------|   |
 * | 43| 44|  |  38   |              39                      |  3A   |  52   |  53   |   |
 * `-------'  `--------------------------------------------------------------------------'
 */
const uint8_t PROGMEM unimap_cs1[MATRIX_ROWS][MATRIX_COLS] = {
    { UNIMAP_NO,    UNIMAP_ESC,   UNIMAP_1,     UNIMAP_2,     UNIMAP_3,     UNIMAP_4,     UNIMAP_5,     UNIMAP_6     }, /* 00-07 */
    { UNIMAP_7,     UNIMAP_8,     UNIMAP_9,     UNIMAP_0,     UNIMAP_MINUS, UNIMAP_EQUAL, UNIMAP_BSPACE,UNIMAP_TAB   }, /* 08-0F */
    { UNIMAP_Q,     UNIMAP_W,     UNIMAP_E,     UNIMAP_R,     UNIMAP_T,     UNIMAP_Y,     UNIMAP_U,     UNIMAP_I     }, /* 10-17 */
    { UNIMAP_O,     UNIMAP_P,     UNIMAP_LBRC,  UNIMAP_RBRC,  UNIMAP_ENTER, UNIMAP_LCTL,  UNIMAP_A,     UNIMAP_S,    }, /* 18-1F */
    { UNIMAP_D,     UNIMAP_F,     UNIMAP_G,     UNIMAP_H,     UNIMAP_J,     UNIMAP_K,     UNIMAP_L,     UNIMAP_SCLN  }, /* 20-27 */
    { UNIMAP_QUOTE, UNIMAP_GRAVE, UNIMAP_LSHIFT,UNIMAP_BSLASH,UNIMAP_Z,     UNIMAP_X,     UNIMAP_C,     UNIMAP_V,    }, /* 28-2F */
    { UNIMAP_B,     UNIMAP_N,     UNIMAP_M,     UNIMAP_COMMA, UNIMAP_DOT,   UNIMAP_SLASH, UNIMAP_RSHIFT,UNIMAP_PAST  }, /* 30-37 */
    { UNIMAP_LALT,  UNIMAP_SPACE, UNIMAP_CAPS,  UNIMAP_F1,    UNIMAP_F2,    UNIMAP_F3,    UNIMAP_F4,    UNIMAP_F5    }, /* 38-3F */
    { UNIMAP_F6,    UNIMAP_F7,    UNIMAP_F8,    UNIMAP_F9,    UNIMAP_F10,   UNIMAP_NLCK,  UNIMAP_SLCK,  UNIMAP_P7    }, /* 40-47 */
    { UNIMAP_P8,    UNIMAP_P9,    UNIMAP_PMNS,  UNIMAP_P4,    UNIMAP_P5,    UNIMAP_P6,    UNIMAP_PPLS,  UNIMAP_P1    }, /* 48-4F */
    { UNIMAP_P2,    UNIMAP_P3,    UNIMAP_P0,    UNIMAP_PDOT,  UNIMAP_PSCR,  UNIMAP_PAUSE, UNIMAP_NUHS,  UNIMAP_F11   }, /* 50-57 */
    { UNIMAP_F12,   UNIMAP_PEQL,  UNIMAP_LGUI,  UNIMAP_RGUI,  UNIMAP_APP,   UNIMAP_MUTE,  UNIMAP_VOLD,  UNIMAP_VOLU  }, /* 58-5F */
    { UNIMAP_UP,    UNIMAP_LEFT,  UNIMAP_DOWN,  UNIMAP_RIGHT, UNIMAP_F13,   UNIMAP_F14,   UNIMAP_F15,   UNIMAP_F16   }, /* 60-67 */
    { UNIMAP_F17,   UNIMAP_F18,   UNIMAP_F19,   UNIMAP_F20,   UNIMAP_F21,   UNIMAP_F22,   UNIMAP_F23,   UNIMAP_PENT  }, /* 68-6F */
    { UNIMAP_KANA,  UNIMAP_INSERT,UNIMAP_DELETE,UNIMAP_RO,    UNIMAP_HOME,  UNIMAP_END,   UNIMAP_F24,   UNIMAP_PGUP  }, /* 70-77 */
    { UNIMAP_PGDN,  UNIMAP_HENK,  UNIMAP_RCTL,  UNIMAP_MHEN,  UNIMAP_RALT,  UNIMAP_JYEN,  UNIMAP_PCMM,  UNIMAP_PSLS  }, /* 78-7F */
};

/*
 * Scan Code Set 2:
 *         ,-----------------------------------------------.
 *         |F13|F14|F15|F16|F17|F18|F19|F20|F21|F22|F23|F24|
 * ,---.   |-----------------------------------------------|     ,-----------.     ,-----------.
 * |Esc|   |F1 |F2 |F3 |F4 |F5 |F6 |F7 |F8 |F9 |F10|F11|F12|     |PrS|ScL|Pau|     |VDn|VUp|Mut|
 * `---'   `-----------------------------------------------'     `-----------'     `-----------'
 * ,-----------------------------------------------------------. ,-----------. ,---------------.
 * |  `|  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|JPY|Bsp| |Ins|Hom|PgU| |NmL|  /|  *|  -|
 * |-----------------------------------------------------------| |-----------| |---------------|
 * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|  \  | |Del|End|PgD| |  7|  8|  9|  +|
 * |-----------------------------------------------------------| `-----------' |---------------|
 * |CapsL |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '| ^a|Entr|               |  4|  5|  6|KP,|
 * |-----------------------------------------------------------|     ,---.     |---------------|
 * |Shft|  <|  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /| RO|Shift |     |Up |     |  1|  2|  3|Ent|
 * |-----------------------------------------------------------| ,-----------. |---------------|
 * |Ctl|Gui|Alt|MHEN|     Space      |HENK|KANA|Alt|Gui|App|Ctl| |Lef|Dow|Rig| |  #|  0|  .|KP=|
 * `-----------------------------------------------------------' `-----------' `---------------'
 *
 *         ,-----------------------------------------------.
 *         | 08| 10| 18| 20| 28| 30| 38| 40| 48| 50| 57| 5F|
 * ,---.   |-----------------------------------------------|     ,-----------.     ,-----------.
 * | 76|   | 05| 06| 04| 0C| 03| 0B| 83| 0A| 01| 09| 78| 07|     | FC| 7E| FE|     | A1| B2| A3|
 * `---'   `-----------------------------------------------'     `-----------'     `-----------'
 * ,-----------------------------------------------------------. ,-----------. ,---------------.
 * | 0E| 16| 1E| 26| 25| 2E| 36| 3D| 3E| 46| 45| 4E| 55| 6A| 66| | F0| EC| FD| | 77| CA| 7C| 7B|
 * |-----------------------------------------------------------| |-----------| |---------------|
 * | 0D  | 15| 1D| 24| 2D| 2C| 35| 3C| 43| 44| 4D| 54| 5B|  5D | | F1| E9| FA| | 6C| 75| 7D| 79|
 * |-----------------------------------------------------------| `-----------' |---------------|
 * | 58   | 1C| 1B| 23| 2B| 34| 33| 3B| 42| 4B| 4C| 52| ^a| 5A |               | 6B| 73| 74| 6D|
 * |-----------------------------------------------------------|     ,---.     |---------------|
 * | 12 | 61| 1A| 22| 21| 2A| 32| 31| 3A| 41| 49| 4A| 51|  59  |     | F5|     | 69| 72| 7A| DA|
 * |-----------------------------------------------------------| ,-----------. |---------------|
 * | 14| 9F| 11| 67 |     29         | 64 | 13 | 91| A7| AF| 94| | EB| F2| F4| | 68|70 | 71| 63|
 * `-----------------------------------------------------------' `-----------' `---------------'
 * ^a ISO hash key uses identical scancode 5D to US backslash.
 * 51, 63, 68, 6D: hidden keys in IBM model M
 */
const uint8_t PROGMEM unimap_cs2[MATRIX_ROWS][MATRIX_COLS] = {
    { UNIMAP_NO,    UNIMAP_F9,    UNIMAP_NO,    UNIMAP_F5,    UNIMAP_F3,    UNIMAP_F1,    UNIMAP_F2,    UNIMAP_F12   }, /* 00-07 */
    { UNIMAP_F13,   UNIMAP_F10,   UNIMAP_F8,    UNIMAP_F6,    UNIMAP_F4,    UNIMAP_TAB,   UNIMAP_GRV,   UNIMAP_NO    }, /* 08-0F */
    { UNIMAP_F14,   UNIMAP_LALT,  UNIMAP_LSHIFT,UNIMAP_KANA,  UNIMAP_LCTL,  UNIMAP_Q,     UNIMAP_1,     UNIMAP_NO    }, /* 10-17 */
    { UNIMAP_F15,   UNIMAP_NO,    UNIMAP_Z,     UNIMAP_S,     UNIMAP_A,     UNIMAP_W,     UNIMAP_2,     UNIMAP_NO    }, /* 18-1F */
    { UNIMAP_F16,   UNIMAP_C,     UNIMAP_X,     UNIMAP_D,     UNIMAP_E,     UNIMAP_4,     UNIMAP_3,     UNIMAP_NO    }, /* 20-27 */
    { UNIMAP_F17,   UNIMAP_SPACE, UNIMAP_V,     UNIMAP_F,     UNIMAP_T,     UNIMAP_R,     UNIMAP_5,     UNIMAP_NO    }, /* 28-2F */
    { UNIMAP_F18,   UNIMAP_N,     UNIMAP_B,     UNIMAP_H,     UNIMAP_G,     UNIMAP_Y,     UNIMAP_6,     UNIMAP_NO    }, /* 30-37 */
    { UNIMAP_F19,   UNIMAP_NO,    UNIMAP_M,     UNIMAP_J,     UNIMAP_U,     UNIMAP_7,     UNIMAP_8,     UNIMAP_NO    }, /* 38-3F */
    { UNIMAP_F20,   UNIMAP_COMMA, UNIMAP_K,     UNIMAP_I,     UNIMAP_O,     UNIMAP_0,     UNIMAP_9,     UNIMAP_NO    }, /* 40-47 */
    { UNIMAP_F21,   UNIMAP_DOT,   UNIMAP_SLASH, UNIMAP_L,     UNIMAP_SCOLON,UNIMAP_P,     UNIMAP_MINUS, UNIMAP_NO    }, /* 48-4F */
    { UNIMAP_F22,   UNIMAP_RO,    UNIMAP_QUOTE, UNIMAP_NO,    UNIMAP_LBRC,  UNIMAP_EQUAL, UNIMAP_NO,    UNIMAP_F23   }, /* 50-57 */
    { UNIMAP_CAPS,  UNIMAP_RSHIFT,UNIMAP_ENTER, UNIMAP_RBRC,  UNIMAP_NO,    UNIMAP_BSLASH,UNIMAP_NO,    UNIMAP_F24   }, /* 58-5F */
    { UNIMAP_NO,    UNIMAP_NUBS,  UNIMAP_NO,    UNIMAP_PEQL,  UNIMAP_HENK,  UNIMAP_NO,    UNIMAP_BSPACE,UNIMAP_MHEN  }, /* 60-67 */
    { UNIMAP_NUHS,  UNIMAP_P1,    UNIMAP_JYEN,  UNIMAP_P4,    UNIMAP_P7,    UNIMAP_PCMM,  UNIMAP_NO,    UNIMAP_NO    }, /* 68-6F */
    { UNIMAP_P0,    UNIMAP_PDOT,  UNIMAP_P2,    UNIMAP_P5,    UNIMAP_P6,    UNIMAP_P8,    UNIMAP_ESC,   UNIMAP_NLCK  }, /* 70-77 */
    { UNIMAP_F11,   UNIMAP_PPLS,  UNIMAP_P3,    UNIMAP_PMNS,  UNIMAP_PAST,  UNIMAP_P9,    UNIMAP_SLCK,  UNIMAP_NO    }, /* 78-7F */
    { UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_F7,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO    }, /* 80-87 */
    { UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO    }, /* 88-8F */
    { UNIMAP_NO,    UNIMAP_RALT,  UNIMAP_NO,    UNIMAP_NO,    UNIMAP_RCTRL, UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO    }, /* 90-97 */
    { UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_LGUI  }, /* 98-9F */
    { UNIMAP_NO,    UNIMAP_VOLD,  UNIMAP_NO,    UNIMAP_MUTE,  UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_RGUI  }, /* A0-A7 */
    { UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_APP   }, /* A8-AF */
    { UNIMAP_NO,    UNIMAP_NO,    UNIMAP_VOLU,  UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO    }, /* B0-B7 */
    { UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO    }, /* B8-BF */
    { UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO    }, /* C0-C7 */
    { UNIMAP_NO,    UNIMAP_NO,    UNIMAP_PSLS,  UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO    }, /* C8-CF */
    { UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO    }, /* D0-D7 */
    { UNIMAP_NO,    UNIMAP_NO,    UNIMAP_PENT,  UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO    }, /* D8-DF */
    { UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO    }, /* E0-E7 */
    { UNIMAP_NO,    UNIMAP_END,   UNIMAP_NO,    UNIMAP_LEFT,  UNIMAP_HOME,  UNIMAP_NO,    UNIMAP_NO,    UNIMAP_NO    }, /* E8-EF */
    { UNIMAP_INS,   UNIMAP_DEL,   UNIMAP_DOWN,  UNIMAP_NO,    UNIMAP_RIGHT, UNIMAP_UP,    UNIMAP_NO,    UNIMAP_NO    }, /* F0-F7 */
    { UNIMAP_NO,    UNIMAP_NO,    UNIMAP_PGDOWN,UNIMAP_NO,    UNIMAP_PSCR,  UNIMAP_PGUP,  UNIMAP_PAUSE, UNIMAP_NO    }, /* F8-FF */
};

/*
 * Scan Code Set 3:
 *               ,-----------------------------------------------.
 *               |F13|F14|F15|F16|F17|F18|F19|F20|F21|F22|F23|F24|
 *               |-----------------------------------------------|
 *               |F1 |F2 |F3 |F4 |F5 |F6 |F7 |F8 |F9 |F10|F11|F12|
 *               `-----------------------------------------------'
 * ,-------. ,-----------------------------------------------------------. ,-----------. ,---------------.
 * |PrS|Esc| |  `|  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|Yen| BS| |Ins|Hom|PgU| |NmL|  /|  *|  -|
 * |-------| |-----------------------------------------------------------| |-----------| |---------------|
 * |ScL|Hen| |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|    \| |Del|End|PgD| |  7|  8|  9|  +|
 * |-------| |-----------------------------------------------------------| `-----------' |-----------|---|
 * |Pau|Muh| |CapsLo|  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|  #| Ret|     |Up |     |  4|  5|  6|  ,|
 * |-------| |-----------------------------------------------------------| ,-----------. |---------------|
 * |VoD|VoU| |Shif|  \|  Z|  X|  C|  V|  B|  N|  M|  ,|  ,|  /| RO| Shift| |Lef|App|Rig| |  1|  2|  3|Ent|
 * |-------| |-----------------------------------------------------------| `-----------' |-----------|---|
 * |Gui|Gui| |Ctrl|    |Alt |          Space              |Alt |    |Ctrl|     |Dow|     |Kan|  0|  .|  =|
 * `-------' `----'    `---------------------------------------'    `----'     `---'     `---------------'
 *
 *               ,-----------------------------------------------.
 *               | 08| 10| 18| 20| 28| 30| 38| 40| 48| 50| 57| 5F|
 *               |-----------------------------------------------|
 *               | 07| 0F| 17| 1F| 27| 2F| 37| 3F| 47| 4F| 56| 5E|
 *               `-----------------------------------------------'
 * ,-------. ,-----------------------------------------------------------. ,-----------. ,---------------.
 * | 05| 06| | 0E| 16| 1E| 26| 25| 2E| 36| 3D| 3E| 46| 45| 4E| 55| 5D| 66| | 67| 6E| 6F| | 76| 77| 7E|*84|
 * |-------| |-----------------------------------------------------------| |-----------| |---------------|
 * | 04| 0C| | 0D  | 15| 1D| 24| 2D| 2C| 35| 3C| 43| 44| 4D| 54| 5B|  5C | | 64| 65| 6D| | 6C| 75| 7D| 7C|
 * |-------| |-----------------------------------------------------------| `-----------' |---------------|
 * | 03| 0B| | 58   | 1C| 1B| 23| 2B| 34| 33| 3B| 42| 4B| 4C| 52| 53| 5A |     | 63|     | 6B| 73| 74| 7B|
 * |-------| |-----------------------------------------------------------| ,-----------. |---------------|
 * |*83| 0A| | 12 | 13| 1A| 22| 21| 2A| 32| 31| 3A| 41| 49| 4A| 51|  59  | | 61| 62| 6A| | 69| 72| 7A| 79|
 * |-------| |-----------------------------------------------------------| `-----------' |---------------|
 * | 01| 09| | 11  |   |19  |        29                   |39  |   | 58  |     | 60|     | 68| 70| 71| 78|
 * `-------' `-----'   `---------------------------------------'   `-----'     `---'     `---------------'
 * *83=02
 * *84=7F
 * 51, 5C, 5D, 68, 78: hidden keys in IBM 122-key terminal keyboard
 */
const uint8_t PROGMEM unimap_cs3[MATRIX_ROWS][MATRIX_COLS] = {
    { UNIMAP_NO,    UNIMAP_LGUI,  UNIMAP_VOLD,  UNIMAP_PAUSE, UNIMAP_SLCK,  UNIMAP_PSCR,  UNIMAP_ESC,   UNIMAP_F1    }, /* 00-07 */
    { UNIMAP_F13,   UNIMAP_RGUI,  UNIMAP_VOLU,  UNIMAP_MHEN,  UNIMAP_HENK,  UNIMAP_TAB,   UNIMAP_GRV,   UNIMAP_F2    }, /* 08-0F */
    { UNIMAP_F14,   UNIMAP_LCTL,  UNIMAP_LSHIFT,UNIMAP_NUBS,  UNIMAP_CAPS,  UNIMAP_Q,     UNIMAP_1,     UNIMAP_F3    }, /* 10-17 */
    { UNIMAP_F15,   UNIMAP_LALT,  UNIMAP_Z,     UNIMAP_S,     UNIMAP_A,     UNIMAP_W,     UNIMAP_2,     UNIMAP_F4    }, /* 18-1F */
    { UNIMAP_F16,   UNIMAP_C,     UNIMAP_X,     UNIMAP_D,     UNIMAP_E,     UNIMAP_4,     UNIMAP_3,     UNIMAP_F5    }, /* 20-27 */
    { UNIMAP_F17,   UNIMAP_SPACE, UNIMAP_V,     UNIMAP_F,     UNIMAP_T,     UNIMAP_R,     UNIMAP_5,     UNIMAP_F6    }, /* 28-2F */
    { UNIMAP_F18,   UNIMAP_N,     UNIMAP_B,     UNIMAP_H,     UNIMAP_G,     UNIMAP_Y,     UNIMAP_6,     UNIMAP_F7    }, /* 30-37 */
    { UNIMAP_F19,   UNIMAP_RALT,  UNIMAP_M,     UNIMAP_J,     UNIMAP_U,     UNIMAP_7,     UNIMAP_8,     UNIMAP_F8    }, /* 38-3F */
    { UNIMAP_F20,   UNIMAP_COMMA, UNIMAP_K,     UNIMAP_I,     UNIMAP_O,     UNIMAP_0,     UNIMAP_9,     UNIMAP_F9    }, /* 40-47 */
    { UNIMAP_F21,   UNIMAP_DOT,   UNIMAP_SLASH, UNIMAP_L,     UNIMAP_SCOLON,UNIMAP_P,     UNIMAP_MINUS, UNIMAP_F10   }, /* 48-4F */
    { UNIMAP_F22,   UNIMAP_RO,    UNIMAP_QUOTE, UNIMAP_NUHS,  UNIMAP_LBRC,  UNIMAP_EQUAL, UNIMAP_F11,   UNIMAP_F23   }, /* 50-57 */
    { UNIMAP_RCTL,  UNIMAP_RSHIFT,UNIMAP_ENTER, UNIMAP_RBRC,  UNIMAP_BSLASH,UNIMAP_JYEN,  UNIMAP_F12,   UNIMAP_F24   }, /* 58-5F */
    { UNIMAP_DOWN,  UNIMAP_LEFT,  UNIMAP_APP,   UNIMAP_UP,    UNIMAP_DEL,   UNIMAP_END,   UNIMAP_BSPACE,UNIMAP_INS   }, /* 60-67 */
    { UNIMAP_KANA,  UNIMAP_P1,    UNIMAP_RIGHT, UNIMAP_P4,    UNIMAP_P7,    UNIMAP_PGDN,  UNIMAP_HOME,  UNIMAP_PGUP  }, /* 68-6F */
    { UNIMAP_P0,    UNIMAP_PDOT,  UNIMAP_P2,    UNIMAP_P5,    UNIMAP_P6,    UNIMAP_P8,    UNIMAP_NLCK,  UNIMAP_PSLS  }, /* 70-77 */
    { UNIMAP_PEQL,  UNIMAP_PENT,  UNIMAP_P3,    UNIMAP_PCMM,  UNIMAP_PPLS,  UNIMAP_P9,    UNIMAP_PAST,  UNIMAP_PMNS  }, /* 78-7F */
};


extern const action_t actionmaps[][UNIMAP_ROWS][UNIMAP_COLS];
action_t action_for_key(uint8_t layer, keypos_t key)
{
    uint8_t unimap_pos;
    switch (keyboard_kind) {
        case PC_XT:
            unimap_pos = pgm_read_byte(&unimap_cs1[key.row][key.col]);
            break;
        case PC_AT:
            unimap_pos = pgm_read_byte(&unimap_cs2[key.row][key.col]);
            break;
        case PC_TERMINAL:
            unimap_pos = pgm_read_byte(&unimap_cs3[key.row][key.col]);
            break;
        default:
            return (action_t)ACTION_NO;
    }

    if (unimap_pos == UNIMAP_NO) return (action_t)ACTION_NO;

    return (action_t)pgm_read_word(&actionmaps[(layer)][(unimap_pos & 0x70) >> 4][(unimap_pos & 0x0f)]);
}
#endif
