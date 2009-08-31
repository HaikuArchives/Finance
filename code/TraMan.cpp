#include <algorithm>
#include <stdio.h>

#include <Entry.h>
#include <File.h>
#include <TypeConstants.h>

#include "Acc.h"
#include "Pye.h"
#include "SplCat.h"
#include "TraMan.h"
#include "Version.h"

TraMan::TraMan()
{
}

TraMan::~TraMan()
{
	ClearData();
}

void TraMan::AccUpdated(Acc* acc)
{
	acc->CalcBal();
	acc->CalcReconBal();
}

void TraMan::AddAmt(Tra* tra)
{
	if (tra->pam)
	{
		recPamL.push_back(tra->amt);
		recPamL.unique();
		if (recPamL.size() > numRecToKeep)
			recPamL.pop_front();
		if (!tra->tsf)
		{
			if (tra->cat->spl)
			{
				SplCat* sc = (SplCat*)tra->cat;
				i32l::iterator itera = sc->amtL->begin();
				for (cal::iterator iterc = sc->catL->begin();
					iterc != sc->catL->end(); iterc++)
				{
					(*iterc)->sum -= *itera;
					itera++;
				}
			}
			else
				tra->cat->sum -= tra->amt;
		}
	}
	else
	{
		recDepL.push_back(tra->amt);
		recDepL.unique();
		if (recDepL.size() > numRecToKeep)
			recDepL.pop_front();
		if (!tra->tsf)
		{
			if (tra->cat->spl)
			{
				SplCat* sc = (SplCat*)tra->cat;
				i32l::iterator itera = sc->amtL->begin();
				for (cal::iterator iterc = sc->catL->begin();
					iterc != sc->catL->end(); iterc++)
				{
					(*iterc)->sum += *itera;
					itera++;
				}
			}
			else
				tra->cat->sum += tra->amt;
		}
	}
}

Cat* TraMan::AddCat(string name)
{
	Cat* par = 0;
	Cat* cat;
	size_t pos0, pos1;
	pos0 = 0;
	pos1 = name.find(':');
	while (pos1 != string::npos)
	{
		bool fnd = false;
		for (cas::iterator ci = catS.begin(); ci != catS.end(); ci++)
		{
			if ((*ci)->LeafName() == name.substr(pos0, pos1 - pos0))
			{
				fnd = true;
				par = *ci;
				break;
			}
		}
		if (!fnd)
		{
			par = new Cat(name.substr(pos0, pos1 - pos0), par);
			catS.insert(par);
		}
		pos0 = pos1 + 1;
		pos1 = name.find(':', pos0);
	}
	cat = new Cat(name.substr(pos0, name.length() - pos0), par);
	catS.insert(cat);
	return cat;
}

void TraMan::AddPye(Pye* pye)
{
	pyeS.insert(pye);
}

pair<uint16, Tra*> TraMan::AddCtrprtTra(Tra* tra, Acc* acc)
{
	Tra* t = new Tra(*tra);
	t->pam = !t->pam;
	t->num.type = 3;
	pair<trs::iterator, bool> p2;
	p2 = tra->tsfAcc->traS.insert(t);
	tra->tsfAcc->GrayEveryOther(t);
	tra->tsfAcc->CalcBal(t);
	tra->tsfAcc->CalcReconBal(t);
	tra->ctrprt = t;
	t->ctrprt = tra;
	t->tsfAcc = acc;
	uint16 j = 0;
	trs::iterator si2 = tra->tsfAcc->traS.begin();
	while (si2 != p2.first)
	{
		j++;
		si2++;
	}
	return pair<uint16, Tra*>(j, t);
}

uint16 TraMan::AddTra(Acc* acc, Tra* tra)
{
	pair<trs::iterator, bool> p;
	p = acc->traS.insert(tra);
	if (tra->num.type == 0)
	{
		acc->chkNumL.insert(tra->num.chknum);
		acc->MakeNxtChkL();
	}
	recDatL.push_back(tra->date);
	recDatL.unique();
	if (recDatL.size() > numRecToKeep)
		recDatL.pop_front();
	if (!tra->tsf)
	{
		recPyeL.push_back(tra->pye);
		recPyeL.unique();
		if (recPyeL.size() > numRecToKeep)
			recPyeL.pop_front();
	}
	AddAmt(tra);
	acc->GrayEveryOther(tra);
	acc->CalcBal(tra);
	acc->CalcReconBal(tra);
	uint16 i = 0;
	trs::iterator si = acc->traS.begin();
	while (si != p.first)
	{
		i++;
		si++;
	}

	return i;
}

