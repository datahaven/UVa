/*
 * UVA_321_The_New_Villa.cpp
 *
 *  Created on: 12 Nov 2011
 *      Author: Adrian Dale
 *
 *     Total Submissions Users that tried it Users that solved it
 *        2669 					749 			582
 * Your best accepted try
 * Ranking Submission Run Time Language Submission Date
 *   319 	9466290 	0.020 	C++ 	2011-11-12 20:18:00
 *
 * Another one that got Accepted first go!
 */
#include <iostream>
#include <string>
#include <cstring> // for memset
#include <sstream>
#include <queue>
#include <vector>
#include <algorithm>
#include <assert.h>
using namespace std;

const int MAX_ROOMS = 10 + 1;

// State represents how the lights are switched,
// plus where our character is located.
// Bits 0-3 for location
// Bits 4-13 for switches
const int BITS_START = 4;
typedef unsigned int StateType;

// Adjacency matrix representation of connected doors
// and switches. Probably quickest, since we know there are a
// max of ten rooms (Numbered 1 to 10)
// 0 means no connection.
// 1 means connection
// Then again, with hindsight, probably not!
int Doors[MAX_ROOMS][MAX_ROOMS];
int Switches[MAX_ROOMS][MAX_ROOMS];

// Wasting twice as much spaces as necessary because
// I've got the "gap" bit where room==0 which is unused.
// Really ought to fix this but it shouldn't matter for this code
const int MAX_STATES = 32751 + 1;

const int BIG_NUM = 0x7FFFFFFF;

// Data used to hold our search data.
// See page 595 of Cormen, Leiserson, Rivest, Stein
// Introduction to Algorithms
enum ColourType {WHITE, GRAY, BLACK};

ColourType StateColour[MAX_STATES];
int StateDepth[MAX_STATES];
StateType StatePrevious[MAX_STATES];

void DumpState(StateType s)
{
	int room = s & 0x0F;
	cout << "State=" << s << " in room " << room << endl;
	for(int i=1; i<MAX_ROOMS; ++i)
	{
		if ( (s & (1<<(BITS_START+i))) != 0 )
			cout << "1";
		else
			cout << "0";
	}
	cout << endl;
}

bool isValidState(StateType s)
{
	int room = s & 0x0F;
	if (room<1 || room>10)
		return false;
	if ( (s & (1<<BITS_START)) != 0 )
		return false;
	return true;
}

// Which states can we reach from s?
void NextStates(vector<StateType> &results, StateType s)
{
	assert(isValidState(s)==true);
	results.clear();

	int room = s & 0x0F;
	// We can either switch a light, or move to another room,
	// but not both
	// - try doors
	for(int i=1; i<MAX_ROOMS; ++i)
	{
		if (Doors[room][i] == 1)
		{
			// Can go to room i, but only if the
			// light is on in that room
			assert(room != i);
			if ((s & (1 << (BITS_START+i))) != 0)
			{
				StateType newState = s & ~(0x0F);
				newState |= i;
				assert(isValidState(newState));
				results.push_back(newState);
			}
		}
	}
	// - try lights
	for(int i=1; i<MAX_ROOMS; ++i)
	{
		if (Switches[room][i] == 1)
		{
			// Can switch the light in room i
			// unless we are in that room
			// (which won't happen as I've removed those switches earlier)
			assert(room != i);
			StateType newState = s;
			newState ^= (1 << (BITS_START+i));
			assert(isValidState(newState));
			results.push_back(newState);
		}
	}
}

