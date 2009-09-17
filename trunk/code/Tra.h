#ifndef TraH
#define TraH

#include <set>
#include <string>
#include "NameSorted.h"
#include "Numfield.h"
#include "Prefs.h"
class Cat;
class Pye;
class Acc;

class BFile;

class Tra
{
public:
	Tra* sthis;
	Numfield num;
	time_t date;
	Pye* pye;
	uint32 amt;
	bool pam;  //or dep?
	Cat* cat;
	string mem;
	bool gry;
	int32 balFol;
	int32 recBalFol;
	uint8 rec;	//0 - no, 1 - in reconciliation, 2 - confirmed
	bool tsf;
	Tra* ctrprt;
	Tra* sctrprt;
	Acc* tsfAcc;
	Acc* stsfAcc;
	Tra();
	Tra(string numtext, string datetext, Pye* pye,
		string pamtext, string deptext, Cat* cat, 
		string memtext, uint8 reconciled, bool tsf,
		Acc* tsfAcc, Prefs* prefs);
	Tra(BFile* file, set<Pye*, NameSorted>* pyeset,
		set<Cat*, NameSorted>* catset, Prefs* prefs);
	~Tra();
	int operator()(const Tra* t1, const Tra* t2) const;
	uint8 Month();
	uint8 Year();
	void Serialize(BFile* file);
};

#endif
