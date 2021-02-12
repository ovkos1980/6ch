#ifndef _HMI_H
#define _HMI_H

// return data first byte
#define HMI_TOUCH_EVENT       0x65
#define HMI_CURR_PAGE_ID      0x66
#define HMI_TOUCH_COORDS      0x67
#define HMI_TOUCH_EVENT_SLEEP 0x68
#define HMI_STRING_VAR_DATA   0x70
#define HMI_NUMERIC_VAR_DATA  0x71
#define HMI_AUTO_SLEEP        0x86
#define HMI_AUTO_WAKEUP       0x87
#define HMI_START_SUCCESS     0x88
#define HMI_SD_DETECT         0x89
#define HMI_VAR_NAME_INVALID  0x1A
#define HMI_UNKNOWN           0xFF

// HMI components ID
#define HMI_BT1_ID            1
#define HMI_BT2_ID            2
#define HMI_BT3_ID            3
#define HMI_BT4_ID            4
#define HMI_BT5_ID            5
#define HMI_BT6_ID            6
#define HMI_P_MODE_ID         15
#define HMI_P_TIME_SET_ID     16
#define HMI_P_SETTINGS_ID     17
#define HMI_P3_EXIT_ID        16

#define HMI_PROG1_BTN_ID      23
#define HMI_PROG2_BTN_ID      24
#define HMI_PROG3_BTN_ID      25
#define HMI_PROG4_BTN_ID      26
#define HMI_PROG5_BTN_ID      27
#define HMI_PROG6_BTN_ID      28

#define HMI_P5_EXIT_ID        7

#define HMI_HYST1_ID          9
#define HMI_HYST2_ID          10
#define HMI_HYST3_ID          11
#define HMI_HYST4_ID          12
#define HMI_HYST5_ID          13
#define HMI_HYST6_ID          14

#define HMI_LAMP1_ID          7
#define HMI_LAMP2_ID          8
#define HMI_LAMP3_ID          9
#define HMI_LAMP4_ID          10
#define HMI_LAMP5_ID          11
#define HMI_LAMP6_ID          12

#define HMI_TIME_EXIT_ID      3
#define HMI_TIME_ENTER_ID     15

#define HMI_RETURN_SIGN_T     'T'
#define HMI_RETURN_SIGN_0     '0'
#define HMI_RETURN_SIGN_1     '1'
#define HMI_RETURN_SIGN_2     '2'
#define HMI_RETURN_SIGN_3     '3'
#define HMI_RETURN_SIGN_4     '4'
#define HMI_RETURN_SIGN_5     '5'
#define HMI_RETURN_SIGN_6     '6'
#define HMI_RETURN_SIGN_7     '7'
#define HMI_RETURN_SIGN_8     '8'
#define HMI_RETURN_SIGN_NULL  0


#endif //_HMI_H