void TraMan::ClearData()
{
	DeleteData();
	catS.erase(catS.begin(), catS.end());
	pyeS.erase(pyeS.begin(), pyeS.end());
	recDatL.erase(recDatL.begin(), recDatL.end());
	recDepL.erase(recDepL.begin(), recDepL.end());
	recPyeL.erase(recPyeL.begin(), recPyeL.end());
	recPamL.erase(recPamL.begin(), recPamL.end());
}

bool TraMan::DataExists()
{
	return !accL.empty();
}

void TraMan::DeleteData()
{
	while (!accL.empty())
	{
		delete accL.front();
		accL.pop_front();
	}
	for (cas::iterator iter = catS.begin();	iter != catS.end(); iter++)
		delete *iter;
	for (pas::iterator iter = pyeS.begin();	iter != pyeS.end(); iter++)
		delete *iter;
}

uint16 TraMan::DeleteTra(Acc* acc, Tra* tra)
{
	if (tra->num.type == 0)
	{
		acc->chkNumL.erase(tra->num.chknum);
		acc->MakeNxtChkL();
	}
	RemoveAmt(tra);
	trs::iterator iter = acc->traS.find(tra);
	delete *iter;
	uint16 i = 0;
	trs::iterator si = acc->traS.begin();
	while (*si != *iter)
	{
		i++;
		si++;
	}
	Tra* t = 0;
	if (si != acc->traS.begin())
	{
		si--;
		t = *si;
	}
	acc->traS.erase(iter);
	acc->GrayEveryOther(t);
	acc->CalcBal(t);
	acc->CalcReconBal(t);
	return i;
}

void TraMan::EditTra(Acc* acc, Tra* curT, 
	Tra* newT)
{
	RemoveAmt(curT);
	AddAmt(newT);
	
	curT->date = newT->date;
	curT->pam = newT->pam;
	curT->amt = newT->amt;
	curT->pye = newT->pye;
	if (!curT->tsf)
		if (curT->cat->spl)
			delete curT->cat;
	if (!newT->tsf)
	{
		if (newT->cat->spl)
		{
			curT->cat = new SplCat(*(SplCat*)newT->cat);
		}
		else
			curT->cat = newT->cat;
	}
	if (curT->num.type == 0)
	{
		acc->chkNumL.erase(curT->num.chknum);
		acc->MakeNxtChkL();
	}
	if (newT->num.type == 0)
	{
		acc->chkNumL.insert(newT->num.chknum);
		acc->MakeNxtChkL();
	}
	curT->num = newT->num;
	curT->mem = newT->mem;
	curT->tsf = newT->tsf;
	curT->tsfAcc = newT->tsfAcc;
	curT->ctrprt = newT->ctrprt;
	delete newT;
	recDatL.push_back(curT->date);
	recDatL.unique();
	if (recDatL.size() > numRecToKeep)
		recDatL.pop_front();
	if (!curT->tsf)
	{
		recPyeL.push_back(curT->pye);
		recPyeL.unique();
		if (recPyeL.size() > numRecToKeep)
			recPyeL.pop_front();
	}
	acc->CalcBal(curT);
	acc->CalcReconBal(curT);
}

list<Acc*>* TraMan::GetAccL()
{
	return &accL;
}

set<Cat*, NameSorted>* TraMan::GetCatS()
{
	return &catS;
}

list<uint16>* TraMan::GetNxtChkL(Acc* acc)
{
	return &acc->nxtChkL;
}

int32 TraMan::GetRecBal(Acc* acc)
{
	if (acc->traS.empty())
		return 0;
	return (*acc->traS.rbegin())->recBalFol;
}

list<time_t>* TraMan::GetRecDatL()
{
	return &recDatL;
}

