#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <File.h>

#include "Cat.h"
#include "Numfield.h"
#include "Pye.h"
#include "SplCat.h"
#include "Tra.h"
#include "Version.h"

Tra::Tra()
{
	cat = 0;
}

Tra::Tra(string numtext, string datetext, Pye* pye,
	string pamtext, string deptext, Cat* cat, 
	string memtext, uint8 rec, bool tsf, Acc*
	tsfAcc, Prefs* prefs)
{
	#ifdef DEBUG
	printf("Tra::Tra - %p\n", this);
	#endif
	uint16 chknum = atoi(numtext.c_str());
	if (chknum > 0)
	{
		num.type = 0;
		num.chknum = chknum;
	}
	else
	{
		for (string::iterator si = numtext.begin();
			si != numtext.end(); si++)
		{
			*si = toupper(*si);
		}
		if (numtext == "ATM")
		{
			num.type = 1;
		}
		else if (numtext == "EFT")
		{
			num.type = 2;
		}
		else if (numtext == "XFER")
		{
			if (!tsf)
				throw invalid_argument("Not a transfer.  Can't Use Xfer");
			num.type = 3;
		}
		else if (numtext == "")
		{
			if (tsf)
				num.type = 3;
			else
				num.type = 4;
		}
		else
			throw invalid_argument("No Check Number.  Choose a Number, \"ATM\", \"EFT\", \"Xfer\", or leave blank");
	}
	date = prefs->stringToDate(datetext);
	this->pye = pye;
	if ((pamtext.length() > 0 && deptext.length() > 0) ||
		(pamtext.length() == 0 && deptext.length() == 0))
	{
		throw invalid_argument(
			"Invalid Transaction.  Must have either a deposit or payment.");
	}
//	size_t pos0;
	int32 amt_temp;
	if (pamtext.length() > 0)
	{
		amt_temp = prefs->stringToCurrency(pamtext);
		pam = true;
	}
	else
	{
		amt_temp = prefs->stringToCurrency(deptext);
		pam = false;
	}
	if (amt_temp < 0)
		throw invalid_argument("Invalid Transaction.  Amount cannot be negative.");
	amt = amt_temp;
	if (!tsf)
	{
		if (cat->spl)
			this->cat = new SplCat(*(SplCat*)cat);
		else
			this->cat = cat;
	}
	else
		this->cat = 0;
	mem = memtext;
	this->rec = rec;
	this->tsf = tsf;
	this->tsfAcc = tsfAcc;
}

Tra::Tra(BFile* file, set<Pye*, NameSorted>* pyeset,
	set<Cat*, NameSorted>* catset, Prefs* prefs)
{
	#ifdef DEBUG
		printf("Tra::Tra - %p\n", this);
	#endif

	file->Read(&sthis, sizeof(Tra*));
//	printf("\tsthis = %p\n", sthis);
	file->Read(&tsf, sizeof(bool));
//	printf("\ttsf = %i\n", tsf);
	file->Read(&stsfAcc, sizeof(Acc*));
//	printf("\tstsfAcc = %p\n", stsfAcc);
	file->Read(&sctrprt, sizeof(Tra*));
//	printf("\tsctrprt = %p\n", sctrprt);
	file->Read(&num.type, sizeof(uint8));
//	printf("\tnum.type = %i\n", num.type);
	if (num.type == 0)
	{
		file->Read(&num.chknum, sizeof(uint16));
//		printf("\tnum.chknum = %i\n", num.chknum);
	}
	file->Read(&date, sizeof(time_t));
//	printf("\tdate = %li\n", date);
	char* str;
	size_t st;
	if (!tsf)
	{
		file->Read(&st, sizeof(size_t));
//		printf("\tpye->name.size = %li\n", st);
		str = new char[st + 1];
		file->Read(str, st);
		str[st] = '\0';
		Pye p;
		p.name = str;
		set<Pye*, NameSorted>::iterator piter = pyeset->find(&p);
		pye = *piter;
//		printf("\tpye->name = %s\n", pye->name.c_str());
		delete[] str;
	}
	else
		pye = 0;
	file->Read(&amt, sizeof(uint32));
//	printf("\tamt = %li\n", amt);
	file->Read(&pam, sizeof(bool));
//	printf("\tpam = %i\n", pam);
	bool split;
	file->Read(&split, sizeof(bool));
//	printf("\tsplit = %i\n", split);
	if (!tsf && !split)
	{
		file->Read(&st, sizeof(size_t));
		str = new char[st + 1];
		file->Read(str, st);
		str[st] = '\0';
		Cat c(str);
		set<Cat*, NameSorted>::iterator citer = catset->find(&c);
		cat = *citer;
		delete[] str;
//		printf("\tcat->name = %s\n", cat->name.c_str());
	}
	else if (tsf)
		cat = 0;
	else if (split)
	{
		cat = new SplCat(file, catset, prefs);
//		printf("\tcat->name = %s\n", cat->name.c_str());
	}
	file->Read(&st, sizeof(size_t));
//	printf("\tmem.size = %li\n", mem.size());
	str = new char[st + 1];
	file->Read(str, st);
	str[st] = '\0';
	mem = str;
//	printf("\tmem = %s\n", mem.c_str());
	delete[] str;
	file->Read(&gry, sizeof(bool));
//	printf("\tgry = %i\n", gry);
	file->Read(&rec, sizeof(uint8));
//	printf("\trec = %i\n", rec);
}

