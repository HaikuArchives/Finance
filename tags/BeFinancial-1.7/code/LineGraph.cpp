#include <Button.h>
#include <stdio.h>
#include "Acc.h"
#include "LineGraph.h"
#include "Version.h"

LineGraph::LineGraph(BRect frame, list<Acc*>* acclist, Prefs* prefs) :
	BView(frame, "LineGraph", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW | 
	B_SUBPIXEL_PRECISE)
{
	#ifdef DEBUG
	printf("LineGraph::LineGraph\n");
	#endif
	this->acclist = acclist;
	this->prefs = prefs;

	BFont font;
	GetFont(&font);
	font.SetSize(10);
	SetFont(&font);

	BButton* button = new BButton(BRect(5, 5, 45, 12), "zoomB", "Zoom", new BMessage(LineZoomMSG));
	AddChild(button);
	
	FindBounds();

	if (!itslist.empty())
		CalcMonthlyBals();
}


LineGraph::~LineGraph()
{
}

void LineGraph::CalcMonthlyBals()
{
	while (!monthlybalances.empty())
	{
		delete monthlybalances.front();
		monthlybalances.pop_front();
	}

	// aliter = iterator to a list of accounts
	list<Acc*>::iterator aliter;

	// m = counter for months of the year starting at the startmonth
	uint8 m = startmonth;
	list<int32>* balances;
	// total*balance = total for a month
	// maxtotal*balance = highest/lowest totals out of all months
	uint32 totalpositivebalance = 0;
	uint32 totalnegativebalance = 0;
	maxtotalpositivebalance = 0;
	maxtotalnegativebalance = 0;
	// Do all the years that we have data for
	for (uint8 y = startyear; y <= endyear; y++)
	{
		// Keep us inside normal months while iterating
		while (m < 12 && !(y == endyear && m > endmonth))
		{
			totalpositivebalance = 0;
			totalnegativebalance = 0;
			balances = new list<int32>;
			// Add the inner list to the outer list
			monthlybalances.push_back(balances);
			aliter = acclist->begin();
			// Go through the transactions for an account
			for (list<set<Tra*, Tra>::iterator>::iterator itsliter = 
				itslist.begin(); itsliter != itslist.end(); itsliter++)
			{
				// backdown = indication that we've gone past the month/year we want
				bool backdown = false;
				while ((*aliter)->traS.empty())
					aliter++;
				while (*itsliter != (*aliter)->traS.end() && (*(*itsliter))->Year() < y)
				{
					(*itsliter)++;
					backdown = true;
				}
				while (*itsliter != (*aliter)->traS.end() && (*(*itsliter))->Month() <= m && (*(*itsliter))->Year() == y)
				{
					(*itsliter)++;
					backdown = true;
				}
				// We have gone past the month/year we want; go back a transaction
				if (backdown)
					(*itsliter)--;
				// Add this balance to the inner list
				balances->push_back((*(*itsliter))->balFol);
				if ((*(*itsliter))->balFol > 0)
					totalpositivebalance += (*(*itsliter))->balFol;
				else
					totalnegativebalance -= (*(*itsliter))->balFol;
				aliter++;
			}
			m++;
			// Is this the highest/lowest sum so far?
			if (totalpositivebalance > maxtotalpositivebalance)
				maxtotalpositivebalance = totalpositivebalance;
			if (totalnegativebalance > maxtotalnegativebalance)
				maxtotalnegativebalance = totalnegativebalance;
		}
		m = 0;
	}
}

uint32 LineGraph::DivideRange(uint32 range)
{
	const uint8 maxDivisions = 14;
	const uint8 minDivisions = 7;
	
	uint32 divider = 1;
	
	while (range > maxDivisions)
	{
		if ((float)range / 2.5f > minDivisions && divider % 2 == 0)
		{
			range = (uint32)((float)range / 2.5f);
			divider = (uint32)((float)divider * 2.5f);
		}
		else
		{
			range /= 2;
			divider *= 2;
		}
	}
	
	return divider;
}

