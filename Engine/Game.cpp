/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include "SpriteCodex.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	config("config.txt"),
	brd( gfx ),
	rng( std::random_device()() ),
	snek( {2,2} )
{
	std::string str_config = "";
	// find length of file
	config.seekg(0, std::ios::end);
	const int length = config.tellg();
	//return to beginning of file
	config.seekg(0, std::ios::beg);
	// read file into string
	for (char c = config.get(); config.good(); c = config.get()) str_config.push_back(c);
	config.close();
	const int iBrdSize = str_config.find("[Board Size]");
	const int iTileSize = str_config.find("[Tile Size]");
	const int iSpeedupRate = str_config.find("[Speedup Rate]");
	const int iPoisonAmount = str_config.find("[Poison Amount]");
	const int iGoalAmount = str_config.find("[Goal Amount]");
	//check if config for board size exists and set it
	if (iBrdSize != std::string::npos)
	{
		// find values
		// get substring of configuration from brd size to end of configuration
		std::string s = str_config.substr(iBrdSize, str_config.size() - iBrdSize);
		const int iValue = findValuePos(s);
		if (iValue != -1)
		{
			// get substring of configuration from the value1 position to end of configuration
			std::string s2 = s.substr(iValue, s.size() - iValue);
			int begStrAfterValue1 = s2.find(" ");
			// get substring of configuration from space after value1 to end of config
			s2 = s2.substr(begStrAfterValue1, s.size() - begStrAfterValue1);
			const int iValue2 = findValuePos(s2);
			const int nextSettingPos = findNextSettingPos(s2);
			if ((nextSettingPos == -1 && iValue2 != -1) || (iValue2 < nextSettingPos && nextSettingPos != -1))
			{
				int brdWidth = std::stoi(s.substr(iValue, s.find(" ")));
				int brdHeight = std::stoi(s2.substr(iValue2, 
					// if there is no newline after the second value assume the file ends
					// cuz that's how this needs to be written and i'm lazy
					(s2.find("\n") == std::string::npos) ? s2.size() - 1 : s2.find("\n")));
				brd = Board(brd.GetTileDimension(), brdWidth, brdHeight, gfx);
			}
		}
	}
	if (iTileSize != std::string::npos) //check if config for tile size exists and set it
	{
		// find values
		// get substring of configuration from brd size to end of configuration
		std::string s = str_config.substr(iTileSize, str_config.size() - iTileSize);
		int nextSettingPos = findNextSettingPos(s);
		const int iValue = findValuePos(s);
		if ((nextSettingPos == -1 && iValue != -1) || (iValue < nextSettingPos && nextSettingPos != -1))
		{
			int dimension = std::stoi(s.substr(iValue, s.find(" ")));
			brd = Board(dimension, brd.GetGridWidth(), brd.GetGridHeight(), gfx);
		}
	}
	if (iSpeedupRate != std::string::npos) //check if config for speedup rate exists and set it
	{
		// find values
		// get substring of configuration from brd size to end of configuration
		std::string s = str_config.substr(iSpeedupRate, str_config.size() - iSpeedupRate);
		int nextSettingPos = findNextSettingPos(s);
		const int iValue = findValuePos(s);
		if ((nextSettingPos == -1 && iValue != -1) || (iValue < nextSettingPos && nextSettingPos != -1))
		{
			snekSpeedupFactor = 1/std::stof(s.substr(iValue,
				(s.find(" ") != std::string::npos) ? s.find(" ") : s.find("\n")));
		}
	}
	if (iPoisonAmount != std::string::npos) //check if config for poison amount exists and set it
	{
		// find values
		// get substring of configuration from brd size to end of configuration
		std::string s = str_config.substr(iPoisonAmount, str_config.size() - iPoisonAmount);
		int nextSettingPos = findNextSettingPos(s);
		const int iValue = findValuePos(s);
		if ((nextSettingPos == -1 && iValue != -1) || (iValue < nextSettingPos && nextSettingPos != -1))
		{
			nPoison = std::stoi(s.substr(iValue, (s.find(" ") != std::string::npos) ? s.find(" ") : s.find("\n")));
		}
	} 
	if (iGoalAmount != std::string::npos) //check if config for goal amount exists and set it
	{
		// find values
		// get substring of configuration from brd size to end of configuration
		std::string s = str_config.substr(iGoalAmount, str_config.size() - iGoalAmount);
		int nextSettingPos = findNextSettingPos(s);
		const int iValue = findValuePos(s);
		if ((nextSettingPos == -1 && iValue != -1) || (iValue < nextSettingPos && nextSettingPos != -1))
		{
			nFood = std::stoi(s.substr(iValue, (s.find(" ") != std::string::npos) ? s.find(" ") : s.find("\n")));
		}
	}

	for( int i = 0; i < nPoison; i++ )
	{
		brd.SpawnContents( rng,snek,Board::CellContents::Poison );
	}
	for( int i = 0; i < nFood; i++ )
	{
		brd.SpawnContents( rng,snek,Board::CellContents::Food );
	}
	sndTitle.Play( 1.0f,1.0f );
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	const float dt = ft.Mark();
	
	if( gameIsStarted )
	{
		if( !gameIsOver )
		{
			if( wnd.kbd.KeyIsPressed( VK_UP ) )
			{
				delta_loc = { 0,-1 };
			}
			else if( wnd.kbd.KeyIsPressed( VK_DOWN ) )
			{
				delta_loc = { 0,1 };
			}
			else if( wnd.kbd.KeyIsPressed( VK_LEFT ) )
			{
				delta_loc = { -1,0 };
			}
			else if( wnd.kbd.KeyIsPressed( VK_RIGHT ) )
			{
				delta_loc = { 1,0 };
			}

			float snekModifiedMovePeriod = snekMovePeriod;
			if( wnd.kbd.KeyIsPressed( VK_CONTROL ) )
			{
				snekModifiedMovePeriod = std::min( snekMovePeriod,snekMovePeriodSpeedup );
			}

			snekMoveCounter += dt;
			if( snekMoveCounter >= snekModifiedMovePeriod )
			{
				snekMoveCounter -= snekModifiedMovePeriod;
				const Location next = snek.GetNextHeadLocation( delta_loc );
				const Board::CellContents contents = brd.GetContents( next );
				if( !brd.IsInsideBoard( next ) ||
					snek.IsInTileExceptEnd( next ) ||
					contents == Board::CellContents::Obstacle )
				{
					gameIsOver = true;
					sndFart.Play( rng,1.2f );
					sndMusic.StopAll();
				}
				else if( contents == Board::CellContents::Food )
				{
					snek.GrowAndMoveBy( delta_loc );
					brd.ConsumeContents( next );
					brd.SpawnContents( rng,snek,Board::CellContents::Obstacle );
					brd.SpawnContents( rng,snek,Board::CellContents::Food );
					sfxEat.Play( rng,0.8f );
				}
				else if( contents == Board::CellContents::Poison )
				{
					snek.MoveBy( delta_loc );
					brd.ConsumeContents( next );
					snekMovePeriod = std::max( snekMovePeriod * snekSpeedupFactor,snekMovePeriodMin );
					sndFart.Play( rng,0.6f );
				}
				else
				{
					snek.MoveBy( delta_loc );
					sfxSlither.Play( rng,0.08f );
				}
			}
		}
	}
	else
	{
		if( wnd.kbd.KeyIsPressed( VK_RETURN ) )
		{
			sndMusic.Play( 1.0f,0.6f );
			gameIsStarted = true;
		}
	}
}