list<uint32>* TraMan::GetRecDepL()
{
	return &recDepL;
}

list<Pye*>* TraMan::GetRecPyeL()
{
	return &recPyeL;
}

list<uint32>* TraMan::GetRecPamL()
{
	return &recPamL;
}

set<Pye*, NameSorted>* TraMan::GetPyeS()
{
	return &pyeS;
}

map<string, Acc*>* TraMan::GetStrAccM()
{
	return &strAccM;
}

set<Tra*, Tra>* TraMan::GetTraS(Acc* acc)
{
	return &acc->traS;
}

Prefs* TraMan::GetPrefs()
{
	return &prefs;
}

Acc* TraMan::NewAcc(string name)
{
	Acc* acc = new Acc(name);
	accL.push_back(acc);
	return acc;
}

bool TraMan::Open(BEntry* entry)
{
	ClearData();
	
	BFile openFile(entry, B_READ_ONLY);
	
	type_code tc;
	openFile.Read(&tc, sizeof(type_code));
	if (tc != B_ANY_TYPE)
		return false;
	
	uint8 version;
	openFile.Read(&version, sizeof(uint8));
	
	size_t st, st2;
	char* str;
	string curSym;
	bool curSymPlaceBefore;
	string curSep;
	string curDec;
	uint8 curDecPlace;
	uint8 dateFormat;
	string dateSep;
	if (version >= 1)
	{
		openFile.Read(&st, sizeof(size_t));
		str = new char[st + 1];
		openFile.Read(str, st);
		str[st] = '\0';
		curSym = str;
		openFile.Read(&curSymPlaceBefore, sizeof(uint8));
		openFile.Read(&st, sizeof(size_t));
		str = new char[st + 1];
		openFile.Read(str, st);
		str[st] = '\0';
		curSep = str;
		openFile.Read(&st, sizeof(size_t));
		str = new char[st + 1];
		openFile.Read(str, st);
		str[st] = '\0';
		curDec = str;
		openFile.Read(&curDecPlace, sizeof(uint8));
		openFile.Read(&dateFormat, sizeof(uint8));
		openFile.Read(&st, sizeof(size_t));
		str = new char[st + 1];
		openFile.Read(str, st);
		str[st] = '\0';
		dateSep = str;

		prefs.Set(curSym, curSymPlaceBefore, curSep, 
			curDec, curDecPlace, dateFormat, dateSep);
	}
	
	openFile.Read(&st, sizeof(size_t));
	while (catS.size() < st)
		catS.insert(new Cat(&openFile, &catS));
	
	openFile.Read(&st, sizeof(size_t));
	Pye* pye;
	for (size_t i = 0; i < st; i++)
	{
		openFile.Read(&st2, sizeof(size_t));
		str = new char[st2 + 1];
		openFile.Read(str, st2);
		str[st2] = '\0';
		pye = new Pye;
		pye->name = str;
		pyeS.insert(pye);
		delete[] str;
	}

	openFile.Read(&st, sizeof(size_t));
	time_t tt;
	for (size_t i = 0; i < st; i++)
	{
		openFile.Read(&tt, sizeof(time_t));
		recDatL.push_back(tt);
	}

	openFile.Read(&st, sizeof(size_t));
	uint32 ui32;
	for (size_t i = 0; i < st; i++)
	{
		openFile.Read(&ui32, sizeof(uint32));
		recDepL.push_back(ui32);
	}
	
	openFile.Read(&st, sizeof(size_t));
	Pye p;
	for (size_t i = 0; i < st; i++)
	{
		openFile.Read(&st2, sizeof(size_t));
		str = new char[st2 + 1];
		openFile.Read(str, st2);
		str[st2] = '\0';
		p.name = str;
		recPyeL.push_back(*pyeS.find(&p));
		delete[] str;
	}
	
	openFile.Read(&st, sizeof(size_t));
	for (size_t i = 0; i < st; i++)
	{
		openFile.Read(&ui32, sizeof(uint32));
		recPamL.push_back(ui32);
	}
	
	openFile.Read(&st, sizeof(size_t));
	for (size_t i = 0; i < st; i++)
	{
		accL.push_back(new Acc(&openFile, &pyeS, &catS, &prefs));
	}

	// Match up tsfs
	for (list<Acc*>::iterator iter = accL.begin(); iter !=
		accL.end(); iter++)
	{
		for (trs::iterator iter2 = 
			(*iter)->traS.begin(); iter2 != (*iter)->
			traS.end(); iter2++)
		{
/*			if ((*iter2)->num.type == 0)
			{
				(*iter)->chkNumL.insert((*iter2)->num.chknum);
			}
*/			if ((*iter2)->tsf)
			{
				(*iter2)->tsfAcc = *find_if(accL.begin(),
					accL.end(), SerialMatch<Acc*>((*iter2)->stsfAcc));
				(*iter2)->ctrprt = *find_if((*iter2)->tsfAcc->traS.
					begin(), (*iter2)->tsfAcc->traS.end(),
					SerialMatch<Tra*>((*iter2)->sctrprt));
			}
		}
		(*iter)->MakeNxtChkL();
	}

	return true;
}