void LineGraph::Draw(BRect updateRect)
{
	BRect frame = Frame();
	frame.OffsetTo(0, 0);
	SetHighColor(156, 156, 156);
	StrokeRect(frame);
	
	if (itslist.empty())
		return;
	
	int32 nextbalance;
	BPoint lastbalancept, nextbalancept;
	uint32 posbalance = 0;
	uint32 negbalance = 0;
	BRect graphArea = Frame();
	graphArea.top += 3;
	graphArea.bottom -= 23;
	graphArea.right -= 35;
	graphArea.OffsetTo(35, 3);
	float monthWidth = graphArea.Width() / (monthlybalances.size() + 1);
	uint16 month = 0;
	// What is the range between the highest and lowest values?
	float range = (maxtotalnegativebalance + maxtotalpositivebalance) / graphArea.Height();

	uint32 centsInADollar = 1;
	for (uint8 i = 0; i < prefs->curDecPlace; i++)
		centsInADollar *= 10;
	uint32 hashUnits = centsInADollar * DivideRange((uint32)(maxtotalnegativebalance + maxtotalpositivebalance) / centsInADollar);

	// Draw line to mark $0
	BPoint pospt(graphArea.left, 0);
	BPoint newbalpt(graphArea.right, 0);
	pospt.y = newbalpt.y = graphArea.bottom - maxtotalnegativebalance / range;
	StrokeLine(pospt, newbalpt);
	
	// Other lines
	uint8 divisions = maxtotalnegativebalance / hashUnits;
	string amtS = "";
	for (uint16 i = 1; i <= divisions; i++)
	{
		pospt.y = newbalpt.y = graphArea.bottom - (maxtotalnegativebalance - hashUnits * i) / range;
		SetHighColor(200, 200, 200);
		StrokeLine(pospt, newbalpt);
		if (i != divisions)
		{
			SetHighColor(150, 150, 150);
			amtS = prefs->currencyToString(-hashUnits * i);
			amtS = amtS.substr(0, amtS.length() - prefs->curDecPlace - 1);
			DrawString(amtS.c_str(), pospt - BPoint(20, -4));
		}
	}
	
	divisions = maxtotalpositivebalance / hashUnits;
	for (uint16 i = 1; i <= divisions; i++)
	{
		pospt.y = newbalpt.y = graphArea.bottom - (maxtotalnegativebalance + hashUnits * i) / range;
		SetHighColor(200, 200, 200);
		StrokeLine(pospt, newbalpt);
		if (i != divisions)
		{
			SetHighColor(150, 150, 150);
			amtS = prefs->currencyToString(hashUnits * i);
			amtS = amtS.substr(0, amtS.length() - prefs->curDecPlace - 1);
			DrawString(amtS.c_str(), pospt - BPoint(20, -4));
		}
	}

	uint8 accnum = 0;
	// textPt = where the month label goes
	BPoint textPt(0, graphArea.bottom + 15);
	char buffer[20];
	float lastText = 0;
	float strW;
	uint8 year;
	BPoint negpt;

	for (list<list<int32>*>::iterator iter = monthlybalances.begin(); 
		iter != monthlybalances.end(); iter++)
	{
		month++;
		pospt.x = graphArea.left + monthWidth * month - monthWidth / 6;
		pospt.y = graphArea.bottom - maxtotalnegativebalance / range;
		negpt.x = pospt.x;
		negpt.y = pospt.y;
		
		textPt.x = graphArea.left + monthWidth * month;
		newbalpt.x = pospt.x + monthWidth / 3;
		posbalance = 0;
		negbalance = 0;
		accnum = 0;
		for (list<int32>::iterator iter2 = (*iter)->begin(); iter2 !=
			(*iter)->end(); iter2++)
		{
			SetHighColor(cm.ColorNumber(accnum++));
			if (*iter2 < 0) // Add a negative bar
			{
				negbalance -= *iter2;
				newbalpt.y = graphArea.bottom - (maxtotalnegativebalance - negbalance) / range;
				FillRect(BRect(negpt.x, negpt.y, newbalpt.x, newbalpt.y));
				uint8 divisions = *iter2 / hashUnits;
				SetHighColor(200, 200, 200);
				for (uint16 i = 1; i <= divisions; i++)
				{
					StrokeLine(BPoint(negpt.x, negpt.y + i * hashUnits / range), 
						BPoint(newbalpt.x, negpt.y + i * hashUnits / range));
				}
				SetHighColor(0, 0, 0);
				StrokeRect(BRect(newbalpt, negpt));
				negpt.y = newbalpt.y;
			}
			else // Add a positive bar
			{
				posbalance += *iter2;
				newbalpt.y = graphArea.bottom - (maxtotalnegativebalance + posbalance) / range;
				FillRect(BRect(pospt.x, newbalpt.y, newbalpt.x, pospt.y));
				uint8 divisions = *iter2 / hashUnits;
				SetHighColor(200, 200, 200);
				for (uint16 i = 1; i <= divisions; i++)
				{
					StrokeLine(BPoint(pospt.x, pospt.y - i * hashUnits / range), 
						BPoint(newbalpt.x, pospt.y - i * hashUnits / range));
				}
				SetHighColor(0, 0, 0);
				StrokeRect(BRect(pospt, newbalpt));
				pospt.y = newbalpt.y;
			}
		}
		nextbalance = posbalance - negbalance;
		nextbalancept.x = pospt.x + monthWidth / 6;
		nextbalancept.y = graphArea.bottom - (maxtotalnegativebalance + nextbalance) / range;
		if (iter != monthlybalances.begin())
			StrokeLine(lastbalancept, nextbalancept);
		lastbalancept = nextbalancept;
		FillEllipse(BRect(lastbalancept.x - 2, lastbalancept.y - 2,
			lastbalancept.x + 2, lastbalancept.y + 2));
		year = ((startmonth + month - 1) / 12 + startyear) % 100;
		sprintf(buffer, "%i/%s%i", (startmonth + month - 1) % 12 + 1, year < 10 ? "0" : "", year);
		strW = StringWidth(buffer);
		textPt.x -= .5 * strW;
		if (textPt.x > lastText + 5 && textPt.x + strW < graphArea.right)
		{
			DrawString(buffer, textPt);
			lastText = textPt.x + strW;
		}
	}
}

