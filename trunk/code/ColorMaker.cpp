#include <stdexcept>
#include "ColorMaker.h"

ColorMaker::ColorMaker()
{
/*	cblue = ColorFromNumbers(0, 0, 255);
	cred = ColorFromNumbers(255, 0, 0);
	cgreen = ColorFromNumbers(63, 127, 63);
	cpurple = ColorFromNumbers(255, 0, 255);
	ccyan = ColorFromNumbers(0, 255, 255);
	cyellow = ColorFromNumbers(255, 255, 0);
	cdred = ColorFromNumbers(127, 0, 0);
	cdgreen = ColorFromNumbers(0, 127, 0);
	cdblue = ColorFromNumbers(0, 0, 127);
	colive = ColorFromNumbers(127, 127, 0);
	cdpurple = ColorFromNumbers(127, 0, 127);
	caqua = ColorFromNumbers(0, 127, 127);
	cfrblue = ColorFromNumbers(127, 127, 255);
	ctan = ColorFromNumbers(195, 195, 127);
	cpeach = ColorFromNumbers(255, 206, 156);
	clbrown = ColorFromNumbers(206, 156, 99);
	cbrown = ColorFromNumbers(156, 99, 49);
	cdbrown = ColorFromNumbers(99, 49, 0);*/
	cltyellow = ColorFromNumbers(251, 254, 228);
	cltgreen = ColorFromNumbers(239, 254, 228);
	clavendar = ColorFromNumbers(226, 219, 228);
	cpink = ColorFromNumbers(242, 219, 228);
	cltaqua = ColorFromNumbers(200, 255, 255);
	cpeach = ColorFromNumbers(253, 216, 200);
	cgrint = ColorFromNumbers(215, 236, 230);
	ctan = ColorFromNumbers(235, 220, 183);
	cslblue = ColorFromNumbers(197, 201, 219);
	cbuff = ColorFromNumbers(255, 236, 230);
	cwhite = ColorFromNumbers(255, 255, 255);
}

rgb_color ColorMaker::ColorFromNumbers(uint8 r, uint8 g, uint8 b)
{
	rgb_color c;
	c.red = r;
	c.green = g;
	c.blue = b;
	return c;
}

rgb_color ColorMaker::ColorNumber(uint8 i)
{
	rgb_color rc;
	switch(i)
	{
		case 0:
			rc = cltyellow;
		break;
		case 1:
			rc = cltgreen;
		break;
		case 2:
			rc = clavendar;
		break;
		case 3:
			rc = cpink;
		break;
		case 4:
			rc = cltaqua;
		break;
		case 5:
			rc = cpeach;
		break;
		case 6:
			rc = cgrint;
		break;
		case 7:
			rc = ctan;
		break;
		case 8:
			rc = cslblue;
		break;
		case 9:
			rc = cbuff;
		break;
/*		case 10:
			rc = cdpurple;
		break;
		case 11:
			rc = caqua;
		break;
		case 12:
			rc = cfrblue;
		break;
*/		case 255:
			rc = cwhite;
		break;
		default:
			throw new exception;
	}
	return rc;
}