void TraMan::PrefsUpdated(int8 decPlaceChange)
{
	bool neg = decPlaceChange < 0;
	uint8 absDecPlaceChange = decPlaceChange * (neg ? -1 : 1);
	for (acl::iterator iter1 = accL.begin(); iter1 != accL.end(); iter1++)
	{
		for (trs::iterator iter2 = (*iter1)->traS.begin(); iter2 !=
			(*iter1)->traS.end(); iter2++)
		{
			for (int8 i = 0; i < absDecPlaceChange; i++)
			{
				if (neg)
					(*iter2)->amt /= 10;
				else
					(*iter2)->amt *= 10;
				if (!(*iter2)->tsf && (*iter2)->cat->spl)
				{
					SplCat* sc = (SplCat*)(*iter2)->cat;
					for (i32l::iterator iter3 = sc->amtL->begin();
						iter3 != sc->amtL->end(); iter3++)
					{
						if (neg)
							*iter3 /= 10;
						else
							*iter3 *= 10;
					}
					sc->MakeName();
				}
			}
		}
		(*iter1)->CalcBal();
		(*iter1)->CalcReconBal();
	}
}

void TraMan::ReconFinish(Acc* acc)
{
	for (trs::iterator iter =
		acc->traS.begin(); iter != acc->traS.
		end(); iter++)
		if ((*iter)->rec == 1)
			(*iter)->rec = 2;
}

void TraMan::ReconTra(Acc* acc, Tra* tra)
{
	acc->CalcReconBal(tra);
}

void TraMan::RemoveAmt(Tra* tra)
{
	if (tra->tsf)
		return;
	if (tra->pam)
	{
		if (tra->cat->spl)
		{
			SplCat* sc = (SplCat*)tra->cat;
			i32l::iterator itera = sc->amtL->begin();
			for (cal::iterator iterc = sc->catL->begin();
				iterc != sc->catL->end(); iterc++)
			{
				(*iterc)->sum += *itera;
				itera++;
			}
		}
		else
		{
			tra->cat->sum += tra->amt;
		}
	}
	else
	{
		if (tra->cat->spl)
		{
			SplCat* sc = (SplCat*)tra->cat;
			i32l::iterator itera = sc->amtL->begin();
			for (cal::iterator iterc = sc->catL->begin();
				iterc != sc->catL->end(); iterc++)
			{
				(*iterc)->sum -= *itera;
				itera++;
			}
		}
		else
		{
			tra->cat->sum -= tra->amt;
		}
	}
}

void TraMan::ReplaceCat(Cat* icat, Cat* ncat)
{
	for (acl::iterator iter3 = accL.begin(); iter3 != accL.end(); iter3++)
		for (trs::iterator iter = (*iter3)->traS.begin(); iter != (*iter3)->traS.end(); iter++)
		{
			if (!(*iter)->tsf)
			{
				if ((*iter)->cat == icat)
				{
					RemoveAmt(*iter);
					(*iter)->cat = ncat;
					AddAmt(*iter);
				}
				else if ((*iter)->cat->spl)
				{
					RemoveAmt(*iter);
					cal::iterator iterT;
					SplCat* sc = (SplCat*)(*iter)->cat;
					cal::iterator iter2 = sc->catL->begin();
					while (iter2 != sc->catL->end())
					{
						if (*iter2 == icat)
						{
							iterT = iter2;
							sc->catL->insert(iter2, ncat);
							iter2++;
							sc->catL->erase(iterT);
						}	
						else
							iter2++;
					}
					AddAmt(*iter);
					sc->MakeName();
				}
			}
		}
}

