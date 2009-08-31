#include <algorithm>
#include <stdexcept>
#include <stdio.h>
#include <Path.h>
#include "Acc.h"
#include "Cat.h"
#include "MainView.h"
#include "Proxy.h"
#include "Pye.h"
#include "SplCat.h"
#include "StringInputWindow.h"
#include "Tra.h"
#include "TraMan.h"
#include "Version.h"

void Proxy::AccUpdated(Acc* acc)
{
	tm->AccUpdated(acc);
	mv->AccUpdated(acc);
}

Cat* Proxy::AddCat(string name, bool update = true)
{
	Cat* cat = tm->AddCat(name);
	if (update)
		mv->CatLChanged();
	return cat;
}

void Proxy::AddPye(Pye* pye, bool update = true)
{
	tm->AddPye(pye);
	if (update)
		mv->PyeLChanged();
}

void Proxy::AddTra(Acc* acc, Tra* tra)
{
	uint16 pos = tm->AddTra(acc, tra);
	#ifdef DEBUG
	printf("tm->AddTra\n");
	#endif
	mv->AddTra(tra, pos);
	#ifdef DEBUG
	printf("mv->AddTra\n");
	#endif
	if (tra->tsf)
	{
		pair<uint16, Tra*> p = tm->AddCtrprtTra(tra, acc);
		mv->AddTra(tra->tsfAcc, p.second, p.first);
	}
}

void Proxy::BMCImport(Acc* acc, BEntry* entry)
{
	BPath path(entry);
	ifstream bmcFile(path.Path());
	
	uint16 ui16, ui16_2;
	uint16 numAccs;
	string str;
	
	bmcFile >> ui16;
	bmcFile >> ui16;
	bmcFile >> ui16;
	
	for (uint16 i = 0; i < ui16; i++)
	{
		str = GetStrFromIfstream(&bmcFile);
		bmcFile >> ui16_2;
		for (uint16 j = 0; j < ui16_2; j++)
		{
			str = GetStrFromIfstream(&bmcFile);
		}
	}
	
	bmcFile >> numAccs;
	
	for (uint16 i = 0; i < numAccs * 8 + 1; i++)
	{
		str = GetStrFromIfstream(&bmcFile);
	}
	
	list<string> accNames;
	list<uint16> numTrans;
	for (uint16 i = 0; i < numAccs; i++)
	{
		str = GetStrFromIfstream(&bmcFile);
		accNames.push_back(str);
		for (uint8 j = 0; j < 8; j++)
		{
			str = GetStrFromIfstream(&bmcFile);
		}
		bmcFile >> ui16;
		numTrans.push_back(ui16);
		for (uint8 j = 0; j < 2; j++)
		{
			str = GetStrFromIfstream(&bmcFile);
		}
	}

	string accToImp = mv->GetStrChoice(&accNames, "", acc);
	if (accToImp == "")
		return;

	uint16 type;
	string catS, subCatS, tsfAccS, amtS, dateS, memS, numS, pyeS;
	list<string>::iterator iter2 = accNames.begin();
	for (list<uint16>::iterator iter = numTrans.begin(); iter != numTrans.end(); iter++)
	{
		for (uint16 i = 0; i < *iter; i++)
		{
			amtS = GetStrFromIfstream(&bmcFile);
			bmcFile >> type;
			dateS = GetStrFromIfstream(&bmcFile);
			str = GetStrFromIfstream(&bmcFile);
			str = GetStrFromIfstream(&bmcFile);
			numS = GetStrFromIfstream(&bmcFile);
			pyeS = GetStrFromIfstream(&bmcFile);
			catS = GetStrFromIfstream(&bmcFile);
			subCatS = GetStrFromIfstream(&bmcFile);

			if (subCatS != "")
				catS = catS + ":" + subCatS;

			memS = GetStrFromIfstream(&bmcFile);
			str = GetStrFromIfstream(&bmcFile);
			tsfAccS = GetStrFromIfstream(&bmcFile);

			if (*iter2 == accToImp)
			{
				bool tsf = false;
				Acc* tsfAcc = 0;
				Cat* cat = 0;
				Pye* pye = 0;
				string pamtext, deptext;
				if (type == 3 || type == 4)
				{
					tsf = true;
					stacm::iterator iter3 = tm->GetStrAccM()->find(catS);
					if (iter3 == tm->GetStrAccM()->end())
					{
						tsfAcc = mv->GetAccMapping(tsfAccS, acc);
						if (tsfAcc == 0)
							goto skip;
						tm->GetStrAccM()->insert(pair<string, Acc*>(tsfAccS, tsfAcc));
					}
					else
						tsfAcc = (*iter3).second;

					if (type == 3)
					{
						pamtext = amtS;
						deptext = "";
					}
					else
					{
						deptext = amtS;
						pamtext = "";
					}
					numS = "XFER";
				}
				else
				{
					Cat c("");
					cas::iterator csi;
					c.name = catS;
					csi = tm->GetCatS()->find(&c);
					if (csi == tm->GetCatS()->end())
						cat = AddCat(catS, false);
					else
						cat = *csi;
					Pye p;
					p.name = pyeS;
					pas::iterator psi = tm->GetPyeS()->find(&p);
					if (psi == tm->GetPyeS()->end())
					{
						pye = new Pye;
						pye->name = pyeS;
						AddPye(pye, false);
					}
					else
						pye = *psi;
					
					if (type == 0 || type == 2)
					{
						pamtext = amtS;
						deptext = "";
					}
					else
					{
						deptext = amtS;
						pamtext = "";
					}
					if (numS != "ATM" && numS != "EFT" && atoi(numS.c_str()) == 0)
						numS = "";
				}
				try
				{
					Tra* t = new Tra(numS, dateS, pye, pamtext, deptext, 
						cat, memS, 0, tsf, tsfAcc, GetPrefs());
					AddTra(acc, t);
				}
				catch(invalid_argument ia)
				{
				}
			}
skip:;
		}
		iter2++;
	}
	
	bmcFile.close();
}

