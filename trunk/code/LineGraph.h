#include <list>
#include <View.h>

#include "ColorMaker.h"
#include "Prefs.h"
class Acc;

#define LineZoomMSG		711

class LineGraph : public BView
{
	list<Acc*>* acclist;
	ColorMaker cm;
	uint8 endmonth;
	uint8 endyear;
	list<set<Tra*, Tra>::iterator> itslist;  // itslist = list of iterators to sets of transactions
	uint32 maxtotalpositivebalance;
	uint32 maxtotalnegativebalance;
	list<list<int32>*> monthlybalances;  // monthlybalances = list of month lists with balances for each account
	Prefs* prefs;
	uint8 startmonth;
	uint8 startyear;
	uint32 DivideRange(uint32);
	void FindBounds();
	void CalcMonthlyBals();
public:
	LineGraph(BRect frame, list<Acc*>* acclist, Prefs* prefs);
	~LineGraph();
	void Draw(BRect updateRect);
	void InvalidateData();
};