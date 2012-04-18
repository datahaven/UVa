/*
 * UVA_704_Colour_Hash.cpp
 *
 *  Created on: 12 Nov 2011
 *      Author: Adrian Dale
 *
 * Total Submissions Users that tried it Users that solved it
 * 3250 					750 			609
 * Your best accepted try
 * Ranking Submission Run Time Language Submission Date
 *     81     9486422 0.084      C++       2011-11-19 14:50:19
 *
 * Accepted first attempt. Reasonable run time, although first place run time is
 * 0.008 second, which is ten times faster than mine.
 * I had a bit of help with this in terms of a forum suggestion to try meet
 * in the middle search, and also a good set of test cases that found some
 * mistakes I'd perhaps not have spotted myself.
 * Although, being fair, without these I could still have used my slow full
 * search of the code to validate some more of my own test cases.
 */
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <fstream>
#include <assert.h>
#include <queue>
#include <map>
#include <stack>
using namespace std;

// A ull is 64 bits which will let us
// store colours 0-5 in 21 positions
typedef unsigned long long StateType;

const StateType SolvedState = 822067470497842320ull;

// Get colour at pos in state
inline int GetStateColour(StateType s, int pos)
{
	assert(pos>=0 && pos<=20);
	return (s >> (3ull*pos)) & 0x07ull;
}

StateType SetStateColour(StateType s, int pos, int col)
{
	assert(pos>=0 && pos<=20);
	assert(col>=0 && col<=5);
	StateType newstate = s;
	newstate &= static_cast<StateType>(~(0x07ull << (3ull*pos)));
	newstate |= (static_cast<StateType>(col) << (3ull*pos));
	return newstate;
}

// Turn a string of colours of positions into
// a StateType. Assume input string is correct. Assert otherwise
StateType MakeState(const string &stateStr)
{
	StateType s = 0;
	assert(sizeof(s)==8);
	istringstream parser(stateStr);
	int pieces = 0;
	while(pieces < 21) // NB don't need to read in last three pieces
	{
		assert( parser.bad() == false && parser.eof()==false);

		StateType p;
		parser >> p;

		assert(p>=0 && p<=10);

		int colour = (p+1)/2;
		assert(colour>=0 && colour <= 5);

		s |= (((p+1ull)/2ull) << (3ull*pieces));

		++pieces;
	}
	return s;
}

bool isValidState(StateType s)
{
	int counts[6];
	memset(counts, 0, sizeof(counts));

	for(int i=0; i<21; ++i)
	{
		StateType lsb = (s >> (3*i)) & 0x07;
		if ( !(lsb>=0 && lsb<=5))
			return false;
		counts[lsb]++;
	}
	if (counts[0] != 6)
		return false;
	for(int i=1; i<=5; ++i)
	{
		if (counts[i] != 3)
			return false;
	}
	return true;
}

void DumpState(StateType s)
{
	cout << "State=" << s << endl;
	for(int i=0; i<21; ++i)
	{
		StateType lsb = (s >> (3*i)) & 0x07;
		cout << lsb << " ";
	}
	cout << endl;
}

struct StateProperties
{
	StateType previousState;
	unsigned char depth;
	unsigned char previousMove;
};

typedef map<StateType, StateProperties> NodeMapType;

// Simple fn to return the opposite of the input move
// Needed because my code works backwards from the solution
// to the scrambled puzzle, but needs the answer to be
// presented the other way round
inline unsigned char flipMove(int d)
{
	assert(d>=1 && d<=4);
	switch(d)
	{
	case(1):
			return 3;
	case(2):
			return 4;
	case(3):
			return 1;
	case(4):
			return 2;
	};
	return 0;
}


