#pragma once

#include "record.h"

class RecordFactory{
private:
	RecordFactory( const RecordFactory& rf ){}
	RecordFactory& operator=( const RecordFactory& rf ){}
public:
	RecordFactory(){}

	static RecordABC* GetRecord( const RecordeInfo& info )
	{
		switch( info.sampleType )
		{
		case SAMPLE_TYPE_FLOAT32:
			return new RecordFloat32( info );
			break;
		case SAMPLE_TYPE_INT16:
			return new RecordInt16( info );
			break;
		case SAMPLE_TYPE_INT8:
			return new RecordInt8( info );
			break;
		case SAMPLE_TYPE_UINT8:
			return new RecordUInt8( info );
			break;
		default:
			return nullptr;
			break;
		}
	}
};