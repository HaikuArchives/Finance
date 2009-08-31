#include <Button.h>

#include <set>
#include <stdio.h>
#include <Window.h>
#include "Cat.h"
#include "PieGraph.h"
#include "Version.h"

bool CompareCatBySums::operator()(const Cat* c1, const Cat* c2) const
{
	if (c1->sum != c2->sum)
		return (c1->sum < c2->sum);
	return (c1->Name() < c2->Name());
}

PieGraph::PieGraph(BRect frame, set<Cat*, NameSorted>* catset) :
	BView(frame, "PieGraph", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW | 
	B_SUBPIXEL_PRECISE)
{
	#ifdef DEBUG
	printf("PieGraph::PieGraph\n");
	#endif
	this->catset = catset;

	// Force the font to be size 10
	// Don't set font type
	BFont font;
	GetFont(&font);
	font.SetSize(10);
	SetFont(&font);
	
	// Allow the user to zoom the Pie Graph
	BButton* button = new BButton(BRect(5, 5, 45, 12), "zoomB", "Zoom", new BMessage(PieZoomMSG));
	AddChild(button);
	
	drawSem = create_sem(1, "Pie Graph Draw Sem");
}

void PieGraph::Draw(BRect updateRect)
{
	#ifdef DEBUG
	printf("PieGraph::Draw\n");
	#endif
	acquire_sem(drawSem);
	
	while (!textLabels.empty())
	{
		delete textLabels.front();
		textLabels.pop_front();
	}

	// Erase everything in the view
	BRect frame = Frame();
	SetHighColor(255, 255, 255);
	FillRect(updateRect);

	// Draw a border around the view
	frame.OffsetTo(0, 0);
	SetHighColor(156, 156, 156);
	StrokeRect(frame);

	// If no categories return
	if (catset->empty())
	{
		release_sem(drawSem);
		return;
	}

	BRect graphArea = Frame();

	// pams = total amount of payments
	int32 pams = 0;

	// Make a set of categories amounts ordered by trans. sums
	set<Cat*, CompareCatBySums> pamSet;
	for (set<Cat*, NameSorted>::iterator iter = catset->begin();
		iter != catset->end(); iter++)
	{
		if ((*iter)->sum < 0)
		{
			pams += (*iter)->sum;
			pamSet.insert(*iter);
		}
	}

	// How many, at most 12, categories are there?
	uint8 mincat = 12 < pamSet.size() ? 12 : pamSet.size();

	if (mincat == 0)
	{
		release_sem(drawSem);
		return;
	}
	
	set<Cat*>::iterator iter = pamSet.begin();

	// accum = accum sum to this point
	int32 accum = 0;
	// span = span of degrees to fill for pie slice
	uint16 span = 0;
	float spokeLength = (graphArea.Width() < graphArea.Height() ? graphArea.Width() :
		graphArea.Height()) / 2 - 3;
	centerPt.Set(graphArea.Width() / 2, graphArea.Height() / 2);
	BRect pieArea(centerPt.x - spokeLength, centerPt.y - spokeLength, centerPt.x + spokeLength,
		centerPt.y + spokeLength);
	// accumpart = how much of the total payments has been accumulated in degrees
	float accumpart = 0;
	uint8 i;
	float strW = 0;
	BPoint spokePt;
	BPoint textPt;
	LabelInfo* newLI;
	
	for (i = 0; i < mincat; i++)
	{
		// Vary the color for the pie slices
		SetHighColor(cm.ColorNumber(i));
		accumpart = (float)accum / (float) pams * 360.0f;
		// Width of FilledArc in degrees
		span = (uint16)(360.0f * (float)((*iter)->sum + accum) / (float)pams - accumpart + .5);
		// If we've gotten to spans smaller than 20 degrees stop
		if (span < 20)
			break;
		FillArc(BRect(pieArea), accumpart, span);
		// If there is more than one category, draw a dividing line
		if (mincat > 1)
		{
			SetHighColor(0, 0, 0);
			spokePt.x = centerPt.x + spokeLength * cos(M_PI * accumpart / 180.0f);
			spokePt.y = centerPt.y - spokeLength * sin(M_PI * accumpart / 180.0f);
			StrokeLine(centerPt, spokePt);
		}
		textPt.x = centerPt.x + .5 * spokeLength * cos(M_PI * (accumpart + span / 2) / 180.0f);
		textPt.y = centerPt.y - .9 * spokeLength * sin(M_PI * (accumpart + span / 2) / 180.0f);
		// Add this category to the total amount done so far
		accum += (*iter)->sum;
		// How wide is the title for this category?
		uint8 percent = uint8((float)(*iter)->sum / (float)pams * 100.0f + .5f);
		char percStr[4];
		sprintf(percStr, "%i", percent);
		strW = StringWidth(((*iter)->Name() + " " + percStr + "%").c_str());
		// Move the starting point for the text to 
		textPt.x -= strW / 2;
		textPt.y += 4;

		newLI = new LabelInfo;
		newLI->textPt = textPt;
		newLI->text = ((*iter)->Name() + " " + percStr + "%").c_str();
		newLI->colorIndex = i;
		textLabels.push_back(newLI);
		
		iter++;
	}

	// Did we not finish all the categories?
	if (i < pamSet.size())
	{
		// Other pie slice is equal to 360 degrees minus what we have already done
		span = (uint16)(360 - accumpart);
		SetHighColor(0, 0, 0);
		spokePt.x = centerPt.x + spokeLength * cos(M_PI * accumpart / 180.0f);
		textPt.x = centerPt.x + .5 * spokeLength * cos(M_PI * (accumpart + span / 2) / 180.0f);
		spokePt.y = centerPt.y - spokeLength * sin(M_PI * accumpart / 180.0f);
		textPt.y = centerPt.y - .9 * spokeLength * sin(M_PI * (accumpart + span / 2) / 180.0f);
		StrokeLine(centerPt, spokePt);
		strW = StringWidth("Other");
		textPt.x -= strW / 2;
		textPt.y += 4;
		newLI = new LabelInfo;
		newLI->textPt = textPt;
		newLI->text = "Other";
		newLI->colorIndex = 255;
		textLabels.push_back(newLI);
	}

	// Outline and final line
	SetHighColor(0, 0, 0);
	StrokeEllipse(pieArea);
	
	if (mincat > 1)
	{
		spokePt.x = centerPt.x + spokeLength;
		spokePt.y = centerPt.y;
		StrokeLine(centerPt, spokePt);
	}
	
	release_sem(drawSem);

	DrawLabels();
}

