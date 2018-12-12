/*
=================
- cTileMap.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cTileMap.h"
#include "GameConstants.h"
/*
=================================================================
 Defualt Constructor
=================================================================
*/
cTileMap::cTileMap() : cSprite()
{
	this->initialiseMap();
}


void cTileMap::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, cTextureMgr* theTxtMgr, vector<LPCSTR> theTxt)
{
	SDL_Rect tilePosXY;
	tilePosXY = { mapStartXY.x,  mapStartXY.y, 0 , 0 };
	// determine number of rows and columns for array
	int numRows = sizeof(tileMap) / sizeof(tileMap[0]);
	int numCols = sizeof(tileMap[0]) / sizeof(tileMap[0][0]);
	int txtToUse = 0;
	for (int row = 0; row < numRows; row++)
	{
		for (int column = 0; column < numCols; column++)
		{
			txtToUse = tileMap[row][column];
			
			aTile.setTexture(theTxtMgr->getTexture(theTxt[txtToUse]));   // txtToUse
			aTile.setSpriteDimensions(theTxtMgr->getTexture(theTxt[txtToUse])->getTWidth(), theTxtMgr->getTexture(theTxt[txtToUse])->getTHeight());
			tilePosXY.w = aTile.getSpritePos().w;
			tilePosXY.h = aTile.getSpritePos().h;
			// Render tile
			aTile.render(theRenderer, &aTile.getSpriteDimensions(), &tilePosXY, aTile.getSpriteRotAngle(), &aTile.getSpriteCentre(), aTile.getSpriteScale());
			tilePosXY.x += aTile.getSpritePos().w;
		}
		tilePosXY.x = mapStartXY.x;
		tilePosXY.y += aTile.getSpritePos().h;
	}

}


/*
=================================================================
Update the sprite position
=================================================================
*/

void cTileMap::update(SDL_Point theMapAreaClicked, int theTreeToPlant)
{
	SDL_Point areaClicked = theMapAreaClicked;
		if (theTreeToPlant > -1)
		{
			if (areaClicked.x > 100 && areaClicked.x < 612 && areaClicked.y > 100 && areaClicked.y < 612)
			{
				this->tileClickedRC.x = (int)(areaClicked.x - this->mapStartXY.x) / this->aTile.getSpriteDimensions().w;
				this->tileClickedRC.y = (int)(areaClicked.y - this->mapStartXY.y) / this->aTile.getSpriteDimensions().h;
				this->tileMap[this->tileClickedRC.y][this->tileClickedRC.x] = theTreeToPlant;
			}
		}
}

/*
=================================================================
- Set the values for the map to 1 to show a blank map
=================================================================
*/
void cTileMap::clear()
{
	for (int row = 0; row < BOARD_SIZE; row++)
	{
		for (int column = 0; column < BOARD_SIZE; column++)
		{
			tileMap[row][column] = blank;
		}
	}
}

/*
=================================================================
- Set the initial values for the map
=================================================================
*/
void cTileMap::initialiseMap()
{
	/* Let the computer pick a random number */
	random_device rd;    // non-deterministic engine 
	mt19937 gen{ rd() }; // deterministic engine. For most common uses, std::mersenne_twister_engine, fast and high-quality.
	uniform_int_distribution<> dis{ quebec, gcu };

	for (int row = 0; row < BOARD_SIZE; row++)
	{
		for (int column = 0; column < BOARD_SIZE; column++)
		{
			tileMap[row][column] = dis(gen);
		}
	}

}
void cTileMap::memoriseMap()
{
	for (int column = 0; column < BOARD_SIZE; column++)
	{
		for (int row = 0; row < BOARD_SIZE; row++)
		{
			memorisedMap[column][row] = tileMap[column][row];
		}
	}
}
int cTileMap::compareMaps()
{
	int score = 0;
	for (int column = 0; column < BOARD_SIZE; column++)
	{
		for (int row = 0; row < BOARD_SIZE; row++)
		{
			if(memorisedMap[column][row] == tileMap[column][row])
				score++;
		}
	}
	return score;
}

/*
=================================================================
- set start position for tile map
=================================================================
*/
void cTileMap::setMapStartXY(SDL_Point startPosXY)
{
	mapStartXY = startPosXY;
}
/*
=================================================================
- get start pposition for tile map
=================================================================
*/
SDL_Point cTileMap::getMapStartXY()
{
	return mapStartXY;
}
