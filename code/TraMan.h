#ifndef TraManH
#define TraManH

#include <list>
#include <map>
#include <set>
#include <string>

#include "Acc.h"
#include "NameSorted.h"
#include "Prefs.h"

class Cat;
class Pye;
class Tra;

class TraMan
{
	typedef set<Tra*, Tra> trs;
	typedef list<Acc*> acl;
	typedef set<Cat*, NameSorted> cas;
	typedef list<Cat*> cal;
	typedef set<Pye*, NameSorted> pas;
	typedef list<time_t> til;
	typedef list<uint16> u16l;
	typedef list<uint32> u32l;
	typedef list<int32> i32l;
	typedef list<Pye*> pal;
	typedef pair<uint16, Tra*> u32tp;
	typedef map<string, Acc*> stacm;
	static const uint8 numRecToKeep = 10;
	acl accL;
	cas catS;
	pas pyeS;
	til recDatL;
	u32l recDepL;
	pal recPyeL;
	u32l recPamL;
	stacm strAccM;
	Prefs prefs;
	void AddAmt(Tra* tra);
	void RemoveAmt(Tra* tra);
public:
	TraMan();
	~TraMan();
	void AccUpdated(Acc* acc);
	Cat* AddCat(string name);
	void AddPye(Pye* pye);
	u32tp AddCtrprtTra(Tra* tra, Acc* acc);
	uint16 AddTra(Acc* acc, Tra* tra);
	void ClearData();
	bool DataExists();
	void DeleteData();
	uint16 DeleteTra(Acc* acc, Tra* tra);
	void EditTra(Acc* acc, Tra* curT, Tra* newT);
	acl* GetAccL();
	u16l* GetNxtChkL(Acc* acc);
	pas* GetPyeS();
	cas* GetCatS();
	Prefs* GetPrefs();
	int32 GetRecBal(Acc* acc);
	til* GetRecDatL();
	u32l* GetRecDepL();
	pal* GetRecPyeL();
	u32l* GetRecPamL();
	stacm* GetStrAccM();
	trs* GetTraS(Acc* acc);
	Acc* NewAcc(string name);
	bool Open(BEntry* entry);
	void PrefsUpdated(int8 decPlaceChange);
	void ReconFinish(Acc* acc);
	void ReconTra(Acc* acc, Tra* tra);
	void ReplaceCat(Cat* icat, Cat* ncat);
	void ReplacePye(Pye* ipye, Pye* npye);
	void Save(BEntry* entry);
};

template<class T>class SerialMatch : public unary_function<T, bool>
{
	T sToMatch;
public:
	explicit SerialMatch(T x) : sToMatch(x) {}
	bool operator()(T x) const
	{
//		printf("%p %p\n", x->sthis, sToMatch);
		return x->sthis == sToMatch;
	}
};

#endif
