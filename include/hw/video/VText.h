#ifndef HW_VIDEO_VTEXT_H
#define HW_VIDEO_VTEXT_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <hw/HW.h>
#include <hw/video/Video.h>

namespace HW
{

	namespace Video
	{
		
		class VText
		{
		public:

			static const size_t VT_WIDTH = 80;
			static const size_t VT_HEIGHT = 25;
			
			static const uint8_t Color_Black = 0;
			static const uint8_t Color_Blue = 1;
			static const uint8_t Color_Green = 2;
			static const uint8_t Color_Cyan = 3;
			static const uint8_t Color_Red = 4;
			static const uint8_t Color_Magenta = 5;
			static const uint8_t Color_Brown = 6;
			static const uint8_t Color_LightGrey = 7;
			static const uint8_t Color_Grey = 8;
			static const uint8_t Color_LightBlue = 9;
			static const uint8_t Color_LightGreen = 10;
			static const uint8_t Color_LightCyan = 11;
			static const uint8_t Color_LightRed = 12;
			static const uint8_t Color_LightMagenta = 13;
			static const uint8_t Color_LightBrown = 14;
			static const uint8_t Color_White = 15;

			static inline uint8_t MakeColor ( uint8_t Foreground, uint8_t Background )
			{

				return Foreground | Background << 4;

			};

			static inline uint16_t MakeEntry ( char Character, uint8_t Color )
			{

				return static_cast <uint16_t> ( Character ) | static_cast <uint16_t> ( Color ) << 8;

			};

			static void Init ( uint16_t ClearColor );

			static void SetCursor ( size_t X, size_t Y );
			static void SetColor ( uint8_t Color );

			static void PutString ( const char * String );
			static void PutChar ( char Character );
			static void SetString ( size_t X, size_t Y, const char * String );
			static void SetChar ( size_t X, size_t Y, char Character );

			static void PrintF ( const char * FormatString, ... );
			static void PrintVAList ( const char * FormatString, va_list Args );
			
			static void Scroll ( uint8_t Lines );
			static void Clear ();

		private:

			static volatile uint16_t * VMem;

			static size_t Row;
			static size_t Columb;
			static uint8_t Color;

		};

	};

};

#endif