// Return next state in the specified direction.
// direction must be from 1 to 4
StateType NextState(StateType s, int direction)
{
	assert(direction>=1 && direction<=4);

	// Different masks needed depending which wheel we're
	// calculating, due to overlap in the middle
	static const StateType LEFT_MASK_L = 0xFFFFFFFFF;
	static const StateType RIGHT_MASK_L = ~(LEFT_MASK_L);
	static const StateType LEFT_MASK_R = 0x7FFFFFF;
	static const StateType RIGHT_MASK_R = ~(LEFT_MASK_R);
	StateType leftWheel;
	StateType rightWheel;

	if (direction == 1)
	{
		// Left wheel clockwise
		leftWheel = s & LEFT_MASK_L;
		rightWheel = s & RIGHT_MASK_L;
		int col10 = GetStateColour(s, 10);
		int col11 = GetStateColour(s, 11);
		leftWheel <<= 6;
		leftWheel &= LEFT_MASK_L;
		leftWheel |= (col11 << 3);
		leftWheel |= (col10);
	}
	else if (direction == 3)
	{
		// Left wheel anti clockwise
		leftWheel = s & LEFT_MASK_L;
		rightWheel = s & RIGHT_MASK_L;
		int col0 = GetStateColour(s, 0);
		int col1 = GetStateColour(s, 1);
		leftWheel >>= 6;
		leftWheel = SetStateColour(leftWheel, 10, col0);
		leftWheel = SetStateColour(leftWheel, 11, col1);
	}
	else if (direction == 2)
	{
		// Right wheel clockwise
		leftWheel = s & LEFT_MASK_R;
		rightWheel = s & RIGHT_MASK_R;
		int col9 = GetStateColour(s, 9);
		int col10 = GetStateColour(s, 10);
		rightWheel >>= 6;
		rightWheel &= RIGHT_MASK_R;
		rightWheel = SetStateColour(rightWheel, 19, col9);
		rightWheel = SetStateColour(rightWheel, 20, col10);
	}
	else if (direction == 4)
	{
		// Right wheel anti-clockwise
		leftWheel = s & LEFT_MASK_R;
		rightWheel = s & RIGHT_MASK_R;
		int col20 = GetStateColour(s, 19);
		int col21 = GetStateColour(s, 20);
		rightWheel <<= 6;
		rightWheel &= RIGHT_MASK_R;
		rightWheel = SetStateColour(rightWheel, 10, col21);
		rightWheel = SetStateColour(rightWheel, 9, col20);
	}
	else
	{
		assert(false);
	}
	return (leftWheel | rightWheel) & 0x7fffffffffffffffull;
}

void ShowSolution(NodeMapType &Nodes, StateType inputState)
{
	if (inputState==SolvedState)
	{
		cout << "PUZZLE ALREADY SOLVED" << endl;
		return;
	}
	NodeMapType::iterator it = Nodes.find(inputState);
	// We shouldn't be calling this fn if solution isn't in Nodes
	assert(it != Nodes.end());

	unsigned char nextMove = it->second.previousMove;
	while( nextMove != 0 )
	{
		cout << static_cast<int>(flipMove(static_cast<int>(nextMove)));
		// Why is the flip needed here and not below?
		// I think it is because RevNodes was built searching back to front,
		// and perhaps shouldn't have been, but should have been flipped.
		it = Nodes.find(it->second.previousState);
		assert(it != Nodes.end());
		nextMove = it->second.previousMove;
	}
	cout << endl;

}

