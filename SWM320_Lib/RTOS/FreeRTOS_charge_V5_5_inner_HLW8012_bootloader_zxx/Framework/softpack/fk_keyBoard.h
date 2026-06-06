/*********************************************************************************************************************/
#ifndef __FK_KEYBOARD_H_
#define __FK_KEYBOARD_H_	 
//#include "sys.h"
/* 功能键 键值定义 ********************************************************************************/

	// 无 shift
	#define KEY_ALT							0x01		// Alt		(替换)
	#define KEY_CTRL						0x02		// Ctrl 	(控制)
	#define KEY_SHIFT						0x03		// Shift	(转移)
	#define KEY_CAPS						0x04		// Caps 	(大写)
	#define KEY_FN							0x05		// Fn 		(功能)
	#define KEY_PARAM						0x06		// i 		(参数)
	#define KEY_LUM							0x07		// ? 		(亮度)
	#define KEY_BACKSPACE				0x08		// Back 	(退格)
	#define KEY_UP							0x09		// ↑ 		(上)		
	#define KEY_DOWN						0x0A		// ↓ 		(下)		
	#define KEY_LEFT						0x0B		// ← 		(左)
	#define KEY_RIGHT						0x0C		// → 		(右)
	#define KEY_ENTER						0x0D		// Enter 	(回车)
	#define KEY_LOCK						0x0E		// 锁定
	#define KEY_F1							0x11		// F1
	#define KEY_F2							0x12		// F2
	#define KEY_F3							0x13		// F3
	#define KEY_F4							0x14		// F4
	#define KEY_ESC							0x18		// Esc		(退出)
	#define KEY_POUND						0x19		// 英镑
	#define KEY_DEL							0x7F		// Del		(删除)

	// shift+
	#define KEY_SHIFT_ALT					0x80|0x01	// Alt		(替换)	
	#define KEY_SHIFT_CTRL				0x80|0x02	// Ctrl 	(控制)
//	#define KEY_SHIFT_SHIFT				0x80|0x03	// Shift	(转移)
	#define KEY_SHIFT_CAPS				0x80|0x04	// Caps 	(大写)
	#define KEY_SHIFT_FN					0x80|0x05	// Fn 		(功能)	
	#define KEY_SHIFT_PARAM				0x80|0x06	// i 		(参数)
	#define KEY_SHIFT_LUM					0x80|0x07	// ? 		(亮度)	
	#define KEY_SHIFT_BACKSPACE		0x80|0x08	// Back 	(退格)
	#define KEY_SHIFT_UP					0x80|0x09	// ↑ 		(上)		
	#define KEY_SHIFT_DOWN				0x80|0x0A	// ↓ 		(下)	
	#define KEY_SHIFT_LEFT				0x80|0x0A	// ← 		(左)
	#define KEY_SHIFT_RIGHT				0x80|0x0C	// → 		(右)
	#define KEY_SHIFT_ENTER				0x80|0x0D	// Enter 	(回车)
	#define KEY_SHIFT_LOCK				0x80|0x0E	// 锁定
	#define KEY_SHIFT_F1					0x80|0x11	// F1	
	#define KEY_SHIFT_F2					0x80|0x12	// F2	
	#define KEY_SHIFT_F3					0x80|0x13	// F3	
	#define KEY_SHIFT_F4					0x80|0x14	// F4	
	#define KEY_SHIFT_ESC					0x80|0x18	// Esc		(退出)	
	#define KEY_SHIFT_DEL					0x80|0x7F	// Del		(删除)	

/*************************************************************************************************/

