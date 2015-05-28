#ifndef HW_APM_APMHEADER_H
#define HW_APM_APMHEADER_H

typedef struct
{
	
	uint16_t Version;
	
	uint16_t CodeSegment;
	uint32_t Offset;
	uint16_t CodeSegment16;
	uint16_t DataSegment;
	
	uint16_t Flags;
	
	uint16_t CodeSegmentLength;
	uint16_t CodeSegment16Length;
	uint16_t DataSegmentLength;
	
} __attribute__((packed)) APMHeader;

#endif