void PieGraph::DrawLabel(LabelInfo* textLabel)
{
	float strW = StringWidth(textLabel->text.c_str());
	// Vary the color accordingly for this category
	SetHighColor(cm.ColorNumber(textLabel->colorIndex));
	// Fill a rectangle of the appropriate color for the category label
	FillRect(BRect(textLabel->textPt.x - 3, textLabel->textPt.y - 11, 
		textLabel->textPt.x + strW + 3, textLabel->textPt.y + 3));
	SetHighColor(0, 0, 0);
	SetLowColor(cm.ColorNumber(textLabel->colorIndex));
	// Stroke the rectangle and draw the text
	StrokeRect(BRect(textLabel->textPt.x - 3, textLabel->textPt.y - 11, 
		textLabel->textPt.x + strW + 3, textLabel->textPt.y + 3));
	DrawString(textLabel->text.c_str(), textLabel->textPt);
}

void PieGraph::DrawLabels()
{
	acquire_sem(drawSem);
	for (list<LabelInfo*>::iterator iter = textLabels.begin();
		iter != textLabels.end(); iter++)
	{
		DrawLabel(*iter);
	}
	release_sem(drawSem);
}

void PieGraph::MouseDown(BPoint point)
{
	BPoint diff;
	acquire_sem(drawSem);
	for (list<LabelInfo*>::iterator iter = textLabels.begin();
		iter != textLabels.end(); iter++)
	{
		diff = point - (*iter)->textPt;
		if (fabs(diff.x) < StringWidth((*iter)->text.c_str()) && diff.y > -10 && diff.y < 2)
		{
			DrawLabel(*iter);
			break;
		}
	}
	release_sem(drawSem);
}