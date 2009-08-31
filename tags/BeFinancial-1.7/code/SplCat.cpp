#include <stdexcept>
#include <stdio.h>
#include <File.h>
#include "SplCat.h"

SplCat::SplCat(cal* catL, i32l* amtL, Prefs* prefs) 
	: Cat("Split")
{
	#ifdef DEBUG
	printf("SplCat::SplCat - %p\n", this);
	#endif
	this->catL = catL;
	this->amtL = amtL;
	spl = true;
	this->prefs = prefs;
	
	MakeName();
}

SplCat::SplCat(BFile* file, cas* catS, Prefs* prefs)
	 : Cat("Split")
{
	this->prefs = prefs;

	catL = new cal;
	amtL = new i32l;
//	printf("SplCat::SplCat\n");
	size_t st;
	file->Read(&st, sizeof(size_t));
//	printf("\tst = %li\n", st);
	char* str = new char[st + 1];
	file->Read(str, st);
	str[st] = '\0';
	name = str;
//	printf("\tstr = %s\n", str);
	delete[] str;
	file->Read(&sum, sizeof(int32));
//	printf("\tsum = %li\n", sum);
	file->Read(&spl, sizeof(bool));
//	printf("\tspl = %i\n", spl);
	file->Read(&st, sizeof(size_t));
//	printf("\tst = %li\n", st);
	size_t st2;
	Cat* c;
	for (size_t i = 0; i < st; i++)
	{
		file->Read(&st2, sizeof(size_t));
//		printf("\tst2 = %li\n", st2);
		str = new char[st2 + 1];
		str[st2] = '\0';
		file->Read(str, st2);
//		printf("\tstr = %s\n", str);
		c = new Cat(str);
//		printf("%p\n", *catS->find(c));
		catL->push_back(*catS->find(c));
		delete c;
	}
	int32 i32;
	for (size_t i = 0; i < st; i++)
	{
		file->Read(&i32, sizeof(int32));
		amtL->push_back(i32);
	}

	MakeName();
}

SplCat::SplCat(const SplCat& sc) : Cat("Split")
{
	catL = new cal(*sc.catL);
	amtL = new i32l(*sc.amtL);
	spl = true;
	prefs = sc.prefs;

	MakeName();
}

SplCat::~SplCat()
{
	#ifdef DEBUG
	printf("SplCat::~SplCat - %p\n", this);
	#endif
	delete catL;
	delete amtL;
}

void SplCat::AddChild(Cat* child)
{
	throw invalid_argument("Can't do this - not a normal Cat");
}

list<Cat*>* SplCat::ListOfChildren()
{
	throw invalid_argument("Can't do this - not a normal Cat");
}

void SplCat::MakeName()
{
//	printf("SplCat::MakeName\n");
	string n = "Split: ";
	i32l::iterator i1 = amtL->begin();
	for (cal::iterator i2 = catL->begin(); i2 != catL->end(); i2++)
	{
		n += (*i2)->Name() + " ";
		n += prefs->currencyToString(*i1);
		i1++;
		if (i1 != amtL->end())
			n += ", ";
	}
	leafname = n;
}

void SplCat::Serialize(BFile* file)
{
	#ifdef DEBUG
	printf("SplCat::Serialize\n");
	#endif
	size_t st = name.size();
	file->Write(&st, sizeof(size_t));
	file->Write(name.c_str(), st);
	file->Write(&sum, sizeof(int32));
	file->Write(&spl, sizeof(bool));
	st = catL->size();
	file->Write(&st, sizeof(size_t));
	for (cal::iterator iter = catL->begin();
		iter != catL->end(); iter++)
	{
		size_t st = (*iter)->Name().size();
		file->Write(&st, sizeof(size_t));
		file->Write((*iter)->Name().c_str(), st);
	}
	for (i32l::iterator iter = amtL->begin();
		iter != amtL->end(); iter++)
	{
		file->Write(&*iter, sizeof(int32));
	}
}