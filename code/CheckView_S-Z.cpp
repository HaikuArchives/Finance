#include <stdio.h>
#include <string.h>
#include <Button.h>
#include <TextControl.h>
#include "Acc.h"
#include "CheckView.h"
#include "Pye.h"
#include "SplCat.h"
#include "Tra.h"

void CheckView::SetCat(Cat* cat, bool pam, int32 amt)
{
	tsf = false;
	pyeText->SetEnabled(true);
	if (theSplCat)
		delete theSplCat;
	theSplCat = 0;
	catText->SetText(cat->Name().c_str());
	if (pam)
	{
		pamText->SetText(prefs->currencyToString(amt).c_str());
		depText->SetText("");
	}
	else
	{
		depText->SetText(prefs->currencyToString(amt).c_str());
		pamText->SetText("");
	}
	RedoCatPopUp();
	RedoPyePopUp();
}

void CheckView::SetSplCat(SplCat* scat, bool pam)
{
	#ifdef DEBUG
	printf("CheckView::SetSplCat - scat = %p, pam = %i\n", scat, pam);
	#endif
	if (tsf)
	{
		tsf = false;
		pyeText->SetEnabled(true);
		pyeText->SetText("");
	}
	if (theSplCat)
		delete theSplCat;
	theSplCat = scat;
	catText->SetText(scat->Name().c_str());
	catText->SetEnabled(false);
	pamText->SetEnabled(false);
	pamPop->SetEnabled(false);
	depText->SetEnabled(false);
	depPop->SetEnabled(false);
	RedoCatPopUp();
	RedoPyePopUp();
	uint32 sum = 0;
	for (i32l::iterator iter = scat->amtL->begin();
		iter != scat->amtL->end(); iter++)
	{
		sum += *iter;
	}
	if (pam)
	{
		pamText->SetText(prefs->currencyToString(sum).c_str());
		depText->SetText("");
	}
	else
	{
		depText->SetText(prefs->currencyToString(sum).c_str());
		pamText->SetText("");
	}
}

void CheckView::UpdatePopUps()
{
	RedoCatPopUp();
	RedoDatPopUp();
	RedoPyePopUp();
	RedoPamPopUp();
	RedoDepPopUp();
}

void CheckView::UpdateTra(Tra* tra)
{
	#ifdef DEBUG
	printf("CheckView::UpdateTra\n");
	#endif
	curTra = tra;
	RedoDatPopUp();
	RedoPamPopUp();
	RedoDepPopUp();
	RedoNumPopUp();

	char buffer[20];
	
	if (tra)
	{
		deleteB->SetEnabled(true);
	
		if (tra->num.type == 0)
			sprintf(buffer, "%i", tra->num.chknum);
		else if (tra->num.type == 1)
			strcpy(buffer, "ATM");
		else if (tra->num.type == 2)
			strcpy(buffer, "EFT");
		else if (tra->num.type == 3)
			strcpy(buffer, "Xfer");
		else if (tra->num.type == 4)
			strcpy(buffer, "");
		numText->SetText(buffer);

		datText->SetText(prefs->dateToString(tra->date).c_str());

		tsf = tra->tsf;
		tsfAcc = tra->tsfAcc;
		if (!tsf)
			pyeText->SetText(tra->pye->name.c_str());
		else
			pyeText->SetText(("Transfer with: " + tra->tsfAcc->name).c_str());

		if (tra->pam)
		{
			pamText->SetText(prefs->currencyToString(tra->amt).c_str());
			depText->SetText("");
		}
		else
		{
			pamText->SetText("");
			depText->SetText(prefs->currencyToString(tra->amt).c_str());
		}
		
		if (tsf)
			catText->SetText("Transfer");
		else
			catText->SetText(tra->cat->Name().c_str());
		if (!tra->tsf && tra->cat->spl)
		{
			if (theSplCat)
				delete theSplCat;
			theSplCat = new SplCat(*(SplCat*)tra->cat);
			catText->SetEnabled(false);
			pamText->SetEnabled(false);
			pamPop->SetEnabled(false);
			depText->SetEnabled(false);
			depPop->SetEnabled(false);
			pyeText->SetEnabled(true);
		}
		else if (tra->tsf)
		{
			if (theSplCat)
				delete theSplCat;
			theSplCat = 0;
			catText->SetEnabled(false);
			pyeText->SetEnabled(false);
			pamPop->SetEnabled(true);
			depPop->SetEnabled(true);
			depText->SetEnabled(true);
			pamText->SetEnabled(true);
		}
		else
		{
			if (theSplCat)
				delete theSplCat;
			theSplCat = 0;
			pamPop->SetEnabled(true);
			depPop->SetEnabled(true);
			depText->SetEnabled(true);
			pamText->SetEnabled(true);
			catText->SetEnabled(true);
			pyeText->SetEnabled(true);
		}
		
		memText->SetText(tra->mem.c_str());
	}
	else
	{
		deleteB->SetEnabled(false);
		numText->SetText("");
		datText->SetText("");
		pyeText->SetText("");
		pamText->SetText("");
		depText->SetText("");
		catText->SetText("");
		memText->SetText("");
		theSplCat = 0;
		tsf = false;
		pamPop->SetEnabled(true);
		depPop->SetEnabled(true);
		depText->SetEnabled(true);
		pamText->SetEnabled(true);
		catText->SetEnabled(true);
		pyeText->SetEnabled(true);
	}

	RedoCatPopUp();
	RedoPyePopUp();
}
