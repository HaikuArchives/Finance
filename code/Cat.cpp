#include <stdio.h>
#include <File.h>
#include "Cat.h"

Cat::Cat(string leafname, Cat* par)
{
	#ifdef DEBUG
	printf("Cat::Cat - %p\n", this);
	#endif
	this->par = par;
	this->leafname = leafname;
	sum = 0;
	spl = false;
	name = Name();
	if (par)
		par->AddChild(this);
}

Cat::Cat(BFile* file, cas* catS, Cat* par)
{
	this->par = par;
	size_t st;
	file->Read(&st, sizeof(size_t));
	char* str = new char[st + 1];
	file->Read(str, st);
	str[st] = '\0';
	name = str;
	size_t st2 = name.find_last_of(':');
	if (st2 != string::npos)
		leafname = name.substr(st2 + 1, name.size() - st2 - 1);
	else
		leafname = name;
	delete[] str;
	file->Read(&sum, sizeof(int32));
	file->Read(&spl, sizeof(bool));
	file->Read(&st, sizeof(size_t));
	Cat* child;
	for (size_t i = 0; i < st; i++)
	{
		child = new Cat(file, catS, this);
		children.insert(child);
		catS->insert(child);
	}
}

void Cat::AddChild(Cat* child)
{
	#ifdef DEBUG
	printf("AddChild - %s\n", this->name.c_str());
	#endif
	children.insert(child);
}

set<Cat*, NameSorted>* Cat::SetOfChildren()
{
	return &children;
}

bool Cat::IsRoot()
{
	return (par == 0);
}

string Cat::LeafName()
{
	return leafname;
}

string Cat::Name() const
{
	if (par == 0)
		return leafname;
	return par->Name() + ":" + leafname;
}

Cat* Cat::Parent()
{
	return par;
}

void Cat::Serialize(BFile* file)
{
	size_t st = name.size();
	#ifdef DEBUG
	printf("Cat::Serialize\n");
	printf("\tthis = %p\n", this);
	printf("\tname.size() = %li\n", st);
	#endif
	file->Write(&st, sizeof(size_t));
	file->Write(name.c_str(), name.size());
	file->Write(&sum, sizeof(int32));
	file->Write(&spl, sizeof(bool));
	st = children.size();
	file->Write(&st, sizeof(size_t));
	#ifdef DEBUG
	printf("\tname = %s\n", name.c_str());
	printf("\tsum = %li\n", sum);
	printf("\tspl = %i\n", spl);
	printf("\tchildren.size = %li\n", st);
	#endif
	for (set<Cat*, NameSorted>::iterator iter = children.begin();
		iter != children.end(); iter++)
		(*iter)->Serialize(file);
}

void Cat::SetParent(Cat* parent)
{
	par = parent;
}
