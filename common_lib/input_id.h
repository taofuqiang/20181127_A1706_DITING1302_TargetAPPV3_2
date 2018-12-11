/***************************************************************************************************
*                    (c) Copyright 1992-2010 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          input_id.h
*\Description   按键输入相关的定义
*\Note
*\Log           2010.03.10    Ver 1.0    张波
*               创建文件。
***************************************************************************************************/
#ifndef _INPUT_ID_H
#define _INPUT_ID_H

/***************************************************************************************************
*\Enum  		InputID
*\Description   键盘输入ID
***************************************************************************************************/
typedef enum
{
    INPUT_NULL          =  0x00,            /*!<INPUT_NULL*/
    INPUT_LBUTTON       =  0x01,            /*!<INPUT_LBUTTON*/
    INPUT_RBUTTON       =  0x02,            /*!<INPUT_RBUTTON*/
    INPUT_CANCEL        =  0x03,            /*!<INPUT_CANCEL*/
    INPUT_MBUTTON       =  0x04,            /*!<INPUT_MBUTTON*/
    INPUT_XBUTTON1      =  0x05,            /*!<INPUT_XBUTTON1*/
    INPUT_XBUTTON2      =  0x06,            /*!<INPUT_XBUTTON2*/
    INPUT_BACK          =  0x08,            /*!<INPUT_BACK*/
    INPUT_TAB           =  0x09,            /*!<INPUT_TAB*/
    INPUT_CLEAR         =  0x0C,            /*!<INPUT_CLEAR*/
    INPUT_ENTER         =  0x0D,            /*!<INPUT_ENTER*/
    INPUT_SHIFT         =  0x10,            /*!<INPUT_SHIFT*/
    INPUT_CONTROL       =  0x11,            /*!<INPUT_CONTROL*/
    INPUT_MENU          =  0x12,            /*!<INPUT_MENU*/
    INPUT_PAUSE         =  0x13,            /*!<INPUT_PAUSE*/
    INPUT_CAPITAL       =  0x14,            /*!<INPUT_CAPITAL*/
    INPUT_KANA          =  0x15,            /*!<INPUT_KANA*/
    INPUT_HANGEUL       =  0x15,            /*!<INPUT_HANGEUL*/
    INPUT_HANGUL        =  0x15,            /*!<INPUT_HANGUL*/
    INPUT_JUNJA         =  0x17,            /*!<INPUT_JUNJA*/
    INPUT_FINAL         =  0x18,            /*!<INPUT_FINAL*/
    INPUT_HANJA         =  0x19,            /*!<INPUT_HANJA*/
    INPUT_KANJI         =  0x19,            /*!<INPUT_KANJI*/
    INPUT_ESC           =  0x1B,            /*!<INPUT_ESC*/
    INPUT_CONVERT       =  0x1C,            /*!<INPUT_CONVERT*/
    INPUT_NONCONVERT    =  0x1D,            /*!<INPUT_NONCONVERT*/
    INPUT_ACCEPT        =  0x1E,            /*!<INPUT_ACCEPT*/
    INPUT_MODECHANGE    =  0x1F,            /*!<INPUT_MODECHANGE*/
    INPUT_SPACE         =  0x20,            /*!<INPUT_SPACE*/
    INPUT_PRIOR         =  0x21,            /*!<INPUT_PRIOR*/
    INPUT_NEXT          =  0x22,            /*!<INPUT_NEXT*/
    INPUT_END           =  0x23,            /*!<INPUT_END*/
    INPUT_HOME          =  0x24,            /*!<INPUT_HOME*/
    INPUT_LEFT          =  0x25,            /*!<INPUT_LEFT*/
    INPUT_UP            =  0x26,            /*!<INPUT_UP*/
    INPUT_RIGHT         =  0x27,            /*!<INPUT_RIGHT*/
    INPUT_DOWN          =  0x28,            /*!<INPUT_DOWN*/
    INPUT_SELECT        =  0x29,            /*!<INPUT_SELECT*/
    INPUT_PRINT         =  0x2A,            /*!<INPUT_PRINT*/
    INPUT_EXECUTE       =  0x2B,            /*!<INPUT_EXECUTE*/
    INPUT_SNAPSHOT      =  0x2C,            /*!<INPUT_SNAPSHOT*/
    INPUT_INSERT        =  0x2D,            /*!<INPUT_INSERT*/
    INPUT_DELETE        =  0x2E,            /*!<INPUT_DELETE*/
    INPUT_HELP          =  0x2F,            /*!<INPUT_HELP*/
    INPUT_0             =  0x30,            /*!<INPUT_0*/
    INPUT_1             =  0x31,            /*!<INPUT_1*/
    INPUT_2             =  0x32,            /*!<INPUT_2*/
    INPUT_3             =  0x33,            /*!<INPUT_3*/
    INPUT_4             =  0x34,            /*!<INPUT_4*/
    INPUT_5             =  0x35,            /*!<INPUT_5*/
    INPUT_6             =  0x36,            /*!<INPUT_6*/
    INPUT_7             =  0x37,            /*!<INPUT_7*/
    INPUT_8             =  0x38,            /*!<INPUT_8*/
    INPUT_9             =  0x39,            /*!<INPUT_9*/
    INPUT_A             =  0x41,            /*!<INPUT_A*/
    INPUT_B             =  0x42,            /*!<INPUT_B*/
    INPUT_C             =  0x43,            /*!<INPUT_C*/
    INPUT_D             =  0x44,            /*!<INPUT_D*/
    INPUT_E             =  0x45,            /*!<INPUT_E*/
    INPUT_F             =  0x46,            /*!<INPUT_F*/
    INPUT_G             =  0x47,            /*!<INPUT_G*/
    INPUT_H             =  0x48,            /*!<INPUT_H*/
    INPUT_I             =  0x49,            /*!<INPUT_I*/
    INPUT_J             =  0x4A,            /*!<INPUT_J*/
    INPUT_K             =  0x4B,            /*!<INPUT_K*/
    INPUT_L             =  0x4C,            /*!<INPUT_L*/
    INPUT_M             =  0x4D,            /*!<INPUT_M*/
    INPUT_N             =  0x4E,            /*!<INPUT_N*/
    INPUT_O             =  0x4F,            /*!<INPUT_O*/
    INPUT_P             =  0x50,            /*!<INPUT_P*/
    INPUT_Q             =  0x51,            /*!<INPUT_Q*/
    INPUT_R             =  0x52,            /*!<INPUT_R*/
    INPUT_S             =  0x53,            /*!<INPUT_S*/
    INPUT_T             =  0x54,            /*!<INPUT_T*/
    INPUT_U             =  0x55,            /*!<INPUT_U*/
    INPUT_V             =  0x56,            /*!<INPUT_V*/
    INPUT_W             =  0x57,            /*!<INPUT_W*/
    INPUT_X             =  0x58,            /*!<INPUT_X*/
    INPUT_Y             =  0x59,            /*!<INPUT_Y*/
    INPUT_Z             =  0x5A,            /*!<INPUT_Z*/
                                              
    /*                                        
    * INPUT_0 - INPUT_9                       
    * same as ASCII '0' - '9' (0x30 - 0x39)   
    *                           
    * 0x40 : unassigned         
    * INPUT_A - INPUT_Z         
    * same as ASCII 'A' - 'Z' (0x41 - 0x5A)
   */                                       
                                           
    INPUT_LWIN          =  0x5B,            /*!<INPUT_LWIN*/
    INPUT_RWIN          =  0x5C,            /*!<INPUT_RWIN*/
    INPUT_APPS          =  0x5D,            /*!<INPUT_APPS*/
    INPUT_SLEEP         =  0x5F,            /*!<INPUT_SLEEP*/
    INPUT_NUMPAD0       =  0x60,            /*!<INPUT_NUMPAD0*/
    INPUT_NUMPAD1       =  0x61,            /*!<INPUT_NUMPAD1*/
    INPUT_NUMPAD2       =  0x62,            /*!<INPUT_NUMPAD2*/
    INPUT_NUMPAD3       =  0x63,            /*!<INPUT_NUMPAD3*/
    INPUT_NUMPAD4       =  0x64,            /*!<INPUT_NUMPAD4*/
    INPUT_NUMPAD5       =  0x65,            /*!<INPUT_NUMPAD5*/
    INPUT_NUMPAD6       =  0x66,            /*!<INPUT_NUMPAD6*/
    INPUT_NUMPAD7       =  0x67,            /*!<INPUT_NUMPAD7*/
    INPUT_NUMPAD8       =  0x68,            /*!<INPUT_NUMPAD8*/
    INPUT_NUMPAD9       =  0x69,            /*!<INPUT_NUMPAD9*/
    INPUT_MULTIPLY      =  0x6A,            /*!<INPUT_MULTIPLY*/
    INPUT_ADD           =  0x6B,            /*!<INPUT_ADD*/
    INPUT_SEPARATOR     =  0x6C,            /*!<INPUT_SEPARATOR*/
    INPUT_SUBTRACT      =  0x6D,            /*!<INPUT_SUBTRACT*/
    INPUT_DECIMAL       =  0x6E,            /*!<INPUT_DECIMAL*/
    INPUT_DIVIDE        =  0x6F,            /*!<INPUT_DIVIDE*/
    INPUT_F1            =  0x70,            /*!<INPUT_F1*/
    INPUT_F2            =  0x71,            /*!<INPUT_F2*/
    INPUT_F3            =  0x72,            /*!<INPUT_F3*/
    INPUT_F4            =  0x73,            /*!<INPUT_F4*/
    INPUT_F5            =  0x74,            /*!<INPUT_F5*/
    INPUT_F6            =  0x75,            /*!<INPUT_F6*/
    INPUT_F7            =  0x76,            /*!<INPUT_F7*/
    INPUT_F8            =  0x77,            /*!<INPUT_F8*/
    INPUT_F9            =  0x78,            /*!<INPUT_F9*/
    INPUT_F10           =  0x79,            /*!<INPUT_F10*/
    INPUT_F11           =  0x7A,            /*!<INPUT_F11*/
    INPUT_F12           =  0x7B,            /*!<INPUT_F12*/
    INPUT_F13           =  0x7C,            /*!<INPUT_F13*/
    INPUT_F14           =  0x7D,            /*!<INPUT_F14*/
    INPUT_F15           =  0x7E,            /*!<INPUT_F15*/
    INPUT_F16           =  0x7F,            /*!<INPUT_F16*/
    INPUT_F17           =  0x80,            /*!<INPUT_F17*/
    INPUT_F18           =  0x81,            /*!<INPUT_F18*/
    INPUT_F19           =  0x82,            /*!<INPUT_F19*/
    INPUT_F20           =  0x83,            /*!<INPUT_F20*/
    INPUT_F21           =  0x84,            /*!<INPUT_F21*/
    INPUT_F22           =  0x85,            /*!<INPUT_F22*/
    INPUT_F23           =  0x86,            /*!<INPUT_F23*/
    INPUT_F24           =  0x87,            /*!<INPUT_F24*/
    INPUT_NUMLOCK       =  0x90,            /*!<INPUT_NUMLOCK*/
    INPUT_SCROLL        =  0x91,            /*!<INPUT_SCROLL*/
    INPUT_OEM_NEC_EQUAL = 0x92,             /*!<INPUT_OEM_NEC_EQUAL*/
    INPUT_OEM_FJ_JISHO  =  0x92,            /*!<'Dictionary'key*/
    INPUT_OEM_FJ_MASSHOU = 0x93,            /*!<'Unregisterword'key*/
    INPUT_OEM_FJ_TOUROKU = 0x94,            /*!<'Registerword'key*/
    INPUT_OEM_FJ_LOYA   =  0x95,            /*!<'LeftOYAYUBI'key*/
    INPUT_OEM_FJ_ROYA   =  0x96,            /*!<'RightOYAYUBI'key*/
    INPUT_LSHIFT        =  0xA0,            /*!<INPUT_LSHIFT*/
    INPUT_RSHIFT        =  0xA1,            /*!<INPUT_RSHIFT*/
    INPUT_LCONTROL      =  0xA2,            /*!<INPUT_LCONTROL*/
    INPUT_RCONTROL      =  0xA3,            /*!<INPUT_RCONTROL*/
    INPUT_LMENU         =  0xA4,            /*!<INPUT_LMENU*/
    INPUT_RMENU         =  0xA5,            /*!<INPUT_RMENU*/
    INPUT_BROWSER_BACK  =  0xA6,            /*!<INPUT_BROWSER_BACK*/
    INPUT_BROWSER_FORWARD  =  0xA7,         /*!<INPUT_BROWSER_FORWARD*/
    INPUT_BROWSER_REFRESH  =  0xA8,         /*!<INPUT_BROWSER_REFRESH*/
    INPUT_BROWSER_STOP     =  0xA9,         /*!<INPUT_BROWSER_STOP*/
    INPUT_BROWSER_SEARCH   =  0xAA,         /*!<INPUT_BROWSER_SEARCH*/
    INPUT_BROWSER_FAVORITES=  0xAB,         /*!<INPUT_BROWSER_FAVORITES*/
    INPUT_BROWSER_HOME     =  0xAC,         /*!<INPUT_BROWSER_HOME*/
    INPUT_VOLUME_MUTE      =  0xAD,         /*!<INPUT_VOLUME_MUTE*/
    INPUT_VOLUME_DOWN      =  0xAE,         /*!<INPUT_VOLUME_DOWN*/
    INPUT_VOLUME_UP        =  0xAF,         /*!<INPUT_VOLUME_UP*/
    INPUT_MEDIA_NEXT_TRACK =  0xB0,         /*!<INPUT_MEDIA_NEXT_TRACK*/
    INPUT_MEDIA_PREV_TRACK =  0xB1,         /*!<INPUT_MEDIA_PREV_TRACK*/
    INPUT_MEDIA_STOP       =  0xB2,         /*!<INPUT_MEDIA_STOP*/
    INPUT_MEDIA_PLAY_PAUSE =  0xB3,         /*!<INPUT_MEDIA_PLAY_PAUSE*/
    INPUT_LAUNCH_MAIL      =  0xB4,         /*!<INPUT_LAUNCH_MEDIA_SELECT*/
    INPUT_LAUNCH_MEDIA_SELECT = 0xB5,       /*!<INPUT_LAUNCH_MEDIA_SELECT*/
    INPUT_LAUNCH_APP1      =  0xB6,         /*!<INPUT_LAUNCH_APP1*/
    INPUT_LAUNCH_APP2      =  0xB7,         /*!<INPUT_LAUNCH_APP2*/
    INPUT_OEM_1       =  0xBA,              /*!<';:'forUS*/
    INPUT_OEM_PLUS    =  0xBB,              /*!<'+'anycountry*/
    INPUT_OEM_COMMA   =  0xBC,              /*!<','anycountry*/
    INPUT_OEM_MINUS   =  0xBD,              /*!<'-'anycountry*/
    INPUT_OEM_PERIOD  =  0xBE,              /*!<'.'anycountry*/
    INPUT_OEM_2       =  0xBF,              /*!<'/?'forUS*/
    INPUT_OEM_3       =  0xC0,              /*!<'`~'forUS*/
    INPUT_OEM_4       =  0xDB,              /*!<'[{'forUS*/
    INPUT_OEM_5       =  0xDC,              /*!<'\|'forUS*/
    INPUT_OEM_6       =  0xDD,              /*!<']}'forUS*/
    INPUT_OEM_7       =  0xDE,              /*!<''"'forUS*/
    INPUT_OEM_8       =  0xDF,              /*!<INPUT_OEM_8*/
    INPUT_OEM_AX      =  0xE1,              /*!<'AX'keyonJapaneseAXkbd*/
    INPUT_OEM_102     =  0xE2,              /*!<"<>"or"\|"onRT102-keykbd.*/
    INPUT_ICO_HELP    =  0xE3,              /*!<HelpkeyonICO*/
    INPUT_ICO_00      =  0xE4,              /*!<00keyonICO*/
    INPUT_PROCESSKEY  =  0xE5,              /*!<INPUT_PROCESSKEY*/
    INPUT_ICO_CLEAR   =  0xE6,              /*!<INPUT_ICO_CLEAR*/
    INPUT_PACKET      =  0xE7,              /*!<INPUT_PACKET*/
    INPUT_OEM_RESET   =  0xE9,              /*!<INPUT_OEM_RESET*/
    INPUT_OEM_JUMP    =  0xEA,              /*!<INPUT_OEM_JUMP*/
    INPUT_OEM_PA1     =  0xEB,              /*!<INPUT_OEM_PA1*/
    INPUT_OEM_PA2     =  0xEC,              /*!<INPUT_OEM_PA2*/
    INPUT_OEM_PA3     =  0xED,              /*!<INPUT_OEM_PA3*/
    INPUT_OEM_WSCTRL  =  0xEE,              /*!<INPUT_OEM_WSCTRL*/
    INPUT_OEM_CUSEL   =  0xEF,              /*!<INPUT_OEM_CUSEL*/
    INPUT_OEM_ATTN    =  0xF0,              /*!<INPUT_OEM_ATTN*/
    INPUT_OEM_FINISH  =  0xF1,              /*!<INPUT_OEM_FINISH*/
    INPUT_OEM_COPY    =  0xF2,              /*!<INPUT_OEM_COPY*/
    INPUT_OEM_AUTO    =  0xF3,              /*!<INPUT_OEM_AUTO*/
    INPUT_OEM_ENLW    =  0xF4,              /*!<INPUT_OEM_ENLW*/
    INPUT_OEM_BACKTAB =  0xF5,              /*!<INPUT_OEM_BACKTAB*/
    INPUT_ATTN        =  0xF6,              /*!<INPUT_ATTN*/
    INPUT_CRSEL       =  0xF7,              /*!<INPUT_CRSEL*/
    INPUT_EXSEL       =  0xF8,              /*!<INPUT_EXSEL*/
    INPUT_EREOF       =  0xF9,              /*!<INPUT_EREOF*/
    INPUT_PLAY        =  0xFA,              /*!<INPUT_PLAY*/
    INPUT_ZOOM        =  0xFB,              /*!<INPUT_ZOOM*/
    INPUT_NONAME      =  0xFC,              /*!<INPUT_NONAME*/
    INPUT_PA1         =  0xFD,              /*!<INPUT_PA1*/
    INPUT_OEM_CLEAR   =  0xFE,              /*!<INPUT_OEM_CLEAR*/
    INPUT_INVALID_ID                        /*!<INPUT_INVALID_ID*/
}InputID;                                     
                                              
                                              
#endif /*_INPUT_ID_H*/