void Proxy::ClearData()
{
	mv->ClearViews();
	tm->ClearData();
}

bool Proxy::DataExists()
{
	return tm->DataExists();
}

void Proxy::DeleteCat(Cat* cat, Cat* ncat)
{
	if (GetCatS()->find(cat) != GetCatS()->end())
	{
		GetCatS()->erase(cat);
		if (!cat->IsRoot())
		{
			cat->Parent()->SetOfChildren()->erase(cat);
		}
		for (cas::iterator iter = cat->SetOfChildren()->begin(); iter != cat->SetOfChildren()->end(); iter++)
		{
			ncat->SetOfChildren()->insert(*iter);
			(*iter)->SetParent(ncat);
		}
	}
	tm->ReplaceCat(cat, ncat);
	delete cat;
	mv->CatLChanged();
	mv->RegItemsChanged();
}

void Proxy::DeletePye(Pye* pye, Pye* npye)
{
	if (GetPyeS()->find(pye) != GetPyeS()->end())
	{
		GetPyeS()->erase(pye);
	}
	list<Pye*>::iterator iter = find(GetRecPyeL()->begin(), GetRecPyeL()->end(), pye);
	if (iter != GetRecPyeL()->end())
	{
		GetRecPyeL()->erase(iter);
	}
	tm->ReplacePye(pye, npye);
	delete pye;
	mv->PyeLChanged();
	mv->RegItemsChanged();
}

void Proxy::DeleteTra(Acc* acc, Tra* tra)
{
	uint16 pos;
	if (tra->tsf)
	{
		Acc* a = tra->tsfAcc;
		pos = tm->DeleteTra(a, tra->ctrprt);
		mv->DeleteTra(a, pos);
	}	
	pos = tm->DeleteTra(acc, tra);
	mv->DeleteTra(pos);
}

void Proxy::EditTra(Acc* acc, Tra* curT, 
	Tra* newT)
{
	uint16 pos;
	//Was a transfer to account A, still a transfer to account A
	bool editCtrTra = curT->tsf && newT->tsf && newT->tsfAcc == curT->tsfAcc;
	//Was a transfer, is not now
	if (curT->tsf && !newT->tsf)
	{
		Acc* a = curT->tsfAcc;
		pos = tm->DeleteTra(a, curT->ctrprt);
		mv->DeleteTra(a, pos);
	}
	//Was not a transfer, is now
	else if (!curT->tsf && newT->tsf)
	{
		pair<uint16, Tra*> p = tm->AddCtrprtTra(newT, acc);
		mv->AddTra(newT->tsfAcc, p.second, p.first);
	}
	//Was a transfer to account A, now a transfer to account B
	else if (curT->tsf && newT->tsf && newT->tsfAcc !=
		curT->tsfAcc)
	{
		Acc* a = curT->tsfAcc;
		pos = tm->DeleteTra(a, curT->ctrprt);
		mv->DeleteTra(a, pos);

		pair<uint16, Tra*> p = tm->AddCtrprtTra(newT, acc);
		mv->AddTra(newT->tsfAcc, p.second, p.first);
	}
	else
		newT->ctrprt = curT->ctrprt;
	tm->EditTra(acc, curT, newT);
	mv->EditTra();
	if (curT->tsf)
	{
		curT->ctrprt->amt = curT->amt;
		curT->ctrprt->pam = !curT->pam;
		curT->ctrprt->date = curT->date;
		curT->ctrprt->mem = curT->mem;
		curT->tsfAcc->CalcBal(curT->ctrprt);
		curT->tsfAcc->CalcReconBal(curT->ctrprt);
	}
	if (editCtrTra)
		mv->EditTra(curT->tsfAcc);
}

