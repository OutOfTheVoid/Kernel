#include <hw/acpi/aml/Object.h>

bool HW::ACPI::AML::Object :: CoerceObjectToInteger ( ACPIObject * Object, bool Is64Bit )
{
	
	union
	{
		
		uint32_t Temp32;
		uint64_t Temp64;
		
		struct
		{
			
			uint32_t Index;
			
			char TestChar;
			
			union
			{
				
				uint32_t Temp32;
				uint64_t Temp64;
				
			};
			
		} StrTest;
		
	};
	
	switch ( Object -> Type )
	{
		
	case kObjectType_Int64:
	
		if ( ! Is64Bit )
		{
			
			Object -> Value.Int32 = static_cast <uint32_t> ( Object -> Value.Int64 );
			Object -> Type = kObjectType_Int32;
			
		}
		
		return true;
		
	case kObjectType_Int32:
	
		if ( Is64Bit )
		{
			
			Object -> Value.Int64 = static_cast <uint64_t> ( Object -> Value.Int32 );
			Object -> Type = kObjectType_Int64;
			
		}
		
		return true;
		
	case kObjectType_BCDInt32:
		
		if ( Is64Bit )
		{
			
			Temp64 = 0;
			
			Temp64 += ( Object -> Value.BCDInt32 & 0x0000000F ) >> 0;
			Temp64 += ( ( Object -> Value.BCDInt32 & 0x000000F0 ) >> 4 ) * 10;
			Temp64 += ( ( Object -> Value.BCDInt32 & 0x00000F00 ) >> 8 ) * 100;
			Temp64 += ( ( Object -> Value.BCDInt32 & 0x0000F000 ) >> 12 ) * 1000;
			Temp64 += ( ( Object -> Value.BCDInt32 & 0x000F0000 ) >> 16 ) * 10000;
			Temp64 += ( ( Object -> Value.BCDInt32 & 0x00F00000 ) >> 20 ) * 100000;
			Temp64 += ( ( Object -> Value.BCDInt32 & 0x0F000000 ) >> 24 ) * 1000000;
			Temp64 += ( ( Object -> Value.BCDInt32 & 0xF0000000 ) >> 28 ) * 10000000;
			
			Object -> Type = kObjectType_Int64;
			Object -> Value.Int64 = Temp64;
			
		}
		else
		{
			
			Temp32 = 0;
			
			Temp32 += ( Object -> Value.BCDInt32 & 0x0000000F ) >> 0;
			Temp32 += ( ( Object -> Value.BCDInt32 & 0x000000F0 ) >> 4 ) * 10;
			Temp32 += ( ( Object -> Value.BCDInt32 & 0x00000F00 ) >> 8 ) * 100;
			Temp32 += ( ( Object -> Value.BCDInt32 & 0x0000F000 ) >> 12 ) * 1000;
			Temp32 += ( ( Object -> Value.BCDInt32 & 0x000F0000 ) >> 16 ) * 10000;
			Temp32 += ( ( Object -> Value.BCDInt32 & 0x00F00000 ) >> 20 ) * 100000;
			Temp32 += ( ( Object -> Value.BCDInt32 & 0x0F000000 ) >> 24 ) * 1000000;
			Temp32 += ( ( Object -> Value.BCDInt32 & 0xF0000000 ) >> 28 ) * 10000000;
			
			Object -> Type = kObjectType_Int32;
			Object -> Value.Int32 = Temp32;
			
		}
		
		return true;
		
	case kObjectType_BCDInt64:
		
		if ( Is64Bit )
		{
			
			Temp64 = 0;
			
			Temp64 += ( ( Object -> Value.BCDInt64 & 0x000000000000000F ) >> 0 ) * 1;
			Temp64 += ( ( Object -> Value.BCDInt64 & 0x00000000000000F0 ) >> 4 ) * 10;
			Temp64 += ( ( Object -> Value.BCDInt64 & 0x0000000000000F00 ) >> 8 ) * 100;
			Temp64 += ( ( Object -> Value.BCDInt64 & 0x000000000000F000 ) >> 12 ) * 1000;
			Temp64 += ( ( Object -> Value.BCDInt64 & 0x00000000000F0000 ) >> 16 ) * 10000;
			Temp64 += ( ( Object -> Value.BCDInt64 & 0x0000000000F00000 ) >> 20 ) * 100000;
			Temp64 += ( ( Object -> Value.BCDInt64 & 0x000000000F000000 ) >> 24 ) * 1000000;
			Temp64 += ( ( Object -> Value.BCDInt64 & 0x00000000F0000000 ) >> 28 ) * 10000000;
			Temp64 += ( ( Object -> Value.BCDInt64 & 0x0000000F00000000 ) >> 32 ) * 100000000;
			Temp64 += ( ( Object -> Value.BCDInt64 & 0x000000F000000000 ) >> 36 ) * 1000000000;
			Temp64 += ( ( Object -> Value.BCDInt64 & 0x00000F0000000000 ) >> 40 ) * 10000000000;
			Temp64 += ( ( Object -> Value.BCDInt64 & 0x0000F00000000000 ) >> 44 ) * 100000000000;
			Temp64 += ( ( Object -> Value.BCDInt64 & 0x000F000000000000 ) >> 48 ) * 1000000000000;
			Temp64 += ( ( Object -> Value.BCDInt64 & 0x00F0000000000000 ) >> 52 ) * 10000000000000;
			Temp64 += ( ( Object -> Value.BCDInt64 & 0x0F00000000000000 ) >> 56 ) * 100000000000000;
			Temp64 += ( ( Object -> Value.BCDInt64 & 0xF000000000000000 ) >> 60 ) * 1000000000000000;
			
			Object -> Type = kObjectType_Int64;
			Object -> Value.Int64 = Temp64;
			
		}
		else
		{
			
			Temp32 = 0;
			
			Temp32 += ( ( Object -> Value.BCDInt64 & 0x000000000000000F ) >> 0 ) * 1;
			Temp32 += ( ( Object -> Value.BCDInt64 & 0x00000000000000F0 ) >> 4 ) * 10;
			Temp32 += ( ( Object -> Value.BCDInt64 & 0x0000000000000F00 ) >> 8 ) * 100;
			Temp32 += ( ( Object -> Value.BCDInt64 & 0x000000000000F000 ) >> 12 ) * 1000;
			Temp32 += ( ( Object -> Value.BCDInt64 & 0x00000000000F0000 ) >> 16 ) * 10000;
			Temp32 += ( ( Object -> Value.BCDInt64 & 0x0000000000F00000 ) >> 20 ) * 100000;
			Temp32 += ( ( Object -> Value.BCDInt64 & 0x000000000F000000 ) >> 24 ) * 1000000;
			Temp32 += ( ( Object -> Value.BCDInt64 & 0x00000000F0000000 ) >> 28 ) * 10000000;
			Temp32 += ( ( Object -> Value.BCDInt64 & 0x0000000F00000000 ) >> 32 ) * 100000000;
			Temp32 += ( ( Object -> Value.BCDInt64 & 0x000000F000000000 ) >> 36 ) * 1000000000;
			
			Object -> Type = kObjectType_Int32;
			Object -> Value.Int32 = Temp32;
			
		}
		
		return true;
		
	case kObjectType_String:
		
		if ( Object -> Value.String.Length == 0 )
			return false;
		
		StrTest.Index = 0;
		
		if ( Is64Bit )
		{
			
			StrTest.Temp64 = 0;
			
			while ( ( StrTest.Index < Object -> Value.String.Length ) && ( StrTest.Index < 16 ) )
			{
				
				Temp64 <<= 4;
				
				StrTest.TestChar = Object -> Value.String.CString [ StrTest.Index ];
				
				if ( ( StrTest.TestChar >= '0' ) && ( StrTest.TestChar <= '9' ) )
					Temp64 += static_cast <uint64_t> ( StrTest.TestChar - '0' );
				else if ( ( StrTest.TestChar >= 'A' ) && ( StrTest.TestChar <= 'F' ) )
					Temp64 += static_cast <uint64_t> ( StrTest.TestChar - 'A' ) + 10;
				else if ( ( StrTest.TestChar >= 'a' ) && ( StrTest.TestChar <= 'f' ) )
					Temp64 += static_cast <uint64_t> ( StrTest.TestChar - 'a' ) + 10;
				else
					break;
				
			}
			
			if ( Object -> Value.String.Allocated )
				ACPIFree ( reinterpret_cast <void *> ( const_cast <char *> ( Object -> Value.String.CString ) ) );
			
			Object -> Value.Int64 = Temp64;
			Object -> Type = kObjectType_Int64;
			
		}
		else
		{
			
			StrTest.Temp32 = 0;
			
			while ( ( StrTest.Index < Object -> Value.String.Length ) && ( StrTest.Index < 8 ) )
			{
				
				StrTest.Temp32 <<= 4;
				
				StrTest.TestChar = Object -> Value.String.CString [ StrTest.Index ];
				
				if ( ( StrTest.TestChar >= '0' ) && ( StrTest.TestChar <= '9' ) )
					StrTest.Temp32 += static_cast <uint64_t> ( StrTest.TestChar - '0' );
				else if ( ( StrTest.TestChar >= 'A' ) && ( StrTest.TestChar <= 'F' ) )
					StrTest.Temp32 += static_cast <uint64_t> ( StrTest.TestChar - 'A' ) + 10;
				else if ( ( StrTest.TestChar >= 'a' ) && ( StrTest.TestChar <= 'f' ) )
					StrTest.Temp32 += static_cast <uint64_t> ( StrTest.TestChar - 'a' ) + 10;
				else
					break;
				
			}
			
			if ( Object -> Value.String.Allocated )
				ACPIFree ( reinterpret_cast <void *> ( const_cast <char *> ( Object -> Value.String.CString ) ) );
			
			Object -> Value.Int32 = StrTest.Temp32;
			Object -> Type = kObjectType_Int32;
			
		}
		
	}
	
};

