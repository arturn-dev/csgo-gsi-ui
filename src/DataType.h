#ifndef DATATYPE_H
#define DATATYPE_H

enum DataType
{
	NONE 			= 0,
	DATA_RAW 		= 1 << 0,
	DATA_PROCESSED 	= 1 << 1,
	COUNT
};

#endif //DATATYPE_H