void LineGraph::FindBounds()
{
	while (!itslist.empty())
	{
		itslist.pop_front();
	}

	// aliter = iterator to a list of accounts
	list<Acc*>::iterator aliter;
	// totalsize = total number of transactions in each account
	uint32 totalsize = 0;
	//Put in an iterator for each transaction set (account) which has entries
	for (aliter = acclist->begin(); aliter != acclist->end(); aliter++)
	{
		if ((*aliter)->traS.size())
			itslist.push_back((*aliter)->traS.begin());
		totalsize += (*aliter)->traS.size();
	}
	//Return if there are no transactions
	if (!totalsize)
		return;
	// find the first account with transactions
	aliter = acclist->begin();
	while ((*aliter)->traS.empty())
	{
		aliter++;
	}
	// ts = transaction set of an account with transactions
	set<Tra*, Tra>* ts =  &(*aliter)->traS;
	// tsiter = iterator in the transaction set
	set<Tra*, Tra>::iterator tsiter;
	// Put in the first transaction as the starting month and year
	// Put in the last transaction as the ending month and year
	Tra* t = *ts->begin();
	startmonth = t->Month();
	startyear = t->Year();
	t = *ts->rbegin();
	endmonth = t->Month();
	endyear = t->Year();
	aliter++;
	uint8 thismonth = 0;
	uint8 thisyear = 0;
	// Go through all the other transaction sets to see if there is another earlier month and year
	for (; aliter != acclist->end(); aliter++)
	{
		ts = &(*aliter)->traS;
		if (!ts->empty())
		{
			tsiter = ts->begin();
			thismonth = (*tsiter)->Month();
			thisyear = (*tsiter)->Year();
			// If we've found an earlier year, use that year and month
			if (thisyear < startyear)
			{
				startyear = thisyear;
				startmonth = thismonth;
			}
			// If we've found an earlier month in the same year, use that month
			else if (thisyear == startyear)
			{
				if (thismonth < startmonth)
					startmonth = thismonth;
			}
			t = *ts->rbegin();
			thismonth = t->Month();
			thisyear = t->Year();
			// If we've found an later year, use that year and month
			if (thisyear > endyear)
			{
				endyear = thisyear;
				endmonth = thismonth;
			}
			// If we've found an later month in the same year, use that month
			else if (thisyear == endyear)
			{
				if (thismonth > endmonth)
					endmonth = thismonth;
			}
		}
	}
	#ifdef DEBUG
	printf("%i %i %i %i\n", startmonth, startyear, endmonth, endyear);
	#endif
}

void LineGraph::InvalidateData()
{
	FindBounds();

	if (!itslist.empty())
		CalcMonthlyBals();
	
	#ifdef DEBUG
	printf("%i %i %i %i\n", startmonth, startyear, endmonth, endyear);
	#endif

	Invalidate();
}