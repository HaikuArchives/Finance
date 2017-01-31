#ifndef ProxyH
#define ProxyH

#include <fstream>
#include <list>
#include <map>
#include <set>
#include <string>
#include "NameSorted.h"
#include "Prefs.h"
#include "SupportDefs.h"

class BEntry;

class Acc;
class Cat;
class MainView;
class Pye;
class Tra;
class TraMan;

class Proxy
{
	typedef list<Acc*> acl;
	typedef set<Tra*, Tra> trs;
	TraMan* tm;
	MainView* mv;
	typedef map<string, Acc*> stacm;
	typedef set<Cat*, NameSorted> cas;
	typedef set<Pye*, NameSorted> pas;
	typedef list<Cat*> cal;
	typedef list<int32> i32l;
	string GetStrFromIfstream(ifstream* stream);
	pair<char, string> QifParse(ifstream* qifFile);
public:
	void AccUpdated(Acc* acc);
	Cat* AddCat(string name, bool update = true);
	void AddPye(Pye* pye, bool update = true);
	void AddTra(Acc* acc, Tra* transaction);
	void BMCImport(Acc* acc, BEntry* entry);
	void ClearData();
	bool DataExists();
	void DeleteCat(Cat* cat, Cat* ncat);
	void DeletePye(Pye* pye, Pye* npye);
	void DeleteTra(Acc* acc, Tra* tra);
	void EditTra(Acc* acc, Tra* curT, 
		Tra* newT);
	void Export(Acc* acc, BEntry* entry);
	list<Acc*>* GetAccL();
	set<Cat*, NameSorted>* GetCatS();
	list<uint16>* GetNxtChkL(Acc* acc);
	Prefs* GetPrefs();
	set<Pye*, NameSorted>* GetPyeS();
	int32 GetRecBal(Acc* acc);
	list<time_t>* GetRecDatL();
	list<uint32>* GetRecDepL();
	list<Pye*>* GetRecPyeL();
	list<uint32>* GetRecPamL();
	set<Tra*, Tra>* GetTraSet(Acc* acc);
	void Import(Acc* acc, BEntry* entry);
	void NewAcc(string name);
	void PrefsUpdated(int8 decPlaceChange);
	bool Open(BEntry* entry);
	void ReconFinish(Acc* acc);
	void ReconTra(Acc* acc, Tra* t);
	void Save(BEntry* entry);
	void SetTraMan(TraMan* tm);
	void SetMainView(MainView* mv);
};

#endif
