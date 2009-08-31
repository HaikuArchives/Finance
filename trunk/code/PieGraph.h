#include <list>
#include <set>
#include <View.h>
#include "ColorMaker.h"
#include "NameSorted.h"

#define PieZoomMSG		710

class Cat;

class CompareCatBySums
{
public:
	bool operator()(const Cat* c1, const Cat* c2) const;
};

struct LabelInfo
{
public:
	BPoint textPt;
	string text;
	uint8 colorIndex;
};

class PieGraph : public BView
{
	sem_id drawSem;
	list<LabelInfo*> textLabels;
	BPoint centerPt;
	set<Cat*, NameSorted>* catset;
	ColorMaker cm;
	void DrawLabel(LabelInfo* textLabel);
	void DrawLabels();
public:
	PieGraph(BRect frame, set<Cat*, NameSorted>* catset);
	void Draw(BRect updateRect);
	void MouseDown(BPoint point);
};
