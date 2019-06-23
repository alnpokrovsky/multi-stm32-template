#ifndef __HARDWARE_USB_HID_KEYBOARD_H__
#define __HARDWARE_USB_HID_KEYBOARD_H__

#include <stdint.h>

enum MODIFIER_KEY {
    KEY_CTRL = 1,
    KEY_SHIFT = 2,
    KEY_ALT = 4,
};

enum FUNCTION_KEY {
    COMB_SEL_RIGHT = 2,/* shift+right */
    COMB_SEL_LEFT,  /* shift+left*/
    COMB_SEL_DOWN,  /* shift+down */
    COMB_SEL_UP,    /* shift+up */
    COMB_COPY,      /* ctrl+c */
    COMB_INS,       /* ctrl+v */
    KEY_DEL,        /* delete key */
    KEY_BACKSP,     /* backspace key */
    KEY_TAB,        /* tab key */
    KEY_ENTER,      /* enter key */ 
    KEY_ESC,        /* esc key */
    
    KEY_F1 = 128,   /* F1 key */
    KEY_F2,         /* F2 key */
    KEY_F3,         /* F3 key */
    KEY_F4,         /* F4 key */
    KEY_F5,         /* F5 key */
    KEY_F6,         /* F6 key */
    KEY_F7,         /* F7 key */
    KEY_F8,         /* F8 key */
    KEY_F9,         /* F9 key */
    KEY_F10,        /* F10 key */
    KEY_F11,        /* F11 key */
    KEY_F12,        /* F12 key */

    KEY_PRINTSC,   /* Print Screen key */
    KEY_SCROLL,    /* Scroll lock */
    KEY_PAUSE,          /* Pause break */
    KEY_CAPS_LOCK,      /* caps lock */
    KEY_NUM_LOCK,       /* num lock */
    KEY_INSERT,         /* Insert key */
    KEY_HOME,           /* Home key */
    KEY_PAGE_UP,        /* Page Up key */
    KEY_END,            /* End key */
    KEY_PAGE_DOWN,      /* Page Down key */

    RIGHT_ARROW,        /* Right arrow */
    LEFT_ARROW,         /* Left arrow */
    DOWN_ARROW,         /* Down arrow */
    UP_ARROW,           /* Up arrow */
};

/**
 * init usb device like keyboard
 */
void hid_keyboard_init(void);

/**
 * press alpha char key or one of FUNCTION_KEYs
 * modifier of MODIFIER_KEY
 */
void hid_keyboard_press(uint8_t key, uint8_t modifier);
void hid_keyboard_release(void);

/**
 * move mouse relative to (x,y) pixels
 */
void hid_keyboard_move_mouse(int8_t x, int8_t y);


#endif