void Proxy::Export(Acc* acc, BEntry* entry)
{
	BPath path(entry);
	ofstream qifFile(path.Path());

	char buffer[100];
	struct tm* timestruct;
	uint8 cents;
	map<Acc*, string> acstrM;

	qifFile << "!Type:Bank\n";
	for (trs::iterator iter = acc->traS.begin(); iter != acc->traS.end(); iter++)
	{
		timestruct = localtime(&(*iter)->date);
		strftime(buffer, 20, "%m/%d/%y", timestruct);
		qifFile << "D" << buffer << "\n";
		cents = (*iter)->amt % 100;
		sprintf(buffer, "%li.%s%i", (*iter)->amt / 100, 
			cents < 10 ? "0" : "", cents);
		qifFile << "T" << ((*iter)->pam ? "-" : "") << buffer << "\n";
		if ((*iter)->num.type == 0)
			sprintf(buffer, "%i", (*iter)->num.chknum);
		else if ((*iter)->num.type == 1)
			strcpy(buffer, "ATM");
		else if ((*iter)->num.type == 2)
			strcpy(buffer, "EFT");
		else if ((*iter)->num.type == 3)
			strcpy(buffer, "TXFR");
		else if ((*iter)->num.type == 4)
			strcpy(buffer, "");
		qifFile << "N" << buffer << "\n";
		if ((*iter)->tsf)
			strcpy(buffer, ("Transfer with: " + (*iter)->tsfAcc->name).c_str());
		else
			strcpy(buffer, (*iter)->pye->name.c_str());
		qifFile << "P" << buffer << "\n";
		if ((*iter)->tsf)
		{
			string acname = "";
			if (acstrM.find((*iter)->tsfAcc) != acstrM.end())
			{
				acname = acstrM[(*iter)->tsfAcc];
			}
			else
			{
				StringInputWindow* siw = new StringInputWindow(("Export Name for " + 
					(*iter)->tsfAcc->name).c_str(), 0, 0, (*iter)->tsfAcc->name, true);
				acname = siw->GetString();
				acstrM.insert(pair<Acc*, string>((*iter)->tsfAcc, acname));
			}
			qifFile << "L[" << acname << "]\n";
		}
		else if (!(*iter)->cat->spl)
		{
			qifFile << "L" << (*iter)->cat->Name() << "\n";
		}
		else
		{
			SplCat* sc = (SplCat*)(*iter)->cat;
			cal::iterator citer = sc->catL->begin();
			i32l::iterator iiter = sc->amtL->begin();
			while (citer != sc->catL->end())
			{
				qifFile << "S" << (*citer)->Name() << "\n";
				cents = -*iiter % 100;
				sprintf(buffer, "%li.%s%i", *iiter / 100, 
					cents < 10 ? "0" : "", cents);
				qifFile << "$" << ((*iter)->pam ? "-" : "") << buffer << "\n";
				citer++;
				iiter++;
			}
		}
		qifFile << "M" << (*iter)->mem << "\n";
		qifFile << "^\n";
	}
}

list<Acc*>* Proxy::GetAccL()
{
	return tm->GetAccL();
}

set<Cat*, NameSorted>* Proxy::GetCatS()
{
	return tm->GetCatS();
}

list<uint16>* Proxy::GetNxtChkL(Acc* acc)
{ 
	return tm->GetNxtChkL(acc);
}

Prefs* Proxy::GetPrefs()
{
	return tm->GetPrefs();
}

set<Pye*, NameSorted>* Proxy::GetPyeS()
{
	return tm->GetPyeS();
}