// r is the number of the room we need to reach.
// Return true if we can find our goal,
// false otherwise. Using Breadth First Search
bool Solve(int r)
{
	// We start in room 1, all lights off, except in room 1
	StateType start = 1;
	start |= (1 << (BITS_START+1));
	// We'd like to finish in room r with all lights off,
	// except in room r
	StateType goal = r;
	goal |= (1 << (BITS_START+r));

	// NB - does it help to check to see if there are any switches
	// in room r? If not then we can never switch off the other lights.

	// Initialise our solution data graph
	for(size_t i=0; i<sizeof(StateDepth)/sizeof(int); ++i)
	{
		StateColour[i] = WHITE;
		StateDepth[i] = BIG_NUM;
		StatePrevious[i] = StateType(0);
	}

	StateColour[start] = GRAY;
	StateDepth[start] = 0;
	StatePrevious[start] = 0;
	queue<StateType> Q;
	Q.push(start);

	while( Q.empty() == false )
	{
		StateType u = Q.front();
		Q.pop();
		assert(isValidState(u)==true);

		if (u == goal)
			return true;

		// Need to do for_each(state reachable from u)
		vector<StateType> Reachable;
		NextStates(Reachable, u);
		for( size_t i=0; i<Reachable.size(); ++i)
		{
			StateType v = Reachable[i];
			assert(isValidState(v)==true);

			if ( StateColour[v] == WHITE )
			{
				StateColour[v] = GRAY;
				StateDepth[v] = StateDepth[u]+1;
				StatePrevious[v] = u;
				Q.push(v);
			}
		}
		StateColour[u] = BLACK;
	}

	return false;
}

void ShowSolution(int r)
{
	StateType goal = r;
	goal |= (1 << (BITS_START+r));

	cout << "The problem can be solved in " << StateDepth[goal]
	     << " steps:" << endl;

	StateType current = goal;
	vector<StateType> Moves;
	while(current != 0)
	{
		Moves.push_back(current);
		current = StatePrevious[current];
	}
	// Solution comes out back to front, so need to reverse it
	reverse(Moves.begin(), Moves.end());

	for(size_t i=1; i<Moves.size(); ++i)
	{
		StateType s1 = Moves[i-1];
		StateType s2 = Moves[i];
		int r1 = s1 & 0x0F;
		int r2 = s2 & 0x0F;
		if (r1 != r2)
		{
			cout << "- Move to room " << r2 << "." << endl;
		}
		else
		{
			// One of the lights must have changed.
			for( int j=1; j<MAX_ROOMS; ++j)
			{
				if ( (s1 & (1 << (BITS_START+j))) !=
						(s2 & (1 << (BITS_START+j))) )
				{
					cout << "- Switch ";
					cout << (( (s2 & (1 << (BITS_START+j))) != 0) ? "on" : "off");
					cout << " light in room " << j << "." << endl;
				}
			}
		}

	}
}

void ReadTestCases()
{
	string line;
	int TestNo = 1;
	while(getline(cin, line))
	{
		int r, d, s;
		istringstream parser(line);
		parser >> r >> d >> s;

		if (r==0 && d==0 && s==0)
			break;

		// Initialise Doors/Switches
		memset(Doors, 0, sizeof(Doors));
		memset(Switches, 0, sizeof(Switches));

		// Read doors
		while( d-- > 0)
		{
			getline(cin, line);
			parser.str(line);
			parser.clear();
			int i, j;
			parser >> i >> j;

			assert(i>=1 && i<=r);
			assert(j>=1 && j<=r);

			// I assume that if there is a door between rooms i and j that
			// one can go through it in either direction
			// Remove useless doors - a shortest solution would never having
			// us use a door with a loop in it
			if (i != j)
			{
				Doors[i][j] = 1;
				Doors[j][i] = 1;
			}
		}

		// Read switches
		while( s-- > 0)
		{
			getline(cin, line);
			parser.str(line);
			parser.clear();
			int k, l;
			parser >> k >> l;

			assert(k>=1 && k<=r);
			assert(l>=1 && l<=r);
			// NB Switches aren't symmetric like doors are
			// We can never use a switch that switches the room we are in,
			// as we can only go in rooms where the lights are on
			if (k != l)
				Switches[k][l] = 1;
		}
		// Remove blank line between villa descriptions
		getline(cin, line);

		cout << "Villa #" << TestNo++ << endl;

		if (Solve(r)==true)
			ShowSolution(r);
		else
			cout << "The problem cannot be solved." << endl;

		// Text clearly says "Output a blank line after each test case"
		// So, I'm putting one after every case, including the last one
		cout << endl;
	}
}

void SelfTest()
{


}

int main()
{
	ReadTestCases();

#ifndef ONLINE_JUDGE
	SelfTest();
#endif

	// Test ideas - villas with doors that connect to themselves
	// - switches that switch the light of the room they're in
	return 0;
}

