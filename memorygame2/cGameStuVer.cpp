/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();

/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	this->m_lastTime = high_resolution_clock::now();
	
	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);
	
	theTextureMgr->setRenderer(theRenderer);
	// Store the textures
	textureName = { "blank", 
		"quebec", 
		"portsmouth",
		"tremblay", 
		"gcu",  
		"transparent", 
		"theBackground" };
	texturesToUse = { "Images\\blank.jpg", 
		"Images\\quebec.jpg",
		"Images\\portsmouth.jpg",
		"Images\\tremblay.jpg",
		"Images\\gcu.jpg", 
		"Images\\transparent.png",
		"Images\\scroll.jpg"};
	for (unsigned int tCount = 0; tCount < textureName.size(); tCount++)
	{	
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	theTileMap.setMapStartXY({ 100, 120 });


	// Load game sounds
	soundList = { "theme", "lose", "win", "end",  "click" };
	soundTypes = { soundType::music, soundType::music, soundType::music, soundType::music, soundType::sfx };
	soundsToUse = { "Audio/theme.mp3", "Audio/lose.mp3", "Audio/win.mp3", "Audio/end.mp3", "Audio/click.wav" };
	for (unsigned int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}
	theSoundMgr->getSnd("theme")->play(-1);
	
	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());


	theTreePicker.setTreeListStartXY({ 740, 120 });

	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());

	dragTile.setSpritePos({ 0, 0 });
	dragTile.setTexture(theTextureMgr->getTexture("transparent"));
	dragTile.setSpriteDimensions(theTextureMgr->getTexture("transparent")->getTWidth(), theTextureMgr->getTexture("transparent")->getTHeight());


	// Creating buttons
	// exit, play, clear, submit, menu
	btnNameList = { "exit_btn", "play_btn", "clear_btn", "submit_btn", "menu_btn" };
	btnTexturesToUse = { "Images/Buttons/button_exit.png", "Images/Buttons/button_play.png", "Images/Buttons/button_load.png", "Images/Buttons/button_settings.png", "Images/Buttons/button_menu.png" };
	btnPos = { { 400, 375 },{ 400, 300 },{ 400, 300 },{ 500, 500 },{ 400, 300 },{ 740, 500 },{ 400, 300 } };
	for (unsigned int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]);
	}
	for (unsigned int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount]));
		newBtn->setSpritePos(btnPos[bCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight());
		theButtonMgr->add(btnNameList[bCount], newBtn);
	}
	theGameState = gameState::start;


	// Create textures for Game Dialogue (text)
	fontList = { "text" };
	fontsToUse = { "Fonts/BlackPearl.ttf" };
	for (unsigned int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
	}
	// Create text Textures
	gameTextNames = { "TitleTxt", 
		"CollectTxt", 
		"CollectTxt2", 
		"InstructTxt", 
		"InstructTxt2",
		"ThanksTxt", 
		"SeeYouTxt",
		"Score" };
	gameTextList = { "Memory Game",
		"Help the scribe write the census by", 
		"memorising the symbols.",
		"drag and drop the symbols into the grid" ,
		"to match the first set of symbols.",
		"Thanks for playing!", 
		"See you again soon!",
		"Score: " };
	
	strScore = gameTextList[7];
	strScore += to_string(score).c_str();

	for (unsigned int text = 0; text < gameTextNames.size(); text++)
	{
		
		{
			theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("text")->createTextTexture(theRenderer, gameTextList[text], textType::solid, { 0, 0, 0, 255 }, { 0, 0, 0, 0 }));
		}
	}

	theTextureMgr->addTexture("Score", theFontMgr->getFont("text")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 0, 0, 0, 255 }, { 0, 0, 0, 0 }));

}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	loop = true;

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);


	switch (theGameState)
	{
	case gameState::menu:
	
		spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 70, 50, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("CollectTxt");
		pos = { 70, 100, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("CollectTxt2");
		pos = { 70, 175, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		// Render Button
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 400, 375 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	
	
	break;
	case gameState::memorise:
	
		spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		theTileMap.render(theSDLWND, theRenderer, theTextureMgr, textureName);

		theButtonMgr->getBtn("clear_btn")->setSpritePos({ WINDOW_WIDTH-350, 300 });
		theButtonMgr->getBtn("clear_btn")->render(theRenderer, &theButtonMgr->getBtn("clear_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("clear_btn")->getSpritePos(), theButtonMgr->getBtn("clear_btn")->getSpriteScale());
		
	
	break;
	case gameState::hide:

		
	
	break;
	case gameState::play:	

		spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		theTileMap.render(theSDLWND, theRenderer, theTextureMgr, textureName);
		theTreePicker.render(theSDLWND, theRenderer, theTextureMgr, textureName);
		if (theTreePicker.getTreeToPlant() > -1)
		{
			dragTile.render(theRenderer, &dragTile.getSpriteDimensions(), &dragTile.getSpritePos(), spriteBkgd.getSpriteScale());
		}
		tempTextTexture = theTextureMgr->getTexture("InstructTxt");
		pos = { 70, 70, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		theButtonMgr->getBtn("submit_btn")->setSpritePos({ WINDOW_WIDTH -285, 635 });
		theButtonMgr->getBtn("submit_btn")->render(theRenderer, &theButtonMgr->getBtn("submit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("submit_btn")->getSpritePos(), theButtonMgr->getBtn("submit_btn")->getSpriteScale());

	
	break;
	case gameState::score:
	
		
		spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		tempTextTexture = theTextureMgr->getTexture("ThanksTxt");
		pos = { 70, 70, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		
		if (updateScore)
		{
			updateScore = false;
			theTextureMgr->deleteTexture("Score");
			theTextureMgr->addTexture("Score", theFontMgr->getFont("text")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 0, 0, 0, 255 }, { 0, 0, 0, 0 }));
		}

		tempTextTexture = theTextureMgr->getTexture("Score");
		pos = { WINDOW_WIDTH/2, WINDOW_HEIGHT/2, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 250, 400 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());

	break;
	case gameState::quit:
	
		loop = false;
		
	
	break;
	default:
		break;
	}


	










	
	SDL_RenderPresent(theRenderer);
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{

	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{
	
	switch (theGameState)
	{
	case gameState::start:

		theSoundMgr->getSnd("theme")->play(-1);

		theGameState = gameState::menu;
		break;

	case gameState::menu:
	
		theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, gameState::setup, theAreaClicked);
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, gameState::quit, theAreaClicked);
	
	break;
	case gameState::setup:
		//Setting up a round of play.
		theTileMap.initialiseMap();

		theGameState = gameState::memorise;
		break;
	case gameState::memorise:
	
		theGameState = theButtonMgr->getBtn("clear_btn")->update(theGameState, gameState::hide, theAreaClicked);
	
	break;
	case gameState::hide:
	
		// Saving the initial map to compare later.
		theTileMap.memoriseMap();
		// Clearing the tiles
		theTileMap.clear();
		theGameState = gameState::play;
	
	break;
	case gameState::play: 
		theGameState = theButtonMgr->getBtn("submit_btn")->update(theGameState, gameState::scoring, theAreaClicked);
	
	break;
	case gameState::scoring:
		// Calculating the score
		score = theTileMap.compareMaps();

		theGameState = gameState::score;
		updateScore = true;
		strScore = gameTextList[7];
		strScore += to_string(score).c_str();
		theGameState =  gameState::score;

		if(score==0)
			theSoundMgr->getSnd("lose")->play(0);
		else if (score == 9)
			theSoundMgr->getSnd("win")->play(-1);
		else
			theSoundMgr->getSnd("end")->play(-1);
		break;
	case gameState::score:
	
		theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, gameState::start, theAreaClicked);
	
	break;
	default:
		break;
	}




}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					theAreaClicked = { event.motion.x, event.motion.y };

					// Lab code goes here
					//get mouse point
					SDL_Point mousePos;
					SDL_GetMouseState(&mousePos.x, &mousePos.y);
					//call update on tree picker to set the tree to plant
					theTreePicker.update(mousePos);

					//show the drag tile
					int textureIndex = theTreePicker.getTreeToPlant();
					if (textureIndex > 0)
					{
						textureIndex = textureIndex;
						dragTile.setTexture(theTextureMgr->getTexture(textureName[textureIndex]));
					}
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					
					// get mouse point
						SDL_Point mousePos;
					SDL_GetMouseState(&mousePos.x, &mousePos.y);
					//call update on tile map passing in the tree to plant
					theTileMap.update(mousePos, theTreePicker.getTreeToPlant());

					//reset the tree to plant
					theTreePicker.setTreeToPlant(-1);

					//hide the drag tile
					dragTile.setTexture(theTextureMgr->getTexture("blank"));
					
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			{
				
				// Lab Code goes here
				//get mouse point
				SDL_Point mousePos;
				SDL_GetMouseState(&mousePos.x, &mousePos.y);

				//set drag tile position
				dragTile.setSpritePos(mousePos);
			}
			break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;
				default:
					break;
				}

			default:
				break;
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

