#pragma once
#include <deque>

struct Cell
{
	int x; int y;
};

enum class Dir 
{
	Up, Down, Left, Right
};

class SnakeGame
{
public:
	SnakeGame(int gridW, int gridH);
	void Reset();
	void Update(float dt);
	void SetPendingDir(Dir d);
	bool IsGameOver() const;

	const Cell& GetHead() const;
	const std::deque<Cell>& GetBody() const;
	const Cell& GetFood() const;
	int GetGridW() const;
	int GetGridH() const;
	int GetScore() const;

private:
	void Step();
	Cell NextHead() const;
	bool IsOpposite(Dir a, Dir b) const;
	bool HitsWall(const Cell& c) const;
	bool HitsSelf(const Cell& c) const;
	bool EatsFood(const Cell& c) const;
	void SpawnFood();

private:
	int m_gridW, m_gridH;
	std::deque<Cell> m_snake;
	Cell m_food;
	Dir m_dir;
	Dir m_pendingDir;
	bool m_gameOver;
	float m_stepTime;
	float m_acc;
	unsigned m_rngSeed;
};
