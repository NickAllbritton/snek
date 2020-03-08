#pragma once

#include "Graphics.h"
#include "Location.h"
#include <random>

class Board
{
public:
	enum class CellContents
	{
		Empty,
		Obstacle,
		Food,
		Poison
	};
public:
	Board( Graphics& gfx );
	Board(int width, int height, Graphics& gfx);
	const Board& operator=(const Board& right);
	void DrawCell( const Location& loc,Color c );
	int GetGridWidth() const;
	int GetGridHeight() const;
	bool IsInsideBoard( const Location& loc ) const;
	CellContents GetContents( const Location& loc ) const;
	void ConsumeContents( const Location& loc );
	void SpawnContents( std::mt19937& rng,const class Snake& snake,CellContents contents );
	void DrawBorder();
	void DrawCells();
private:
	static constexpr Color borderColor = Colors::Blue;
	static constexpr Color obstacleColor = Colors::Gray;
	static constexpr Color poisonColor = { 64,8,64 };
	static constexpr Color foodColor = Colors::Red;
	static constexpr int cellPadding = 1;
	static constexpr int borderWidth = 4;
	static constexpr int borderPadding = 2;
	int dimension = 20;
	int maxwidth;
	int maxheight;
	int width = 32;
	int height = 24;
	int x = (Graphics::ScreenWidth/2) - (width*dimension/2);
	int y = (Graphics::ScreenHeight/2) - (height*dimension/2);
	std::vector<CellContents> contents = { CellContents::Empty };
	Graphics& gfx;
};