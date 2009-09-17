#ifndef CatH
#define CatH

#include <SupportDefs.h>
#include <set>
#include <string>

#include "NameSorted.h"

class BFile;

class Cat : public NameSorted
{
protected:
	typedef set<Cat*, NameSorted> cas;
	cas children;
	Cat* par;
	string leafname;
public:
	bool spl;
	int32 sum;
	Cat(string name, Cat* par = 0);
	Cat(BFile* file, cas* catS, Cat* par = 0);
	virtual ~Cat() {};
	virtual void AddChild(Cat* child);
	virtual cas* SetOfChildren();
	bool IsRoot();
	string LeafName();
	string Name() const;
	Cat* Parent();
	virtual void Serialize(BFile* file);
	void SetParent(Cat* parent);
};

#endif