/* ASCII键 键值定义 *******************************************************************************/

	// 无 shift
	#define KEY_1							'1'
	#define KEY_2							'2'
	#define KEY_3							'3'
	#define KEY_4							'4'
	#define KEY_5							'5'
	#define KEY_6							'6'
	#define KEY_7							'7'
	#define KEY_8							'8'
	#define KEY_9							'9'
	#define KEY_0							'0'
	
	#define KEY_A							'A'
	#define KEY_B							'B'
	#define KEY_C							'C'
	#define KEY_D							'D'
	#define KEY_E							'E'
	#define KEY_F							'F'
	#define KEY_G							'G'
	#define KEY_H							'H'
	#define KEY_I							'I'
	#define KEY_J							'J'
	#define KEY_K							'K'
	#define KEY_L							'L'
	#define KEY_M							'M'
	#define KEY_N							'N'
	#define KEY_O							'O'
	#define KEY_P							'P'
	#define KEY_Q							'Q'
	#define KEY_R							'R'
	#define KEY_S							'S'
	#define KEY_T							'T'
	#define KEY_U							'U'
	#define KEY_V							'V'
	#define KEY_W							'W'
	#define KEY_X							'X'
	#define KEY_Y							'Y'
	#define KEY_Z							'Z'
	
	#define KEY_RIGHTSQUARE					']'			// }   ]    价  限
	#define KEY_LEFTSQUARE					'['			// {   [    售  卖
	#define KEY_PLUS						'='			// +   =    万  百
	#define KEY_MINUS					   	'-'			// -   千   十		
	#define KEY_NUMBER					  	'#'			// #   ~    角   放
	#define KEY_APOSTROPHE					0x27		// '   @    元   存
	#define KEY_SEMICOLON					';'			// ;   ：   厘   质
	#define KEY_SPACE						' '			// 空格
	#define KEY_BACKSLASH					0x5C		// 反斜杠\ | 消  此
	#define KEY_SLASH						'/'			// /
	#define KEY_DOT							'.'			// .
	#define KEY_COMMA						','			// ,
	
	// shift+
	#define KEY_SHIFT_1						'!'
	#define KEY_SHIFT_2						'"'
	#define KEY_SHIFT_3						0x19
	#define KEY_SHIFT_4						'$'
	#define KEY_SHIFT_5						'%'
	#define KEY_SHIFT_6						'^'
	#define KEY_SHIFT_7						'&'
	#define KEY_SHIFT_8						'*'
	#define KEY_SHIFT_9						'('
	#define KEY_SHIFT_0						')'
	
	#define KEY_SHIFT_A						'a'
	#define KEY_SHIFT_B						'b'
	#define KEY_SHIFT_C						'c'
	#define KEY_SHIFT_D						'd'
	#define KEY_SHIFT_E						'e'
	#define KEY_SHIFT_F						'f'
	#define KEY_SHIFT_G						'g'
	#define KEY_SHIFT_H						'h'
	#define KEY_SHIFT_I						'i'
	#define KEY_SHIFT_J						'j'
	#define KEY_SHIFT_K						'k'
	#define KEY_SHIFT_L						'l'
	#define KEY_SHIFT_M						'm'
	#define KEY_SHIFT_N						'n'
	#define KEY_SHIFT_O						'o'
	#define KEY_SHIFT_P						'p'
	#define KEY_SHIFT_Q						'q'
	#define KEY_SHIFT_R						'r'
	#define KEY_SHIFT_S						's'
	#define KEY_SHIFT_T						't'
	#define KEY_SHIFT_U						'u'
	#define KEY_SHIFT_V						'v'
	#define KEY_SHIFT_W						'w'
	#define KEY_SHIFT_X						'x'
	#define KEY_SHIFT_Y						'y'
	#define KEY_SHIFT_Z						'z'
	
	#define KEY_SHIFT_RIGHTSQUARE			'}'			// }   ]    价  限
	#define KEY_SHIFT_LEFTSQUARE			'{'			// {   [    售  卖
	#define KEY_SHIFT_PLUS					'+'			// +   =    万  百
	#define KEY_SHIFT_MINUS					'_'			// -   千   十		
	#define KEY_SHIFT_NUMBER				'~'			// #   ~    角   放
	#define KEY_SHIFT_APOSTROPHE			'@'			// '   @    元   存
	#define KEY_SHIFT_SEMICOLON				':'			// ;   ：   厘   质
	#define KEY_SHIFT_SPACE					0x1a		// 空格
	#define KEY_SHIFT_BACKSLASH				'|'			// 反斜杠 | 消  此
	#define KEY_SHIFT_SLASH					'?'		    // /
	#define KEY_SHIFT_DOT					'>'			// .
	#define KEY_SHIFT_COMMA					'<'			// ,
/*************************************************************************************************/	

#endif
