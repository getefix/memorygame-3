/*
=================
- cTreePicker.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cTreePicker.h"

/*
=================================================================
 Defualt Constructor
=================================================================
*/
cTreePicker::cTreePicker()
{
	this->initialiseTreeList();
	this->theTreeToPlant = -1;
}


void cTreePicker::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, cTextureMgr* theTxtMgr, vector<LPCSTR> theTxt)
{
	SDL_Rect tilePosXY;
	tilePosXY = { treeListStartXY.x, treeListStartXY.y, 0, 0 };
	// determine number of rows and columns for array
	int numRows = sizeof(treePickingList) / sizeof(treePickingList[0]);
	int txtToUse = 0;

	for (int row = 0; row < numRows; row++)
	{
		switch (treePickingList[row])
			{
			case 1:
				txtToUse = quebec;
				break;
			case 2:
				txtToUse = portsmouth;
				break;
			case 3:
				txtToUse = tremblay;
				break;
			case 4:
				txtToUse = gcu;
				break;
			}
		this->aTile.setTexture(theTxtMgr->getTexture(theTxt[txtToUse]));
		this->aTile.setSpriteDimensions(theTxtMgr->getTexture(theTxt[txtToUse])->getTWidth(), theTxtMgr->getTexture(theTxt[txtToUse])->getTHeight());
		tilePosXY.w = aTile.getSpritePos().w;
		tilePosXY.h = aTile.getSpritePos().h;
		// Render tile
		aTile.render(theRenderer, &aTile.getSpriteDimensions(), &tilePosXY, aTile.getSpriteRotAngle(), &aTile.getSpriteCentre(), aTile.getSpriteScale());
		tilePosXY.y += aTile.getSpritePos().h;
	}
}
/*
=================================================================
Update the sprite position
=================================================================
*/

void cTreePicker::update(SDL_Point theAreaClicked)
{
	SDL_Point areaClicked = theAreaClicked;
		if (areaClicked.x > 740 && areaClicked.x < 740+ this->aTile.getSpriteDimensions().w && areaClicked.y > 100 && areaClicked.y < 100+ this->aTile.getSpriteDimensions().h*4)
		{
			this->tileClickedRC.x = (int)(areaClicked.x - this->treeListStartXY.x) / this->aTile.getSpriteDimensions().w;
			this->tileClickedRC.y = (int)(areaClicked.y - this->treeListStartXY.y) / this->aTile.getSpriteDimensions().h;
			this->theTreeToPlant = this->tileClickedRC.y + 1;
		}
}

/*
=================================================================
- Set the initial values for the map
=================================================================
*/
void cTreePicker::initialiseTreeList()
{
	for (int row = 0; row < 4; row++)
	{
		this->treePickingList[row] = row + 1;
	}
	 
}

/*
=================================================================
- set start position for tile map
=================================================================
*/
void cTreePicker::setTreeListStartXY(SDL_Point startPosXY)
{
	this->treeListStartXY = startPosXY;
}
/*
=================================================================
- get start pposition for tile map
=================================================================
*/
SDL_Point cTreePicker::getTreeListStartXY()
{
	return this->treeListStartXY;
}
/*
=================================================================
- set value of tree to be used
=================================================================
*/
void cTreePicker::setTreeToPlant(int treeToPlant)
{
	this->theTreeToPlant = treeToPlant;
}
/*
=================================================================
- get value of tree to be used
=================================================================
*/
int cTreePicker::getTreeToPlant()
{
	return this->theTreeToPlant;
}