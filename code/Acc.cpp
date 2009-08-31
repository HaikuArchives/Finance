#include <stdio.h>
#include <File.h>
#include "Acc.h"
#include "Version.h"

Acc::Acc(string name)
{
	#ifdef DEBUG
	printf("Acc::Acc - %p\n", this);
	#endif
	this->name = name;
	MakeNxtChkL();
	//Temporary
	openBal = 0;
	//Temporary
}

Acc::Acc(BFile* file, pas* pyeS,
	cas* catS, Prefs* prefs)
{
	#ifdef DEBUG
	printf("Acc::Acc - %p\n", this);
	#endif
	file->Read(&sthis, sizeof(Acc*));
	size_t st;
	file->Read(&st, sizeof(size_t));
	char* str = new char[st + 1];
	file->Read(str, st);
	str[st] = '\0';
	name = str;
	delete[] str;
	file->Read(&openBal, sizeof(int32));
	file->Read(&st, sizeof(size_t));
	uint16 ui16;
	for (size_t i = 0; i < st; i++)
	{
		file->Read(&ui16, sizeof(uint16));
		chkNumL.insert(ui16);
	}
	file->Read(&st, sizeof(size_t));
	for (size_t i = 0; i < st; i++)
	{
		traS.insert(new Tra(file, pyeS,
			catS, prefs));
	}
	CalcBal();
	CalcReconBal();
	GrayEveryOther();
}

Acc::~Acc()
{
	#ifdef DEBUG
	printf("Acc::~Acc\n");
	#endif
	for(trs::iterator iter = 
		traS.begin(); iter != traS.end(); iter++)
	{
		delete *iter;
	}
}

void Acc::CalcBal(Tra* startTra = 0)
{
	trs::iterator titer;
	int32 balance;
	if (startTra)
	{
		titer = traS.find(startTra);
		if (titer != traS.begin())
		{
			titer--;
			balance = (*titer)->balFol;
			titer++;
		}
		else
			balance = openBal;
	}
	else
	{
		titer = traS.begin();
		balance = openBal;
	}	
	for (; titer !=	traS.end(); titer++)
	{
		balance += ((*titer)->pam ? -1 : 1) * (*titer)->amt;
		(*titer)->balFol = balance;
	}
}

void Acc::CalcReconBal(Tra* startTra = 0)
{
	trs::iterator titer;
	int32 balance;
	if (startTra)
	{
		titer = traS.find(startTra);
		if (titer != traS.begin())
		{
			titer--;
			balance = (*titer)->recBalFol;
			titer++;
		}
		else
			balance = openBal;
	}
	else
	{
		titer = traS.begin();
		balance = openBal;
	}	
	for (; titer !=	traS.end(); titer++)
	{
		if ((*titer)->rec)
			balance += ((*titer)->pam ? -1 : 1) * (*titer)->amt;
		(*titer)->recBalFol = balance;
	}
}

void Acc::GrayEveryOther(Tra* startTra = 0)
{
#ifdef DEBUG
	printf("GrayEveryOther - %p\n", startTra);
#endif
	trs::iterator titer;
	bool flipflop = true;
	if (startTra)
	{	
		titer = traS.find(startTra);
		if (titer != traS.begin())
		{
			titer--;
			flipflop = !(*titer)->gry;
			titer++;
		}
	}
	else
		titer = traS.begin();
	for (; titer !=
		traS.end(); titer++)
	{
		(*titer)->gry = flipflop;
		flipflop = !flipflop;
	}
}

void Acc::MakeNxtChkL()
{
	nxtChkL.clear();
	uint16 lastcheck = 100;
	if (chkNumL.size() > 0)
	{
		lastcheck = *chkNumL.begin();
		u16s::iterator cli = chkNumL.begin()++;
		for (; cli != chkNumL.end(); cli++)
		{
			uint16 i = 0;
			for (i = lastcheck + 1; i < *cli; i++)
			{
				nxtChkL.push_back(i);
				if (nxtChkL.size() > 4)
					break;
			}
			if (nxtChkL.size() > 4)
				break;
			lastcheck = *cli;
		}
	}
	while (nxtChkL.size() < 5)
	{
		nxtChkL.push_back(++lastcheck);
	}
}

void Acc::Serialize(BFile* file)
{
	Acc* thisA = this;
	file->Write(&thisA, sizeof(Acc*));
	size_t st = name.size();
	file->Write(&st, sizeof(size_t));
	file->Write(name.c_str(), st);
	file->Write(&openBal, sizeof(int32));
	st = chkNumL.size();
	file->Write(&st, sizeof(size_t));
	for (u16s::iterator iter = chkNumL.begin();
		iter != chkNumL.end(); iter++)
		file->Write(&*iter, sizeof(uint16));
	st = traS.size();
	file->Write(&st, sizeof(size_t));
	for (trs::iterator iter = traS.begin();
		iter != traS.end(); iter++)
		(*iter)->Serialize(file);
}