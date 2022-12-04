#include "WLApp_Snake.h"
#include "WLApp.h"
#include "WLSystem.h"

#ifdef _WIN32
inline int random(int max)
{
	return rand() % max;
}
#endif

namespace WoodLight
{
	namespace {
		class  XY
		{
		public:
			XY()
			{}

			XY(int x_, int y_)
				: x(x_),
				y(y_)
			{}

			inline bool operator==(const XY& b) const
			{
				return x == b.x && y == b.y;
			}
		public:
			uint16_t
				x : 12,
				y : 4;
		};

		static const int XDir[] = { 1, 0, -1, 0 };
		static const int YDir[] = { 0, -1, 0, 1 };

		class SnakeApp : public App
		{
		public:
			

			class Snake
			{
			public:
				void reset(XY xy, int dir)
				{
					writeIndex = tailIndex = 0;
					addPoint(xy);
					this->dir = dir;
				}

				void addPoint(XY xy)
				{
					if ((writeIndex + 1) % positions.size() != tailIndex)
					{
						positions[writeIndex] = xy;
						writeIndex = (writeIndex + 1) % positions.size();
					}
				}

				XY dropPoint()
				{
					auto result = XY{};
					if (tailIndex != writeIndex)
					{
						result = positions[tailIndex];
						tailIndex = (tailIndex + 1) % positions.size();
					}
					return result;
				}

				XY headPos() const
				{
					return positions[(writeIndex + positions.size() - 1) % positions.size()];
				}

				bool contains(XY xy)
				{
					for (auto index = tailIndex; index != writeIndex; index = (index + 1) % positions.size())
						if (positions[index] == xy)
							return true;
					return false;
				}

				void turnLeft()
				{
					dir = (dir + 1) % 4;
				}

				void turnRight()
				{
					dir = (dir + 3) % 4;
				}

				void draw(Display& display, bool value)
				{
					for (auto i = tailIndex; i != writeIndex; i = (i + 1) % positions.size())
						display.setPixel(positions[i].x, positions[i].y, value);

				}
				void move(Display& display, bool value)
				{
					const auto hp = headPos();

					const auto nextPos = XY{
						(hp.x + Display::Columns + XDir[dir]) % Display::Columns,
						(hp.y + Display::Rows + YDir[dir]) % Display::Rows
					};

					display.setPixel(nextPos.x, nextPos.y, value);
					addPoint(nextPos);

					if (growBuffer == 0)
					{
						removedPos = positions[tailIndex];
						display.setPixel(removedPos.x, removedPos.y, !value);

						tailIndex = (tailIndex + 1) % positions.size();
					} else
					{
						growBuffer -= 1;
					}
				}

				void grow(int i)
				{
					growBuffer += i;
				}

			public:
				XY removedPos;
				std::array<XY, Display::Rows* Display::Columns> positions;
				int writeIndex = 0;
				int tailIndex = 0;
				int dir = 0;
				int growBuffer = 0;
			};

		public:
			SnakeApp()
			{
				// Lange Tastendr�cke nicht �berwachen damit wir die Eingaben schneller bekommen
				supportsLongKeyPresses = false;
			}

			virtual void initialize(System& system) override
			{
				system.display.clear(false);
				system.display.print(0, 0, true, "1");
				system.display.print(25, 0, true, "Wieviele Spieler?");
				system.display.print(system.display.Columns - system.display.getTextWidth("2"), 0, true, "2");

				f = 0;
			}

			virtual void update(System& system) override
			{
				if (f == 0)
				{
					while (auto key = system.getkey())
					{
						switch (key)
						{
						case System::LowerLeftKeyCode:
						case System::UpperLeftKeyCode:
							singlePlayer = true;
							f = 1;
							break;

						case System::LowerRightKeyCode:
						case System::UpperRightKeyCode:
							singlePlayer = false;
							f = 1;
							break;
						}
					}
				}

				if (f == 1)
				{
					system.display.clear(false);
					//system.display.printSmall(0, 0, true, "0123456789");

					snakes[0].reset({ 20, 3 }, 0);
					snakes[0].addPoint({ 21, 3 });
					snakes[0].addPoint({ 22, 3 });

					snakes[1].reset({ Display::Columns - 20, 4 }, 2);
					snakes[1].addPoint({ Display::Columns - 21, 4 });
					snakes[1].addPoint({ Display::Columns - 22, 4 });

					snakes[0].draw(system.display, true);
					if (!singlePlayer)
						snakes[1].draw(system.display, true);

					newTarget(system.display);
					system.display.setPixel(target.x, target.y, true);

					f = 2;
				}

				if (f == 2)
				{
					while (auto key = system.getkey())
					{
						switch (key)
						{
						case System::UpperLeftKeyCode: snakes[0].turnLeft(); break;
						case System::LowerLeftKeyCode: snakes[0].turnRight(); break;
						case System::UpperRightKeyCode: snakes[1].turnRight(); break;
						case System::LowerRightKeyCode: snakes[1].turnLeft(); break;
						}
					}

					if (++speedCount >= speed)
					{
						speedCount = 0;

						for (auto snakeIndex = singlePlayer ? 0 : 1; snakeIndex >= 0; --snakeIndex)
						{
							auto& snake = snakes[snakeIndex];

							snake.move(system.display, true);
							if (snake.headPos() == target)
							{
								snake.grow(3);
								newTarget(system.display);
							}
						}
					}

				}

			}

			void newTarget(Display& display)
			{
				for(auto valid = false; !valid; )
				{
					target = XY{ Display::Columns / 3 + random(Display::Columns / 3), random(Display::Rows) };
					valid = true;

					for (auto snakeIndex = singlePlayer ? 0 : 1; snakeIndex >= 0; --snakeIndex)
						if (snakes[snakeIndex].contains(target))
							valid = false;
				}

				display.setPixel(target.x, target.y, true);
			}

		private:

			int f = 0;

			bool singlePlayer = true;
			int speed = 5;
			int speedCount = 0;

			std::array<Snake, 2> snakes;

			XY target;


			//auto 
		};
	}

	App* createSnakeApp()
	{
		return new SnakeApp;
	}
}