void Game::ComposeFrame()
{
	if( gameIsStarted )
	{
		snek.Draw( brd );
		brd.DrawCells();
		if( gameIsOver )
		{
			SpriteCodex::DrawGameOver( 350,265,gfx );
		}
		brd.DrawBorder();
	}
	else
	{
		SpriteCodex::DrawTitle( 290,225,gfx );
	}
}

const int Game::findValuePos(std::string& s)
{
	int lowestFoundDigitPos = -1;
	int nextSettingPos = findNextSettingPos(s);
	for(int i = 0; i < 10; i++)
	{
		int ipos = s.find(std::to_string(i));
		// if the digit is found after this setting 
		// and if this is not the last setting listed
		// search for the next digit instead
		if (ipos > nextSettingPos && nextSettingPos != -1) continue;
		else
		{
			if (ipos != std::string::npos)
			{
				if (lowestFoundDigitPos == -1) lowestFoundDigitPos = ipos;
				else
				// if the digit found is positioned before the lowest digit position found so far
				// it replaces the lowest digit position found
				lowestFoundDigitPos = (ipos < lowestFoundDigitPos && lowestFoundDigitPos != -1) ? ipos : lowestFoundDigitPos;
			}
			else
			{
				continue;
			}
		}
	}
	return lowestFoundDigitPos;
}

int Game::findNextSettingPos(std::string& s)
{
	int iBrdSize = s.find("[Board Size]");
	int iTileSize;
	int iSpeedupRate;
	int iPoisonAmount;
	int iGoalAmount;
	// check each of the setting headers one at a time and return the pos when the first is found
	if (iBrdSize != std::string::npos && iBrdSize != 0) return iBrdSize;
	else
	{
		iTileSize = s.find("[Tile Size]");
		if (iTileSize != std::string::npos && iTileSize != 0) return iTileSize;
		else
		{
			iSpeedupRate = s.find("[Speedup Rate]");
			if (iSpeedupRate != std::string::npos && iSpeedupRate != 0) return iSpeedupRate;
			else
			{
				iPoisonAmount = s.find("[Poison Amount]");
				if (iPoisonAmount != std::string::npos && iPoisonAmount != 0) return iPoisonAmount;
				else
				{
					iGoalAmount = s.find("[Goal Amount]");
					if (iGoalAmount != std::string::npos && iGoalAmount != 0) return iGoalAmount;
					else return -1;
				}
			}
		}
	}
}