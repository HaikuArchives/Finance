#ifndef NumfieldH
#define NumfieldH

#include <SupportDefs.h>

class Numfield
{
public:
	// 0 = check
	// 1 = ATM
	// 2 = EFT
	// 3 = Xfer
	// 4 = NULL
	uint8 type;
	uint16 chknum;
};

#endif
