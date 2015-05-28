#include <hw/video/VText.h>
#include <util/string/string.h>
#include <system/func/kprintf.h>

volatile uint16_t * HW::Video::VText :: VMem;
size_t HW::Video::VText :: Row;
size_t HW::Video::VText :: Columb;
uint8_t HW::Video::VText :: Color;

void HW::Video::VText :: Init ( uint16_t ClearColor )
{

	Row = 0;
	Columb = 0;

	Color = ClearColor;

	VMem = reinterpret_cast <uint16_t *> ( 0xB8000 );

	Clear ();
	
	system_func_kprintfOutput ( & PrintVAList );

};

void HW::Video::VText :: SetCursor ( size_t X, size_t Y )
{

	Row = Y;
	Columb = X;

	if ( Columb >= VT_WIDTH )
		Columb = VT_WIDTH - 1;

};

void HW::Video::VText :: SetColor ( uint8_t Color )
{

	VText :: Color = Color;

};

void HW::Video::VText :: PutString ( const char * String )
{

	while ( * String )
		PutChar ( * String ++ );

};

void HW::Video::VText :: PutChar ( char Character )
{
	
	if ( Character == '\n' )
	{
		
		Columb = 0;
		Row ++;
		
		if ( Row >= VT_HEIGHT )
			Scroll ( Row - VT_HEIGHT + 1 );
		
		return;
		
	}
		
	VMem [ Row * VT_WIDTH + Columb ] = MakeEntry ( Character, Color );
	Columb ++;
	
	if ( Columb >= VT_WIDTH )
	{

		Columb = 0;
		Row ++;

		if ( Row >= VT_HEIGHT )
			Scroll ( Row - VT_HEIGHT + 1 );

	}	

};

void HW::Video::VText :: SetString ( size_t X, size_t Y, const char * String )
{

	if ( Y >= VT_HEIGHT )
		return;

	if ( X >= VT_WIDTH )
	{

		X = 0;
		Y ++;
		
		if ( Y >= VT_HEIGHT )
			return;

	}

	while ( * String )
	{

		VMem [ Y * VT_WIDTH + X ++ ] = MakeEntry ( * String ++, Color );

		if ( X >= VT_WIDTH - 1 )
		{

			X = 0;
			Y ++;

			if ( Y >= VT_HEIGHT )
				return;

		}

	}

};

void HW::Video::VText :: SetChar ( size_t X, size_t Y, char Character )
{

	if ( Y >= VT_HEIGHT )
		return;

	if ( X >= VT_WIDTH )
	{

		X = 0;
		Y ++;
		
		if ( Y >= VT_HEIGHT )
			return;
		
	}

	VMem [ Y * VT_WIDTH + X ] = MakeEntry ( Character, Color );

};

void HW::Video::VText :: Scroll ( uint8_t Lines )
{

	if ( Lines >= VT_HEIGHT )
	{

		Clear ();

		Row = 0;
		Columb = 0;

		return;

	}

	Row -= Lines;

	size_t i;
	size_t TransferBlockSize = ( VT_HEIGHT - Lines ) * VT_WIDTH;

	for ( i = 0; i < TransferBlockSize; i ++ )
		VMem [ i ] = VMem [ i + Lines * VT_WIDTH ];

	for ( i = TransferBlockSize; i < VT_WIDTH * VT_HEIGHT; i ++ )
		VMem [ i ] = MakeEntry ( ' ', Color );

};

void HW::Video::VText :: Clear ()
{

	uint16_t ClearEntry = MakeEntry ( ' ', Color );

	for ( size_t i = 0; i < VT_WIDTH * VT_HEIGHT; i ++ )
		VMem [ i ] = ClearEntry;

};

void HW::Video::VText :: PrintF ( const char * FormatString, ... )
{

	va_list Args;

	va_start ( Args, FormatString );
	PrintVAList ( FormatString, Args );
	va_end ( Args );

};

void HW::Video::VText :: PrintVAList ( const char * FormatString, va_list Args )
{
	
	while ( * FormatString )
	{
		
		if ( * FormatString == '%' )
		{
			
			FormatString ++;
			
			if ( ! * FormatString )
				return;
			
			switch ( * FormatString )
			{
				
			case 's':
				{
				
					const char * String2 = va_arg ( Args, const char * );
					
					PutString ( String2 );
					
				}
				
				break;
				
			case 'i':
			case 'd':
				{
					
					int32_t Value = va_arg ( Args, int32_t );
					char Temp [ 12 ];
					
					itoa ( Value, Temp, 10 );
					
					PutString ( Temp );
					
				}
				
				break;
				
			case 'u':
				{
					
					uint32_t Value = va_arg ( Args, uint32_t );
					char Temp [ 11 ];
					
					utoa ( Value, Temp, 10 );
					
					PutString ( Temp );
					
				}
				
				break;
				
			case 'x':
				{
					
					uint32_t Value = va_arg ( Args, uint32_t );
					char Temp [ 9 ];
					
					utoa ( Value, Temp, 16 );
					
					PutString ( Temp );
					
				}
				
				break;
				
			case 'h':
				{
					
					uint32_t Value = va_arg ( Args, uint32_t );
					char Temp [ 9 ];
					
					utoa ( Value, Temp, 16 );
					
					PutString ( "0x" );
					PutString ( Temp );
					
				}
				
				break;
				
			case 'c':
				{
					
					char Value = static_cast <char> ( va_arg ( Args, uint32_t ) );
					
					PutChar ( Value );
					
				}
				
				break;
				
			case 'b':
				{
					
					uint32_t Value = va_arg ( Args, uint32_t );
					char Temp [ 33 ];
					
					utoa ( Value, Temp, 2 );
					
					PutString ( Temp );
			
				}
				
				break;
				
			default:
			
				break;
			
			}
			
		}
		else
			PutChar ( * FormatString );
		
		FormatString ++;
		
	}

};

