#ifndef PrefsH
#define PrefsH

#include <SupportDefs.h>
#include <string>
#include <time.h>

class Prefs
{
public:
	string curSym;
	bool curSymPlaceBefore; //true = before
	string curSep;
	string curDec;
	uint8 curDecPlace;
	uint8 dateFormat; //0 = mmddyy, 1 = ddmmyy
	string dateSep;
	Prefs();
	Prefs(string curSym, bool curSymPlaceBefore, string curSep,
		string curDec, uint8 curDecPlace, uint8 dateFormat, string dateSep);
	void Set(Prefs* prefsToCopy);
	void Set(string curSym, bool curSymPlaceBefore, string curSep,
		string curDec, uint8 curDecPlace, uint8 dateFormat, string dateSep);
	string currencyToString(int32 amount);
	string dateToString(time_t date);
	int32 stringToCurrency(string amount);
	time_t stringToDate(string date);
};

#endif