int32 Proxy::GetRecBal(Acc* acc)
{
	return tm->GetRecBal(acc);
}

list<time_t>* Proxy::GetRecDatL()
{
	return tm->GetRecDatL();
}

list<uint32>* Proxy::GetRecDepL()
{
	return tm->GetRecDepL();
}

list<Pye*>* Proxy::GetRecPyeL()
{
	return tm->GetRecPyeL();
}

list<uint32>* Proxy::GetRecPamL()
{
	return tm->GetRecPamL();
}

string Proxy::GetStrFromIfstream(ifstream* stream)
{
	string str;
	char c;
	
	c = stream->get();
	if (c == 9)
		c = stream->get();
	while (!stream->eof() && c != 9)
	{
		str += c;
		c = stream->get();
	}
	
	if (c == 9)
		stream->seekg(-1, ios::cur);
	
	return str;
}

set<Tra*, Tra>* Proxy::GetTraSet(Acc* acc)
{
	return tm->GetTraS(acc);
}

void Proxy::Import(Acc* acc, BEntry* entry)
{
	BPath path(entry);
	ifstream qifFile(path.Path());

	pair<char, string> p;
	
	p = QifParse(&qifFile);
	if (p.first != '!')
		throw invalid_argument("Not a QIF file");
	
	bool dat = false;
	bool amt = false;
	bool num = false;
	bool pye = false;
	bool cat = false;
	bool mem = false;

	string datS;
	string amtS;
	string numS;
	string pyeS;
	string catS;
	string memS;

	list<string> splCat;
	list<string> splAmt;

	while (!qifFile.eof())
	{
		p = QifParse(&qifFile);
		switch (p.first)
		{
			case 'D':
				dat = true;
				datS = p.second;
			break;
			case 'T':
				amt = true;
				amtS = p.second;
			break;
			case 'N':
				num = true;
				numS = p.second;
			break;
			case 'P':
				pye = true;
				pyeS = p.second;
			break;
			case 'L':
				cat = true;
				catS = p.second;
			break;
			case 'M':
				mem = true;
				memS = p.second;
			break;
			case 'S':
				splCat.push_back(p.second);
			break;
			case '$':
				splAmt.push_back(p.second);
			break;
			case '^':
			{
				Acc* tsfAcc = 0;
				bool tsf = false;
				if (pyeS == "Opening Balance")
				{
					if (mv->GetOpenBalChange(amtS))
					{
						size_t pos0;
						while ((pos0 = amtS.find_first_of("$,")) != string::npos)
						{
							amtS.erase(pos0, 1);
						}
						float val = atof(amtS.c_str());
						acc->openBal = (int32)(100 * (val + (val < 0 ? -1 : 1) * .005));
					}
					goto cleanup;
				}
				if (dat && amt && pye)
				{
					if (!cat)
						catS = "Unclass";
					if (numS == "TXFR" || catS[0] == '[')
					{
						numS = "XFER";
						catS = catS.substr(1, catS.size() - 2);
						stacm::iterator iter = tm->GetStrAccM()->find(catS);
						if (iter == tm->GetStrAccM()->end())
						{
							tsfAcc = mv->GetAccMapping(catS, acc);
							if (tsfAcc == 0)
								goto cleanup;
							tsf = true;
							tm->GetStrAccM()->insert(pair<string, Acc*>(catS, tsfAcc));
						}
						else
							tsfAcc = (*iter).second;
					}
					else if (numS != "ATM" && numS != "EFT" && atoi(numS.c_str()) == 0)
						numS = "";
					Pye* pye = 0;
					if (!tsf)
					{
						Pye p;
						p.name = pyeS;
						pas::iterator psi = tm->GetPyeS()->find(&p);
						if (psi == tm->GetPyeS()->end())
						{
							pye = new Pye;
							pye->name = pyeS;
							AddPye(pye, false);
						}
						else
							pye = *psi;
					}
					string pamtext, deptext;
					if (amtS[0] == '-')
					{
						pamtext = amtS.substr(1, amtS.size() - 1);
						deptext = "";
					}
					else
					{
						pamtext = "";
						deptext = amtS;
					}
					Cat* cat = 0;
					if (!tsf)
					{
						Cat c("");
						cas::iterator csi;
						if (splCat.empty())
						{
							c.name = catS;
							csi = tm->GetCatS()->find(&c);
							if (csi == tm->GetCatS()->end())
								cat = AddCat(catS, false);
							else
								cat = *csi;
						}
						else
						{
							cal* catL = new cal;
							i32l* i32L = new i32l;
							for (list<string>::iterator iter = splCat.begin(); iter != splCat.end(); iter++)
							{
								c.name = *iter;
								csi = tm->GetCatS()->find(&c);
								if (csi == tm->GetCatS()->end())
									catL->push_back(AddCat(*iter, false));
								else
									catL->push_back(*csi);
							}	
							for (list<string>::iterator iter = splAmt.begin(); iter != splAmt.end(); iter++)
							{
								size_t pos0;
								while ((pos0 = (*iter).find_first_of(",")) != string::npos)
								{
									(*iter).erase(pos0, 1);
								}
								float val = atof((*iter).c_str());
								if (amtS[0] == '-')
									i32L->push_back(-(int32)(100 * (val + (val < 0 ? -1 : 1) * .005)));
								else
									i32L->push_back((int32)(100 * (val + (val < 0 ? -1 : 1) * .005)));
							}
							cat = new SplCat(catL, i32L, GetPrefs());
						}
					}
					if (!mem)
						memS = "";
#ifdef DEBUG
					printf("%s\n%s\n%s\n%s\n%s\n", numS.c_str(), datS.c_str(), pamtext.c_str(), 
						deptext.c_str(), memS.c_str());
					printf("%p\n%p\n%i\n%p\n", pye, cat, tsf, tsfAcc);
#endif
					try
					{
						Tra* t = new Tra(numS, datS, pye, pamtext, deptext, cat, 
							memS, 0, tsf, tsfAcc, GetPrefs());
						AddTra(acc, t);
					}
					catch(invalid_argument ia)
					{
					}
				}

cleanup:
				dat = false;
				amt = false;
				num = false;
				pye = false;
				cat = false;
				mem = false;
				numS = "";
				
				if (!splCat.empty())
					splCat.erase(splCat.begin(), splCat.end());
				if (!splAmt.empty())
					splAmt.erase(splAmt.begin(), splAmt.end());
			}
			break;
			default:
			break;
		}
	}
	
	qifFile.close();
	mv->NumLChanged();
	mv->PyeLChanged();
	mv->CatLChanged();
}