void TraMan::ReplacePye(Pye* ipye, Pye* npye)
{
	for (acl::iterator iter3 = accL.begin(); iter3 != accL.end(); iter3++)
		for (trs::iterator iter = (*iter3)->traS.begin(); iter != (*iter3)->traS.end(); iter++)
		{
			if ((*iter)->pye == ipye)
				(*iter)->pye = npye;
		}
}

void TraMan::Save(BEntry* entry)
{
	entry_ref er;
	entry->GetRef(&er);
	BFile saveFile(&er, B_WRITE_ONLY);
	
	//Type Code for Endianess
	type_code tc = B_ANY_TYPE;
	saveFile.Write(&tc, sizeof(type_code));
	//File Version
	// 0 = first version
	// 1 = 0 + prefs
	uint8 ui8 = 1;
	saveFile.Write(&ui8, sizeof(uint8));
	
	// Version 1 Prefs Section -->
	size_t st;
	st = prefs.curSym.size();
	saveFile.Write(&st, sizeof(size_t));
	saveFile.Write(prefs.curSym.c_str(), st);
	saveFile.Write(&prefs.curSymPlaceBefore, sizeof(uint8));
	st = prefs.curSep.size();
	saveFile.Write(&st, sizeof(size_t));
	saveFile.Write(prefs.curSep.c_str(), st);
	st = prefs.curDec.size();
	saveFile.Write(&st, sizeof(size_t));
	saveFile.Write(prefs.curDec.c_str(), st);
	saveFile.Write(&prefs.curDecPlace, sizeof(uint8));
	saveFile.Write(&prefs.dateFormat, sizeof(uint8));
	st = prefs.dateSep.size();
	saveFile.Write(&st, sizeof(size_t));
	saveFile.Write(prefs.dateSep.c_str(), st);
	// <-- Version 1 Prefs Section 
	
	st = catS.size();
	saveFile.Write(&st, sizeof(size_t));
	for (cas::iterator iter = catS.begin(); iter !=
		catS.end(); iter++)
		if ((*iter)->IsRoot())
			(*iter)->Serialize(&saveFile);
	
	st = pyeS.size();
	saveFile.Write(&st, sizeof(size_t));
	for (pas::iterator iter = pyeS.begin(); iter !=
		pyeS.end(); iter++)
	{
		st = (*iter)->name.size();
		saveFile.Write(&st, sizeof(size_t));
		saveFile.Write((*iter)->name.c_str(), st);
	}
	
	st = recDatL.size();
	saveFile.Write(&st, sizeof(size_t));
	for (til::iterator iter = recDatL.begin(); iter !=
		recDatL.end(); iter++)
		saveFile.Write(&*iter, sizeof(time_t));
	
	st = recDepL.size();
	saveFile.Write(&st, sizeof(size_t));
	for (u32l::iterator iter = recDepL.begin(); iter !=
		recDepL.end(); iter++)
		saveFile.Write(&*iter, sizeof(uint32));
	
	st = recPyeL.size();
	saveFile.Write(&st, sizeof(size_t));
	for (pal::iterator iter = recPyeL.begin(); iter !=
		recPyeL.end(); iter++)
	{
		st = (*iter)->name.size();
		saveFile.Write(&st, sizeof(size_t));
		saveFile.Write((*iter)->name.c_str(), st);
	}
	
	st = recPamL.size();
	saveFile.Write(&st, sizeof(size_t));
	for (u32l::iterator iter = recPamL.begin(); iter !=
		recPamL.end(); iter++)
		saveFile.Write(&*iter, sizeof(uint32));
	
	st = accL.size();
	saveFile.Write(&st, sizeof(size_t));
	for (list<Acc*>::iterator iter = accL.begin(); iter !=
		accL.end(); iter++)
		(*iter)->Serialize(&saveFile);
}