#ifndef SOURCE_DATA_TYPES_MESSAGE_TYPE_H
#define SOURCE_DATA_TYPES_MESSAGE_TYPE_H
//#include "source/data_types/message_type.h"

#include <stdint.h>

enum MessageType: uint32_t {
	// Must begin at zero.
	null = 0,
	// Values are used to index arrays so should
	// preferably be contiguous to prevent wasted array
	// space from unused elements. Do not change existing
	// values. They are transmitted over networks and
	// read by other languages. You will need to apply
	// changes to all code bases.

	move = 1,
	ability = 2,
	login = 3,
	logout = 4,

	//must always appear last
	NumberOfMessageTypes
};

#endif /* SOURCE_DATA_TYPES_MESSAGE_TYPE_H */