void ShowSolutionFull(NodeMapType &Nodes, NodeMapType &RevNodes, StateType inputState)
{
	// Look through rev node map to see where it overlaps node map
	NodeMapType::iterator nit = Nodes.end();
	NodeMapType::iterator rnit;

	NodeMapType::iterator best_nit = Nodes.end();
	NodeMapType::iterator best_rnit = RevNodes.end();
	int best_depth = 1000;

	for( rnit = RevNodes.begin(); rnit != RevNodes.end(); ++rnit)
	{
		// Only need to check the deepest nodes (I think!)
		//if (rnit->second.depth == 8)
		//{
			nit = Nodes.find(rnit->first);
			if (nit != Nodes.end())
			{
				// Solution Found
				if (rnit->second.depth + nit->second.depth < best_depth)
				{
					best_nit = nit;
					best_rnit = rnit;
					best_depth = rnit->second.depth + nit->second.depth;
				}

			}
		//}
	}

	// Swapping of iterators to save a bit of typing
	// TODO - refactor.
	nit = best_nit; rnit = best_rnit;

	if (nit != Nodes.end())
	{
		// The first part of the solution is in RevNodes
		// and will be back to front, so pop it on a stack to
		// reverse it
		stack<int> S;
		unsigned char nextMove = rnit->second.previousMove;
		while( nextMove != 0 )
		{
			S.push(static_cast<int>(nextMove));
			rnit = RevNodes.find(rnit->second.previousState);
			assert(rnit != RevNodes.end());
			nextMove = rnit->second.previousMove;
		}
		// Now pop them off and they'll be in the right order
		while( S.empty() == false)
		{
			cout << S.top();
			S.pop();
		}

		// Now tag on the second half of the solution, taken from our
		// pre-calculated map
		nextMove = nit->second.previousMove;
		while( nextMove != 0 )
		{
			cout << static_cast<int>(flipMove(static_cast<int>(nextMove)));
			nit = Nodes.find(nit->second.previousState);
			assert(nit != Nodes.end());
			nextMove = nit->second.previousMove;
		}
		cout << endl;
	}
	else
		cout << "NO SOLUTION WAS FOUND IN 16 STEPS" << endl;
}

// Breadth First Search
void BFS(NodeMapType &Nodes, int maxdepth, bool searchUp, StateType startState)
{
	queue<StateType> Q;
	Nodes.clear();

	StateProperties ssp;
	ssp.depth = 0;
	ssp.previousMove = 0;
	ssp.previousState = 0ull;
	StateType s = startState;

	Nodes.insert( make_pair<StateType, StateProperties>(s, ssp));
	Q.push(s);
	while( Q.empty() == false)
	{
		StateType u = Q.front();
		Q.pop();

		// Need to do a depth check at u>16?
		// Will put it in the v loop below
		NodeMapType::iterator uit = Nodes.find(u);
		assert(uit != Nodes.end());

		// Should sometimes get different solutions if I try states
		// in a different order.
		int startD = 1;
		int endD = 5;
		int dir = 1;
		//searchUp = true; // TODO - TESTING
		if (searchUp == false)
		{
			startD = 4;
			endD = 0;
			dir = -1;
		}

		// for_each v in states adjacent to u
		// ie our four different wheel turns away from u
		for( int d=startD; d != endD; d += dir)
		{
			StateType v = NextState(u, d);
			map<StateType, StateProperties>::iterator it = Nodes.find(v);
			if ( it == Nodes.end() )
			{
				// This is equivalent to WHITE, since Nodes stores states
				// we have visited or are in the process of visiting.
				// Hence we don't need to explicitly track colour
				StateProperties np;

				np.depth = uit->second.depth+1;
				np.previousState = u;
				//if (searchUp == true)
					np.previousMove = d;
				//else
				//	np.previousMove = flipMove(d);

				// We need to ensure this is the lowest numbered solution,
				// if more than one exists. I think that by searching in 1-4
				// order we'll always get them in the right order
				// TODO - need to find a test case.
				// Only search further if we haven't reached the maximum
				// depth we're looking for
				if (np.depth <= maxdepth) // Should be 16 but TOO SLOW
				{
					Nodes.insert( make_pair<StateType, StateProperties>(v, np));
					Q.push(v);
				}
			}
		}
	}
}

