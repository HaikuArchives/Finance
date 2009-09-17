#ifndef AccH
#define AccH

#include <list>
#include <set>
#include <string>

#include "Prefs.h"
#include "Tra.h"

class BFile;

class Acc
{
public:
	typedef set<uint16> u16s;
	typedef list<uint16> u16l;
	typedef set<Tra*, Tra> trs;
	typedef set<Pye*, NameSorted> pas;
	typedef set<Cat*, NameSorted> cas;
	Acc* sthis;
	u16s chkNumL;
	u16l nxtChkL;
	string name;
	int32 openBal;
	trs traS;
	Acc(string name);
	Acc(BFile* file, pas* pyeS, cas* catS, Prefs* prefs);
	~Acc();
	void CalcBal(Tra* startT = 0);
	void CalcReconBal(Tra* startT = 0);
	void GrayEveryOther(Tra* startT = 0);
	void MakeNxtChkL();
	void Serialize(BFile* file);
};

#endif
