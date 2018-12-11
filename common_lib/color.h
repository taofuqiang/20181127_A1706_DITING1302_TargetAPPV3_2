/***************************************************************************************************
*                    (c) Copyright 1992-2010 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          color.h
*\Description   颜色相关的定义
*\Note
*\Log           2010.03.19    Ver 1.0    张波
*               创建文件。
***************************************************************************************************/
#ifndef _COLOR_H
#define _COLOR_H

#include "type.h"

/*STD_RGB*/

/*
typedef union RGB_st
{
        u32 Value;
        struct
        {
            u8 Alpha;
            u8 Red;
            u8 Green;
            u8 Blue;
        };
}ColorRGB;
*/
typedef u32 RGBMask;
/***************************************************************************************************
*\Enum  		RGBMask
*\Description   颜色掩码的定义
***************************************************************************************************/
//typedef enum
//{
//    ALPHA_MASK  = 0xFF000000,   /*!<ALPHA_MASK*/
//    RED_MASK    = 0x00FF0000,   /*!<红色掩码*/
//    GREEN_MASK  = 0x0000FF00,   /*!<绿色掩码*/
//    BLUE_MASK   = 0x000000FF    /*!<蓝色掩码*/
//}RGBMask;

#define  RGB_BLACK       0xFF000000   /*!<黑色*/
#define  RGB_RED         0xFFFF0000   /*!<红色*/
#define  RGB_BLUE        0xFF0000FF   /*!<蓝色*/
#define  RGB_BROWN       0xFFA52A2A   /*!<棕色*/
#define  RGB_GOLD        0xFFFFD700   /*!<金色*/
#define  RGB_GRAY        0xFF808080   /*!<灰色*/
#define  RGB_GREEN       0xFF008000   /*!<绿色*/
#define  RGB_WHITE       0xFFFFFFFF   /*!<白色*/
#define  RGB_YELLOW      0xFFFFFF00   /*!<黄色*/   
#define  RGB_TRANSPARENT 0x00FFFFFF   /*!<透明*/   