void ReadTestCases()
{
	NodeMapType Nodes;
	// First search from solved down to depth 8 is
	// the same for all puzzles
	BFS(Nodes, 8, false, SolvedState);

	string line;
	getline(cin, line);
	istringstream parser(line);

	int TestCases;
	parser >> TestCases;
	while(TestCases-- > 0)
	{
		getline(cin, line);
		parser.str(line);
		parser.clear();

		StateType inputState = MakeState(line);
		assert(isValidState(inputState));
		//DumpState(inputState);

		// See if we got lucky and found a solution that needed
		// 8 moves, or less, and therefore already has a pre-computed solution
		NodeMapType::iterator nit = Nodes.find(inputState);
		if (nit != Nodes.end())
		{
			// Solution found within 8 moves, so no need
			// to search backwards to meet in the middle
			ShowSolution(Nodes, inputState);
		}
		else
		{
			// No solution within 8 moves, so
			// need to search from the end and meet in the middle
			NodeMapType ReverseNodes;
			BFS(ReverseNodes, 8, true /*false*/, inputState);
			ShowSolutionFull(Nodes, ReverseNodes, inputState);
		}
	}
}

void SelfTest()
{
	cout << "Self Test Start" << endl;

	// All possible combinations for Get/SetStateColour
	bool fail = false;
	for(int i=0; i<=20; ++i)
	{
		for(int j=0; j<=5; ++j)
		{
			StateType s1 = 0;
			s1 = SetStateColour(s1, i, j);
			if (GetStateColour(s1, i) != j)
			{
				fail = true;
				cout << "FAIL: " << i << " " << j << endl;
			}
		}
	}
	assert(fail==false);

	assert(isValidState(SolvedState));
	DumpState(SolvedState);
	StateType ns = NextState(SolvedState, 1);
	assert(isValidState(ns));
	//DumpState(ns);
	ns = NextState(SolvedState, 2);
	//DumpState(ns);
	assert(isValidState(ns));
	ns = NextState(SolvedState, 3);
	//DumpState(ns);
	assert(isValidState(ns));
	ns = NextState(SolvedState, 4);
	//DumpState(ns);
	assert(isValidState(ns));

	// Some rotate back/forward tests
	StateType ns1 = NextState(SolvedState,1);
	StateType ns2 = NextState(SolvedState,2);
	StateType ns3 = NextState(SolvedState,3);
	StateType ns4 = NextState(SolvedState,4);
	assert( NextState(ns1,3) == SolvedState);
	assert( NextState(ns3,1) == SolvedState);
	DumpState( NextState(ns2,3));
	assert( NextState(ns2,4) == SolvedState);
	assert( NextState(ns4,2) == SolvedState);

	// six spins brings us back to begining
	StateType ns0 = SolvedState; ns1 = ns0; ns2 = ns0; ns3 = ns0;
	for(int i=0; i<6; ++i)
	{
		ns0 = NextState(ns0, 1);
		ns1 = NextState(ns1, 2);
		ns2 = NextState(ns2, 3);
		ns3 = NextState(ns3, 4);
	}
	assert(ns0==SolvedState);
	assert(ns1==SolvedState);
	assert(ns2==SolvedState);
	assert(ns3==SolvedState);

	cout << "BUG CASE" << endl;
	ns = 151574863642286808ull;
	DumpState(ns);


	int moves[]={1,1,1,2,2,1,1,1,2,3,2,3};
	for(size_t i=0; i<sizeof(moves)/sizeof(moves[0]); ++i)
		ns = NextState(ns, moves[i]);
	DumpState(ns);
	// 111441114141 is a valid solution, whereas mine is nowhere near! Now Fixed.


	cout << "Self Test End" << endl;
}

int main()
{
//#define DEBUG_FILE 1

#ifdef DEBUG_FILE
	cout << "WARNING: DEBUG BUILD" << endl;
	// Switch so we read our input from a file.
	// Means I don't have to set up a command line in visual studio to
	// run with the debugger
	streambuf *backup, *psbuf;
	ifstream filestr;
	filestr.open("test.txt");
	backup = cin.rdbuf();
	psbuf = filestr.rdbuf();
	cin.rdbuf(psbuf);
#endif

	// SelfTest();

	ReadTestCases();


#ifdef DEBUG_FILE
	// Put our input stream back to normal
	// Not terribly important if we're about to exit anyway!
	cin.rdbuf(backup);
	filestr.close();
#endif

	return 0;
}