Tra::~Tra()
{
	#ifdef DEBUG
	printf("Tra::~Tra\n");
	#endif
	if (!tsf)
		if (cat)
			if (cat->spl)
				delete cat;
}

int Tra::operator()(const Tra* t1, const Tra* t2) const
{
	if (t1->date != t2->date)
		return (t1->date < t2->date);
	if (t1->num.type != t2->num.type)
		return (t1->num.type < t2->num.type);
	if (t1->num.type == 0)
		return (t1->num.chknum < t2->num.chknum);
	return t1 < t2;
}

uint8 Tra::Month()
{
	tm* t = localtime(&date);
	return(t->tm_mon);
}

uint8 Tra::Year()
{
	tm* t = localtime(&date);
	return(t->tm_year);
}

void Tra::Serialize(BFile* file)
{
	#ifdef DEBUG
	printf("Tra::Serialize - %p\n", this);
	printf("\tthis = %p\n", this);
	printf("\ttsf = %i\n", tsf);
	printf("\ttsfAcc = %p\n", tsfAcc);
	printf("\tctrprt = %p\n", ctrprt);
	printf("\tnum.type = %i\n", num.type);
	if (num.type == 0)
		printf("\tnum.chknum = %i\n", num.chknum);
	printf("\tdate = %li\n", date);
	if (!tsf)
	{
		printf("\tpye->name.size() = %li\n", pye->name.size());
		printf("\tpye->name = %s\n", pye->name.c_str());
	}
	printf("\tamt = %li\n", amt);
	printf("\tpam = %i\n", pam);
	if (!tsf)
	{
		printf("\tcat->name.size() = %li\n", cat->name.size());
		printf("\tcat->name = %s\n", cat->name.c_str());
	}
	printf("\tmem.size = %li\n", mem.size());
	printf("\tmem = %s\n", mem.c_str());
	printf("\tgry = %i\n", gry);
	printf("\trec = %i\n", rec);
	#endif
	Tra* thisT = this;
	file->Write(&thisT, sizeof(Tra*));
	file->Write(&tsf, sizeof(bool));
	file->Write(&tsfAcc, sizeof(Acc*));
	file->Write(&ctrprt, sizeof(Tra*));
	file->Write(&num.type, sizeof(uint8));
	if (num.type == 0)
		file->Write(&num.chknum, sizeof(uint16));
	file->Write(&date, sizeof(time_t));
	size_t st;
	if (!tsf)
	{
		st = pye->name.size();
		file->Write(&st, sizeof(size_t));
		file->Write(pye->name.c_str(), st);
	}
	file->Write(&amt, sizeof(uint32));
	file->Write(&pam, sizeof(bool));
	bool spl = !tsf && cat->spl;
	file->Write(&spl, sizeof(bool));
	if (!tsf && !spl)
	{
		st = cat->Name().size();
		file->Write(&st, sizeof(size_t));
		file->Write(cat->Name().c_str(), st);
	}
	else if (spl)
	{
		cat->Serialize(file);
	}
	st = mem.size();
	file->Write(&st, sizeof(size_t));
	file->Write(mem.c_str(), st);
	file->Write(&gry, sizeof(bool));
	file->Write(&rec, sizeof(uint8));
}
