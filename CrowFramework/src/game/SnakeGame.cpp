#include <game/SnakeGame.h>

SnakeGame::SnakeGame(int gridW, int gridH)
	: m_gridW(gridW), m_gridH(gridH),
	m_dir(Dir::Right), m_pendingDir(Dir::Right),
	m_gameOver(false),
	m_stepTime(0.2f), m_acc(0.0f)
{
	Reset();
}

void SnakeGame::Reset()
{
	// Reset = recreate initial game state
	m_snake.clear();
	m_gameOver = false;
	m_acc = 0.0f;

	m_dir = Dir::Right;
	m_pendingDir = Dir::Right;

	int cx = m_gridW / 2;
	int cy = m_gridH / 2;

	// front = head
	m_snake.push_back({ cx,     cy });
	m_snake.push_back({ cx - 1, cy });
	m_snake.push_back({ cx - 2, cy });

	SpawnFood();
}

void SnakeGame::Update(float dt)
{
	// Stop advancing game logic after death
	if (m_gameOver) return;

	m_acc += dt;

	// Fixed-step movement
	while (m_acc >= m_stepTime)
	{
		Step();
		m_acc -= m_stepTime;
	}
}

void SnakeGame::SetPendingDir(Dir dir)
{
	// Prevent instant reverse
	if (!IsOpposite(m_dir, dir))
		m_pendingDir = dir;
}

bool SnakeGame::IsGameOver() const
{
	return m_gameOver;
}

const Cell& SnakeGame::GetHead() const
{
	return m_snake.front();
}

const std::deque<Cell>& SnakeGame::GetBody() const
{
	return m_snake;
}

const Cell& SnakeGame::GetFood() const
{
	return m_food;
}

int SnakeGame::GetGridW() const
{
	return m_gridW;
}

int SnakeGame::GetGridH() const
{
	return m_gridH;
}

int SnakeGame::GetScore() const
{
	return int(m_snake.size()) - 3;
}

void SnakeGame::Step()
{
	// Commit direction once per step
	m_dir = m_pendingDir;

	Cell newHead = NextHead();

	// Death check before mutating body
	if (HitsWall(newHead) || HitsSelf(newHead))
	{
		m_gameOver = true;
		return;
	}

	m_snake.push_front(newHead);

	if (EatsFood(newHead))
		SpawnFood();
	else
		m_snake.pop_back();
}

Cell SnakeGame::NextHead() const
{
	Cell head = m_snake.front();

	switch (m_dir)
	{
	case Dir::Up:    head.y -= 1; break;
	case Dir::Down:  head.y += 1; break;
	case Dir::Left:  head.x -= 1; break;
	case Dir::Right: head.x += 1; break;
	}

	return head;
}

bool SnakeGame::IsOpposite(Dir a, Dir b) const
{
	return (a == Dir::Up && b == Dir::Down) ||
		(a == Dir::Down && b == Dir::Up) ||
		(a == Dir::Left && b == Dir::Right) ||
		(a == Dir::Right && b == Dir::Left);
}

bool SnakeGame::HitsWall(const Cell& c) const
{
	return c.x < 0 || c.x >= m_gridW ||
		c.y < 0 || c.y >= m_gridH;
}

bool SnakeGame::HitsSelf(const Cell& c) const
{
	for (const Cell& part : m_snake)
		if (part.x == c.x && part.y == c.y)
			return true;

	return false;
}

bool SnakeGame::EatsFood(const Cell& c) const
{
	return c.x == m_food.x && c.y == m_food.y;
}

void SnakeGame::SpawnFood()
{
	// Random empty cell
	unsigned seed = m_rngSeed;
	int fx, fy;
	bool valid;

	do
	{
		seed = (214013 * seed + 2531011);
		fx = (seed >> 16) % m_gridW;

		seed = (214013 * seed + 2531011);
		fy = (seed >> 16) % m_gridH;

		valid = true;
		for (const Cell& part : m_snake)
		{
			if (part.x == fx && part.y == fy)
			{
				valid = false;
				break;
			}
		}
	} while (!valid);

	m_food = { fx, fy };
	m_rngSeed = seed;
}