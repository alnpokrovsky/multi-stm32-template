#if defined(OCM3_USB_LIB)

#include "pinboard.h"
#include "digitalpin.h"
#include "usb_keyboard.h"


typedef uint8_t Rowmap[COLS_N];

static const DIGITALPIN_NAME rows[ROWS_N] = {
    /*PB_0, PB_1,*/ PB_3, PB_11, PB_12, PB_13, PB_14, PB_15, PC_8, PC_9, PC_10, PC_11,
};

static const DIGITALPIN_NAME cols[COLS_N] = {
    PC_0, PC_1,     PC_2, PC_3,/*PA_0,  PA_1,*/PA_2,  PA_3,  PA_4, PA_5,/*PA_6, PA_7*/
};

/* 5 col */
#define ALT_UP PA_0
/* 6 col */
#define ALT PA_1
/* 2 row */
#define CTRL_UP PB_1
/* 1 row */
#define CTRL PB_0

static const Rowmap keymap[ROWS_N] = {
    {      0, KEY_BACKSP,  'q',   KEY_DEL,   KEY_PAUSE,             0,           0, RIGHT_ARROW, },
    {      0,     't',     'h',  COMB_INS,           0,             0,    UP_ARROW,  DOWN_ARROW, },
    {      0,     's',     'd', COMB_COPY,  KEY_SCROLL,             0,     KEY_END,  LEFT_ARROW, },
    {      0,     'm',     'f',       ' ',           0,             0,           0,           0, },
    {      0,     'g',     'z',       ' ', KEY_PAGE_UP, KEY_PAGE_DOWN,   KEY_ENTER,   KEY_ENTER, },
    {      0,     'n',     'y',       'r',         '9',           '6',         '3',         '.', },
    {      0,     'o',     'x',       'p',         '8',           '5',         '2',         '0', },
    {      0, KEY_ESC, KEY_TAB,         0,         '7',           '4',         '1',         '-', },
    { KEY_F1,  KEY_F2,  KEY_F3,    KEY_F4,      KEY_F5,        KEY_F6,      KEY_F7,      KEY_F8, },
    { KEY_F9, KEY_F10,       0,         0,     KEY_F11,             0,     KEY_F12,    KEY_HOME, },
};

static const Rowmap altkeymap[ROWS_N] = {
    {      0, KEY_BACKSP,  ')',   KEY_DEL,   KEY_PAUSE,             0,           0, COMB_SEL_RIGHT, },
    {      0,     'k',     '(',  COMB_INS,           0,             0, COMB_SEL_UP,  COMB_SEL_DOWN, },
    {      0,     'j',     'e', COMB_COPY,  KEY_SCROLL,             0,     KEY_END,  COMB_SEL_LEFT, },
    {      0,     'i',     'l',       ' ',           0,             0,           0,           0, },
    {      0,     'c',     'w',       ' ', KEY_PAGE_UP, KEY_PAGE_DOWN,   KEY_ENTER,   KEY_ENTER, },
    {      0,     'b',     'v',       ';',         '&',           '@',         '=',         '/', },
    {      0,     'a',     'u',       '!',         '$',             0,         '#',         '*', },
    {      0, KEY_ESC, KEY_TAB,         0,         '%',           ':',         ',',         '+', },
    { KEY_F1,  KEY_F2,  KEY_F3,    KEY_F4,      KEY_F5,        KEY_F6,      KEY_F7,      KEY_F8, },
    { KEY_F9, KEY_F10,       0,         0,     KEY_F11,             0,     KEY_F12,    KEY_HOME, },
};

static int rowCnt = 0;

static void nextRow(void) {
    digitalpin_set(rows[rowCnt], 0);
    if (++rowCnt >= ROWS_N) rowCnt = 0;
    digitalpin_set(rows[rowCnt], 1);
}

void pinboard_init() {
    int i;
    /* set columns to pulldown input mode */
    for (i = 0; i < COLS_N; i++) {
        digitalpin_mode(cols[i], DIGITALPIN_INPUT);
    }
    /* alt key special */
    digitalpin_mode(ALT, DIGITALPIN_INPUT);
    /* ctrl key special */
    digitalpin_mode(CTRL, DIGITALPIN_INPUT);


    /* set rows to output mode */
    for (i = 0; i < ROWS_N; i++) {
        digitalpin_mode(rows[i], DIGITALPIN_OUTPUT);
    }
    /* power to alt */
    digitalpin_mode(ALT_UP, DIGITALPIN_OUTPUT);
    digitalpin_set(ALT_UP, 1);
    /* power to ctrl */
    digitalpin_mode(CTRL_UP, DIGITALPIN_OUTPUT);
    digitalpin_set(CTRL_UP, 1);

}


uint8_t pinboard_next() {
    int i;
    const Rowmap * km = digitalpin_get(ALT) ? altkeymap : keymap;
    nextRow();
    for (i = 0; i < COLS_N; i++) {
        if (digitalpin_get(cols[i])) {
            return km[rowCnt][i];
        }
    }
    return 0;
}

uint8_t pinboard_modifiers() {
    return digitalpin_get(CTRL) ? KEY_CTRL : 0;
}

#endif