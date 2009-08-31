#ifndef ColorMakerH
#define ColorMakerH

#include "GraphicsDefs.h"

class ColorMaker
{
	rgb_color ColorFromNumbers(uint8 r, uint8 g, uint8 b);
public:
/*	rgb_color cblue;	//	0	0	255
	rgb_color cred;		//	255	0	0
	rgb_color cgreen;	//	63	127	63
	rgb_color cpurple;	//	255 0	255
	rgb_color ccyan;	//	0	255	255
	rgb_color cyellow;	//	255	255	0
	rgb_color cdred;	//	127	0	0
	rgb_color cdgreen;	//	0	127	0
	rgb_color cdblue;	//	0	0	127
	rgb_color colive;	//	127	127	0
	rgb_color cdpurple;	//	127	0	127
	rgb_color caqua;	//	0	127	127
	rgb_color cfrblue;	//	127	127	255
	rgb_color ctan;		//	195	195	127
	rgb_color cpeach;	//	255	206	156
	rgb_color clbrown;	//	206	156	99
	rgb_color cbrown;	//	156	99	49
	rgb_color cdbrown;	//	99	49	0
*/	rgb_color cltyellow;//	251	254	228
	rgb_color cltgreen;	//	239	254	228
	rgb_color clavendar;//	226	219	228
	rgb_color cpink;	//	253	219	228
	rgb_color cltaqua;	//	200	255	255
	rgb_color cpeach;	//	253	216	200	
	rgb_color cgrint;	//	215	236	230
	rgb_color ctan;		//	235	220	183
	rgb_color cslblue;	//	197	201	219
	rgb_color cbuff;	//	255	236	230
	rgb_color cwhite;	//	255	255	255
	ColorMaker();
	rgb_color ColorNumber(uint8 i);
};

#endif