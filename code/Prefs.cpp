#include <math.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include "Prefs.h"
#include "Version.h"

Prefs::Prefs()
{
	Set("$", true, ",", ".", 2, 0, "/");
}

Prefs::Prefs(string curSym, bool curSymPlaceBefore, string curSep,
	string curDec, uint8 curDecPlace, uint8 dateFormat, string dateSep)
{
	Set(curSym, curSymPlaceBefore, curSep, curDec, curDecPlace, dateFormat, dateSep);
}

void Prefs::Set(Prefs* prefsToCopy)
{
	curSym = prefsToCopy->curSym;
	curSymPlaceBefore = prefsToCopy->curSymPlaceBefore;
	curSep = prefsToCopy->curSep;
	curDec = prefsToCopy->curDec;
	curDecPlace = prefsToCopy->curDecPlace;
	dateFormat = prefsToCopy->dateFormat;
	dateSep = prefsToCopy->dateSep;
}

void Prefs::Set(string curSym, bool curSymPlaceBefore, string curSep,
	string curDec, uint8 curDecPlace, uint8 dateFormat, string dateSep)
{
	this->curSym = curSym;
	this->curSymPlaceBefore = curSymPlaceBefore;
	this->curSep = curSep;
	this->curDec = curDec;
	this->curDecPlace = curDecPlace;
	this->dateFormat = dateFormat;
	this->dateSep = dateSep;
}

string Prefs::currencyToString(int32 amount)
{
	bool negative = amount < 0;
	if (negative)
		amount = -amount;
	uint16 parts = 1;
	for (uint8 i = 0; i < curDecPlace; i++)
		parts *= 10;
	uint16 cents = amount % parts;
	string amtS;

	char buffer[10];
	sprintf(buffer, "%li", amount / parts);
	amtS = buffer;
	for (int8 i = amtS.size() - 3; i >= 1; i -= 3)
	{
		amtS.insert(i, curSep);
	}
	
	if (curDecPlace > 0)
	{
		amtS += curDec;
		sprintf(buffer, "%i", cents);
		string centsS = buffer;
		for (int8 i = 0; i < (int16)curDecPlace - (int16)centsS.size(); i++)
		{
			amtS += "0";
		}
		amtS += centsS;
	}
	
	if (negative)
		amtS = "-" + amtS;
		
	if (curSymPlaceBefore)
		amtS = curSym + amtS;
	else
		amtS = amtS + curSym;
	
	return amtS;
}

string Prefs::dateToString(time_t date)
{
	char buffer[10];
	struct tm* timestruct = localtime(&date);
	string dateS;
	if (dateFormat == 0)
		strftime(buffer, 10, "%m", timestruct);
	else if (dateFormat == 1)
		strftime(buffer, 10, "%d", timestruct);
	dateS += buffer;
	dateS += dateSep;
	if (dateFormat == 0)
		strftime(buffer, 10, "%d", timestruct);
	else if (dateFormat == 1)
		strftime(buffer, 10, "%m", timestruct);
	dateS += buffer;
	dateS += dateSep;
	strftime(buffer, 10, "%Y", timestruct);
	dateS += buffer;
	return dateS;
}

int32 Prefs::stringToCurrency(string amttext)
{
	size_t pos0;
	while ((pos0 = amttext.find_first_of(curSym + curSep)) != string::npos)
	{
		amttext.erase(pos0, 1);
	}
	if (curDec != ".")
		while ((pos0 = amttext.find_first_of(curDec)) != string::npos)
		{
			amttext.replace(pos0, 1, ".");
		}
	if (amttext.find_first_not_of("0123456789.-") != string::npos)
	{
		throw invalid_argument("");
	}
	float f = atof(amttext.c_str());
//	printf("f = %f\n", f);
	bool neg = f < 0;
	f = fabs(f);
	uint32 base10 = 1;
	for (uint8 i = 0; i < curDecPlace; i++)
		base10 *= 10;
//	printf("base10 = %li\n", base10);
	f *= base10;
//	printf("f * base10 = %f\n", f);
	f += 50.0f / float(base10);
//	printf("50 / float(base10) = %f\n", 50.0f / float(base10));
//	printf("amt = %li\n", (uint32)f);
	int32 amt = (neg ? -1 : 1) * (uint32)f;
	return amt;
}

time_t Prefs::stringToDate(string datetext)
{
	#ifdef DEBUG
	printf("Prefs::stringToDate\n");
	#endif
	string monthstr, daystr, yearstr;
	size_t pos0 = datetext.find_first_of(dateSep);
	monthstr = datetext.substr(0, pos0);
	size_t pos1 = datetext.find_first_of(dateSep + "'", ++pos0);
	daystr = datetext.substr(pos0, pos1 - pos0);
	pos1++;
	yearstr = datetext.substr(pos1, datetext.length() - pos1);
	if (monthstr.find_first_not_of("0123456789 ") != string::npos ||
		daystr.find_first_not_of("0123456789 ") != string::npos ||
		yearstr.find_first_not_of("0123456789 ") != string::npos)
	{
		string error = "Invalid Date Format.  Use ";
		if (dateFormat == 1)
			error += "dd" + dateSep + "mm";
		else
			error += "mm" + dateSep + "dd";
		error += dateSep + "yy Please.";
		throw invalid_argument(error.c_str());
	}
	struct tm timestruct;
	if (yearstr.length() == 2)
	{
		timestruct.tm_year = atoi(yearstr.c_str());
		if (timestruct.tm_year < 90)
			timestruct.tm_year += 100;
	}
	else
		timestruct.tm_year = atoi(yearstr.c_str()) - 1900;
	if (dateFormat == 1)
	{
		string temp = daystr;
		daystr = monthstr;
		monthstr = temp;
	}
	timestruct.tm_mon = atoi(monthstr.c_str()) - 1;
	timestruct.tm_mday = atoi(daystr.c_str());
	timestruct.tm_sec = 0; 
	timestruct.tm_min = 0; 
	timestruct.tm_hour = 0;
	timestruct.tm_isdst = -1;
	time_t datetime = mktime(&timestruct);
	if (datetime == -1)
	{
		string error = "Invalid Date.  Use ";
		if (dateFormat == 1)
			error += "dd" + dateSep + "mm";
		else
			error += "mm" + dateSep + "dd";
		error += dateSep + "yy for a valid date please.";
		throw invalid_argument(error.c_str());
	}
	return datetime;
}
