#pragma once
#include "..\Robo.h"
#include "..\DebugControls.h"
#include <random>
#include <deque>


class RoboAI_Demo
{
	using TT = TileMap::TileType;
	using Action = Robo::Action;
public:
	Action Plan(std::array<TT, 3> view)
	{
		if (!winner)
		{
			//two step movement check
			if (do_next_move && !(std::any_of(view.begin(), view.end(), [](auto a) {return a == TT::Goal; })))
			{
				do_next_move = false;

				stm = { view[0],view[1],view[2] };

				last_move = next_move;
				return next_move;
			}

			//if no accesible exit at sight do generic checks and movement, else, move towards exit (after accesible exit check)
			if (!exit_at_sight)
			{
				//check if 'exit at sight' (and if the dude have at least one memory of the path behing)
				if (std::any_of(view.begin(), view.end(), [](auto a) {return a == TT::Goal; }))
				{
					if (first_movement) //could be a trap, must to check
					{
						first_movement = false;

						if (view[1] == TT::Goal)
						{
							//lucky dude
							exit_at_sight = true;
						}
						else
						{
							//kickoff movement because we can't be sure if exit is accesible
							stm = { view[0],view[1],view[2] };

							if (view[0] == TT::Goal)
							{
								last_move = Action::TurnLeft;
								return Action::TurnLeft;
							}
							else
							{
								last_move = Action::TurnRight;
								return Action::TurnRight;
							}
						}
					}
					else
					{
						//standard 'exit at sight' verification
						if (view[1] != TT::Wall)
						{
							exit_at_sight = true;
						}
						else
						{
							if (view[2] == TT::Goal)
							{
								if (last_move == Action::TurnLeft)
								{
									if (stm[1] == TT::Wall)
									{
										stm = { view[0],view[1],view[2] };

										last_move = Action::TurnLeft;
										return Action::TurnLeft;
									}
									else
									{
										exit_at_sight = true;
									}
								}
								
							}
							else if(view[0] == TT::Goal)
							{
								if (last_move == Action::TurnRight)
								{
									if (stm[0] == TT::Wall)
									{
										stm = { view[0],view[1],view[2] };

										last_move = Action::TurnRight;
										return Action::TurnRight;
									}
									else
									{
										exit_at_sight = true;
									}
								}
							}
						}
					}
				}
				else
				{
					if (first_movement) first_movement = false;

					stm = { view[0],view[1],view[2] };

					//standard movement
					if (view[2] == TT::Floor) //Checks if turn to right is allowed
					{
						if (view[1] != TT::Wall) //if a wall is in the face of the dude, must be an aisle or only left turn available
						{
							do_next_move = true;

							next_move = Action::TurnRight;

							last_move = Action::MoveForward;
							return Action::MoveForward;
						}
						else
						{
							last_move = Action::TurnLeft;	
							return Action::TurnLeft; //exceptional case must to find a way
						}
					}
					else //no turning right allowed
					{
						if (view[1] == TT::Floor) //continue straight if possible
						{
							last_move = Action::MoveForward;
							return Action::MoveForward;
						}
						else //must be possible to turn left and see what happens
						{
							last_move = Action::TurnLeft;
							return Action::TurnLeft;
						}
					}
				}
			}
			
			stm = { view[0],view[1],view[2] };
			
			if(exit_at_sight)
			{
				//moving towards exit
				int i = 0;
				for (; view[i] != TT::Goal ; i++);

				switch (i)
				{
				case 0:
					next_move = Action::TurnLeft;
					do_next_move = true;
					return Action::MoveForward;
				case 1:
					winner = true;
					return Action::MoveForward;
				case 2:
					next_move = Action::TurnRight;
					do_next_move = true;
					return Action::MoveForward;
				}
			}
		}
		
		return Action::Done;
	}
private:
	Action next_move;
	Action last_move;
	bool do_next_move = false;
	bool exit_at_sight = false;
	bool winner = false;
	bool first_movement = true;

	std::array<TT, 3> stm; //dude's short-term memory
};

// use this if you don't want to implement a debug RoboAI
class RoboAIDebug_Unimplemented
{
	using TT = TileMap::TileType;
	using Action = Robo::Action;
public:
	RoboAIDebug_Unimplemented( DebugControls& dc )
	{}
	// this signals to the system whether the debug AI can be used
	static constexpr bool implemented = false;
	Action Plan( std::array<TT,3> view )
	{
		assert( "RoboAIDebug is not implemented" && false );
		return Action::TurnLeft;
	}
};

// demo of how to use DebugControls for visualization
class RoboAIDebug_Demo
{
	using TT = TileMap::TileType;
	using Action = Robo::Action;
public:
	RoboAIDebug_Demo( DebugControls& dc )
		:
		dc( dc )
	{}
	static constexpr bool implemented = false;
	Action Plan( std::array<TT,3> view )
	{
		if( view[1] == TT::Wall )
		{
			if( coinflip( rng ) )
			{
				return Action::TurnRight;
			}
			else
			{
				return Action::TurnLeft;
			}
		}
		dc.MarkAt( dc.GetRobotPosition(),{ Colors::Green,32u } );
		return Action::MoveForward;
	}
private:
	DebugControls& dc;
	std::mt19937 rng = std::mt19937( std::random_device{}() );
	std::bernoulli_distribution coinflip;
};

// if you name your classes different than RoboAI/RoboAIDebug, use typedefs like these
typedef RoboAI_Demo RoboAI;
typedef RoboAIDebug_Demo RoboAIDebug;