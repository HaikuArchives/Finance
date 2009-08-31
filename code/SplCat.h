#include <list>
#include "Cat.h"
#include "Prefs.h"

class SplCat : public Cat
{
	Prefs* prefs;
public:
	typedef list<Cat*> cal;
	typedef set<Cat*, NameSorted> cas;
	typedef list<int32> i32l;
	cal* catL;
	i32l* amtL;
	SplCat(cal* catL, i32l* amtL, Prefs* prefs);
	SplCat(BFile* file, cas* catS, Prefs* prefs);
	SplCat(const SplCat& sc);
	~SplCat();
	void AddChild(Cat* child);
	cal* ListOfChildren();
	void MakeName();
	void Serialize(BFile* file);
};