void Proxy::NewAcc(string name)
{
	Acc* acc = tm->NewAcc(name);
	mv->NewAcc(acc);
}

bool Proxy::Open(BEntry* entry)
{
	#ifdef DEBUG
	printf("Proxy::Open\n");
	#endif
	if (!tm->Open(entry))
		return false;
	mv->ClearViews();
	uint16 pos;
	for (acl::iterator iter = tm->GetAccL()->begin(); iter != tm->GetAccL()->end(); iter++)
	{
		pos = 0;
		mv->NewAcc(*iter);
		for (trs::iterator iter2 = (*iter)->traS.begin(); iter2 != (*iter)->traS.
			end(); iter2++)
		{
			mv->AddTra(*iter, *iter2, pos);
			pos++;
		}
	}
	
	return true;
}

void Proxy::PrefsUpdated(int8 decPlaceChange)
{
	tm->PrefsUpdated(decPlaceChange);
	mv->PrefsUpdated();
}

pair<char, string> Proxy::QifParse(ifstream* qifFile)
{
	char ch;
	string str;
	
	(*qifFile) >> ch;
	if (qifFile->eof())
		return pair<char, string>('\0', string("-"));
	if (ch != '^')
	{
		char chstr[100];
		qifFile->getline(chstr, 100, '\n');
		str = chstr;
		if (str[str.size() - 1] == '\r')
			str = str.substr(0, str.size() - 1);
	}
	else
	{
		qifFile->ignore(1);
		str = "-";
	}

#ifdef DEBUG
	printf("'%s'\n", str.c_str());
#endif
	return pair<char,string>(ch, str);
}

void Proxy::ReconFinish(Acc* acc)
{
	tm->ReconFinish(acc);
}

void Proxy::ReconTra(Acc* acc, Tra* t)
{
	tm->ReconTra(acc, t);
	mv->SetRecBal(tm->GetRecBal(acc));
}

void Proxy::Save(BEntry* entry)
{
	tm->Save(entry);
}

void Proxy::SetMainView(MainView* mv)
{
	this->mv = mv;
}

void Proxy::SetTraMan(TraMan* tm)
{
	this->tm = tm;
}