typedef u32 ColorRGB;
/*颜色的定义*/
/***************************************************************************************************
*\Enum  		ColorRGB
*\Description   颜色的定义
***************************************************************************************************/
//typedef enum
//{
//    RGB_ALICEBLUE               = 0xFFF0F8FF,   /*!<RGB_ALICEBLUE*/
//    RGB_ANTIQUEWHITE            = 0xFFFAEBD7,   /*!<RGB_ANTIQUEWHITE*/
//    RGB_AQUA                    = 0xFF00FFFF,   /*!<RGB_AQUA*/
//    RGB_AQUAMARINE              = 0xFF7FFFD4,   /*!<RGB_AQUAMARINE*/
//    RGB_AZURE                   = 0xFFF0FFFF,   /*!<RGB_AZURE*/
//    RGB_BEIGE                   = 0xFFF5F5DC,   /*!<RGB_BEIGE*/
//    RGB_BISQUE                  = 0xFFFFE4C4,   /*!<RGB_BISQUE*/
//    RGB_BLACK                   = 0xFF000000,   /*!<黑色*/
//    RGB_BLANCHEDALMOND          = 0xFFFFEBCD,   /*!<RGB_BLANCHEDALMOND*/
//    RGB_BLUE                    = 0xFF0000FF,   /*!<蓝色*/
//    RGB_BLUEVIOLET              = 0xFF8A2BE2,   /*!<RGB_BLUEVIOLET*/
//    RGB_BROWN                   = 0xFFA52A2A,   /*!<棕色*/
//    RGB_BURLYWOOD               = 0xFFDEB887,   /*!<RGB_BURLYWOOD*/
//    RGB_CADETBLUE               = 0xFF5F9EA0,   /*!<RGB_CADETBLUE*/
//    RGB_CHARTREUSE              = 0xFF7FFF00,   /*!<RGB_CHARTREUSE*/
//    RGB_CHOCOLATE               = 0xFFD2691E,   /*!<RGB_CHOCOLATE*/
//    RGB_CORAL                   = 0xFFFF7F50,   /*!<RGB_CORAL*/
//    RGB_CORNFLOWERBLUE          = 0xFF6495ED,   /*!<RGB_CORNFLOWERBLUE*/
//    RGB_CORNSILK                = 0xFFFFF8DC,   /*!<RGB_CORNSILK*/
//    RGB_CRIMSON                 = 0xFFDC143C,   /*!<RGB_CRIMSON*/
//    RGB_CYAN                    = 0xFF00FFFF,   /*!<RGB_CYAN*/
//    RGB_DARKBLUE                = 0xFF00008B,   /*!<RGB_DARKBLUE*/
//    RGB_DARKCYAN                = 0xFF008B8B,   /*!<RGB_DARKCYAN*/
//    RGB_DARKGOLDENROD           = 0xFFB8860B,   /*!<RGB_DARKGOLDENROD*/
//    RGB_DARKGRAY                = 0xFFA9A9A9,   /*!<RGB_DARKGRAY*/
//    RGB_DARKGREEN               = 0xFF006400,   /*!<RGB_DARKGREEN*/
//    RGB_DARKKHAKI               = 0xFFBDB76B,   /*!<RGB_DARKKHAKI*/
//    RGB_DARKMAGENTA             = 0xFF8B008B,   /*!<RGB_DARKMAGENTA*/
//    RGB_DARKOLIVEGREEN          = 0xFF556B2F,   /*!<RGB_DARKOLIVEGREEN*/
//    RGB_DARKORANGE              = 0xFFFF8C00,   /*!<RGB_DARKORANGE*/
//    RGB_DARKORCHID              = 0xFF9932CC,   /*!<RGB_DARKORCHID*/
//    RGB_DARKRED                 = 0xFF8B0000,   /*!<RGB_DARKRED*/
//    RGB_DARKSALMON              = 0xFFE9967A,   /*!<RGB_DARKSALMON*/
//    RGB_DARKSEAGREEN            = 0xFF8FBC8B,   /*!<RGB_DARKSEAGREEN*/
//    RGB_DARKSLATEBLUE           = 0xFF483D8B,   /*!<RGB_DARKSLATEBLUE*/
//    RGB_DARKSLATEGRAY           = 0xFF2F4F4F,   /*!<RGB_DARKSLATEGRAY*/
//    RGB_DARKTURQUOISE           = 0xFF00CED1,   /*!<RGB_DARKTURQUOISE*/
//    RGB_DARKVIOLET              = 0xFF9400D3,   /*!<RGB_DARKVIOLET*/
//    RGB_DEEPPINK                = 0xFFFF1493,   /*!<RGB_DEEPPINK*/
//    RGB_DEEPSKYBLUE             = 0xFF00BFFF,   /*!<RGB_DEEPSKYBLUE*/
//    RGB_DIMGRAY                 = 0xFF696969,   /*!<RGB_DIMGRAY*/
//    RGB_DODGERBLUE              = 0xFF1E90FF,   /*!<RGB_DODGERBLUE*/
//    RGB_FIREBRICK               = 0xFFB22222,   /*!<RGB_FIREBRICK*/
//    RGB_FLORALWHITE             = 0xFFFFFAF0,   /*!<RGB_FLORALWHITE*/
//    RGB_FORESTGREEN             = 0xFF228B22,   /*!<RGB_FORESTGREEN*/
//    RGB_FUCHSIA                 = 0xFFFF00FF,   /*!<RGB_FUCHSIA*/
//    RGB_GAINSBORO               = 0xFFDCDCDC,   /*!<RGB_GAINSBORO*/
//    RGB_GHOSTWHITE              = 0xFFF8F8FF,   /*!<RGB_GHOSTWHITE*/
//    RGB_GOLD                    = 0xFFFFD700,   /*!<金色*/
//    RGB_GOLDENROD               = 0xFFDAA520,   /*!<RGB_GOLDENROD*/
//    RGB_GRAY                    = 0xFF808080,   /*!<灰色*/
//    RGB_GREEN                   = 0xFF008000,   /*!<绿色*/
//    RGB_GREENYELLOW             = 0xFFADFF2F,   /*!<RGB_GREENYELLOW*/
//    RGB_HONEYDEW                = 0xFFF0FFF0,   /*!<RGB_HONEYDEW*/
//    RGB_HOTPINK                 = 0xFFFF69B4,   /*!<RGB_HOTPINK*/
//    RGB_INDIANRED               = 0xFFCD5C5C,   /*!<RGB_INDIANRED*/
//    RGB_INDIGO                  = 0xFF4B0082,   /*!<RGB_INDIGO*/
//    RGB_IVORY                   = 0xFFFFFFF0,   /*!<RGB_IVORY*/
//    RGB_KHAKI                   = 0xFFF0E68C,   /*!<RGB_KHAKI*/
//    RGB_LAVENDER                = 0xFFE6E6FA,   /*!<RGB_LAVENDER*/
//    RGB_LAVENDERBLUSH           = 0xFFFFF0F5,   /*!<RGB_LAVENDERBLUSH*/
//    RGB_LAWNGREEN               = 0xFF7CFC00,   /*!<RGB_LAWNGREEN*/
//    RGB_LEMONCHIFFON            = 0xFFFFFACD,   /*!<RGB_LEMONCHIFFON*/
//    RGB_LIGHTBLUE               = 0xFFADD8E6,   /*!<RGB_LIGHTBLUE*/
//    RGB_LIGHTCORAL              = 0xFFF08080,   /*!<RGB_LIGHTCORAL*/
//    RGB_LIGHTCYAN               = 0xFFE0FFFF,   /*!<RGB_LIGHTCYAN*/
//    RGB_LIGHTGOLDENRODYELLOW    = 0xFFFAFAD2,   /*!<RGB_LIGHTGOLDENRODYELLOW*/
//    RGB_LIGHTGRAY               = 0xFFD3D3D3,   /*!<RGB_LIGHTGRAY*/
//    RGB_LIGHTGREEN              = 0xFF90EE90,   /*!<RGB_LIGHTGREEN*/
//    RGB_LIGHTPINK               = 0xFFFFB6C1,   /*!<RGB_LIGHTPINK*/
//    RGB_LIGHTSALMON             = 0xFFFFA07A,   /*!<RGB_LIGHTSALMON*/
//    RGB_LIGHTSEAGREEN           = 0xFF20B2AA,   /*!<RGB_LIGHTSEAGREEN*/
//    RGB_LIGHTSKYBLUE            = 0xFF87CEFA,   /*!<RGB_LIGHTSKYBLUE*/
//    RGB_LIGHTSLATEGRAY          = 0xFF778899,   /*!<RGB_LIGHTSLATEGRAY*/
//    RGB_LIGHTSTEELBLUE          = 0xFFB0C4DE,   /*!<RGB_LIGHTSTEELBLUE*/
//    RGB_LIGHTYELLOW             = 0xFFFFFFE0,   /*!<RGB_LIGHTYELLOW*/
//    RGB_LIME                    = 0xFF00FF00,   /*!<RGB_LIME*/
//    RGB_LIMEGREEN               = 0xFF32CD32,   /*!<RGB_LIMEGREEN*/
//    RGB_LINEN                   = 0xFFFAF0E6,   /*!<RGB_LINEN*/
//    RGB_MAGENTA                 = 0xFFFF00FF,   /*!<RGB_MAGENTA*/
//    RGB_MAROON                  = 0xFF800000,   /*!<RGB_MAROON*/
//    RGB_MEDIUMAQUAMARINE        = 0xFF66CDAA,   /*!<RGB_MEDIUMAQUAMARINE*/
//    RGB_MEDIUMBLUE              = 0xFF0000CD,   /*!<RGB_MEDIUMBLUE*/
//    RGB_MEDIUMORCHID            = 0xFFBA55D3,   /*!<RGB_MEDIUMORCHID*/
//    RGB_MEDIUMPURPLE            = 0xFF9370DB,   /*!<RGB_MEDIUMPURPLE*/
//    RGB_MEDIUMSEAGREEN          = 0xFF3CB371,   /*!<RGB_MEDIUMSEAGREEN*/
//    RGB_MEDIUMSLATEBLUE         = 0xFF7B68EE,   /*!<RGB_MEDIUMSLATEBLUE*/
//    RGB_MEDIUMSPRINGGREEN       = 0xFF00FA9A,   /*!<RGB_MEDIUMSPRINGGREEN*/
//    RGB_MEDIUMTURQUOISE         = 0xFF48D1CC,   /*!<RGB_MEDIUMTURQUOISE*/
//    RGB_MEDIUMVIOLETRED         = 0xFFC71585,   /*!<RGB_MEDIUMVIOLETRED*/
//    RGB_MIDNIGHTBLUE            = 0xFF191970,   /*!<RGB_MIDNIGHTBLUE*/
//    RGB_MINTCREAM               = 0xFFF5FFFA,   /*!<RGB_MINTCREAM*/
//    RGB_MISTYROSE               = 0xFFFFE4E1,   /*!<RGB_MISTYROSE*/
//    RGB_MOCCASIN                = 0xFFFFE4B5,   /*!<RGB_MOCCASIN*/
//    RGB_NAVAJOWHITE             = 0xFFFFDEAD,   /*!<RGB_NAVAJOWHITE*/
//    RGB_NAVY                    = 0xFF000080,   /*!<RGB_NAVY*/
//    RGB_OLDLACE                 = 0xFFFDF5E6,   /*!<RGB_OLDLACE*/
//    RGB_OLIVE                   = 0xFF808000,   /*!<RGB_OLIVE*/
//    RGB_OLIVEDRAB               = 0xFF6B8E23,   /*!<RGB_OLIVEDRAB*/
//    RGB_ORANGE                  = 0xFFFFA500,   /*!<RGB_ORANGE*/
//    RGB_ORANGERED               = 0xFFFF4500,   /*!<RGB_ORANGERED*/
//    RGB_ORCHID                  = 0xFFDA70D6,   /*!<RGB_ORCHID*/
//    RGB_PALEGOLDENROD           = 0xFFEEE8AA,   /*!<RGB_PALEGOLDENROD*/
//    RGB_PALEGREEN               = 0xFF98FB98,   /*!<RGB_PALEGREEN*/
//    RGB_PALETURQUOISE           = 0xFFAFEEEE,   /*!<RGB_PALETURQUOISE*/
//    RGB_PALEVIOLETRED           = 0xFFDB7093,   /*!<RGB_PALEVIOLETRED*/
//    RGB_PAPAYAWHIP              = 0xFFFFEFD5,   /*!<RGB_PAPAYAWHIP*/
//    RGB_PEACHPUFF               = 0xFFFFDAB9,   /*!<RGB_PEACHPUFF*/
//    RGB_PERU                    = 0xFFCD853F,   /*!<RGB_PERU*/
//    RGB_PINK                    = 0xFFFFC0CB,   /*!<RGB_PINK*/
//    RGB_PLUM                    = 0xFFDDA0DD,   /*!<RGB_PLUM*/
//    RGB_POWDERBLUE              = 0xFFB0E0E6,   /*!<RGB_POWDERBLUE*/
//    RGB_PURPLE                  = 0xFF800080,   /*!<RGB_PURPLE*/
//    RGB_RED                     = 0xFFFF0000,   /*!<红色*/
//    RGB_ROSYBROWN               = 0xFFBC8F8F,   /*!<RGB_ROSYBROWN*/
//    RGB_ROYALBLUE               = 0xFF4169E1,   /*!<RGB_ROYALBLUE*/
//    RGB_SADDLEBROWN             = 0xFF8B4513,   /*!<RGB_SADDLEBROWN*/
//    RGB_SALMON                  = 0xFFFA8072,   /*!<RGB_SALMON*/
//    RGB_SANDYBROWN              = 0xFFF4A460,   /*!<RGB_SANDYBROWN*/
//    RGB_SEAGREEN                = 0xFF2E8B57,   /*!<RGB_SEAGREEN*/
//    RGB_SEASHELL                = 0xFFFFF5EE,   /*!<RGB_SEASHELL*/
//    RGB_SIENNA                  = 0xFFA0522D,   /*!<RGB_SIENNA*/
//    RGB_SILVER                  = 0xFFC0C0C0,   /*!<RGB_SILVER*/
//    RGB_SKYBLUE                 = 0xFF87CEEB,   /*!<RGB_SKYBLUE*/
//    RGB_SLATEBLUE               = 0xFF6A5ACD,   /*!<RGB_SLATEBLUE*/
//    RGB_SLATEGRAY               = 0xFF708090,   /*!<RGB_SLATEGRAY*/
//    RGB_SNOW                    = 0xFFFFFAFA,   /*!<RGB_SNOW*/
//    RGB_SPRINGGREEN             = 0xFF00FF7F,   /*!<RGB_SPRINGGREEN*/
//    RGB_STEELBLUE               = 0xFF4682B4,   /*!<RGB_STEELBLUE*/
//    RGB_TAN                     = 0xFFD2B48C,   /*!<RGB_TAN*/
//    RGB_TEAL                    = 0xFF008080,   /*!<RGB_TEAL*/
//    RGB_THISTLE                 = 0xFFD8BFD8,   /*!<RGB_THISTLE*/
//    RGB_TOMATO                  = 0xFFFF6347,   /*!<RGB_TOMATO*/
//    RGB_TURQUOISE               = 0xFF40E0D0,   /*!<RGB_TURQUOISE*/
//    RGB_VIOLET                  = 0xFFEE82EE,   /*!<RGB_VIOLET*/
//    RGB_WHEAT                   = 0xFFF5DEB3,   /*!<RGB_WHEAT*/
//    RGB_WHITE                   = 0xFFFFFFFF,   /*!<白色*/
//    RGB_WHITESMOKE              = 0xFFF5F5F5,   /*!<RGB_WHITESMOKE*/   
//    RGB_YELLOW                  = 0xFFFFFF00,   /*!<黄色*/   
//    RGB_YELLOWGREEN             = 0xFF9ACD32,   /*!<RGB_YELLOWGREEN*/   
//    RGB_TRANSPARENT             = 0x00FFFFFF    /*!<透明*/   
//}ColorRGB;                                       

/***************************************************************************************************
*\Enum  		RGB565
*\Description   RGB565
***************************************************************************************************/
//typedef union RGB565_un
//{
//    struct
//    {
//        u8 Red: 5;      /*!<红色值*/
//        u8 Green: 6;    /*!<绿色值*/
//        u8 Blue: 5;     /*!<蓝色值*/
//    };
//    u16 Value;          /*!<值*/
//}RGB565;



/*YUV*/
/*
R = Y + 1.4075 *（V-128）
G = Y – 0.3455 *（U –128） – 0.7169 *（V –128）
B = Y + 1.779 *（U – 128）


u = YUVdata[UPOS] - 128;
v = YUVdata[VPOS] - 128;

rdif = v + ((v * 103) >> 8);
invgdif = ((u * 88) >> 8) +((v * 183) >> 8);
bdif = u +( (u*198) >> 8);

r = YUVdata[YPOS] + rdif;
g = YUVdata[YPOS] - invgdif;
b = YUVdata[YPOS] + bdif;

为了防止出现溢出，还需要判错计算的结果是否在0-255范围内，做类似下面的判断。

if (r>255)
r=255;
if (r<0)
r=0;
*/


#endif /*_COLOR_H*/
