// Adrian Dale
// 11/11/2011
// Solves online judge problem: 10422 - Knights in FEN
// Solved in 0.080s, ranked 279 out of 976 users who tried, 881 who
// solved and 3735 submission.
// Solved first attempt
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <assert.h>
using namespace std;

typedef unsigned int StateType;
// Define this here as I've got it wrong twice before
const int BOARD_START_BIT = 5;
const StateType BAD_STATE = 0xFFFFFFFF;

inline int GetSqPos(const StateType s)
{
	return s & 0x1F;
}

// Debug code that makes sure StateType s
// is valid
bool StateCheck(const StateType s)
{
	int sqpos = GetSqPos(s);
	// Check square is in correct range
	if (sqpos < 0 || sqpos > 24)
		return false;
	// Check bit under square is a zero
	unsigned int bitmask = 1 << (sqpos+BOARD_START_BIT);
	if ( (s & bitmask) != 0 )
		return false;

	// Check there are 12 one bits
	int bc=0;
	for(int i=0; i<25; ++i)
	{
		bitmask = 1 << (i+BOARD_START_BIT);
		if ( (s & bitmask) != 0 )
			++bc;
	}
	return bc==12;
}

// Reachable is a mapping that shows the
// squares a knight can move to from each
// square on the board.
// A minor speed optimisation - I could just
// calculate this on the fly.
vector< vector<int> > Reachable;

// Directions used so I can handle directions
// in a loop, rather than repeating code
const int DX[]={1,2, 2, 1,-1,-2,-2,-1};
const int DY[]={2,1,-1,-2,-2,-1, 1, 2};

// Fill the Reachable vector
void FillReachable(int n = 5)
{
	for(int i=0; i<n; ++i)
	{
		for(int j=0; j<n; ++j)
		{
			vector<int> inRange;
			for(size_t d=0; d<sizeof(DX)/sizeof(int); ++d)
			{
				if (i+DY[d] < 0 || i+DY[d] >= n || j+DX[d] < 0 || j+DX[d] >= n )
					continue;
				inRange.push_back( (i+DY[d])*n + (j+DX[d]) );
			}
			Reachable.push_back(inRange);
		}
	}
}

// Dump out the board represented by StateType s
void DumpState(StateType s)
{
	cout << "S=" << static_cast<unsigned int>(s) << endl;
	int sqpos = GetSqPos(s);
	for(int bpos=1; bpos<=25; ++bpos)
	{
		if ( bpos-1 == sqpos )
			cout << " ";
		else
		{
			unsigned int bitmask = 1 << (bpos-1+BOARD_START_BIT);

			if ( (s & bitmask) != 0 )
				cout << "1";
			else
				cout << "0";
		}
		if (bpos%5==0)
			cout << endl;
	}
	cout << endl;
}

// Read in our starting state from standard input
StateType readState()
{
	StateType res = 0;
	string data;

	for(int i=0; i<5; ++i)
	{
		string line;
		getline(cin, line);
		data += line;
	}

	if (data.size()!=25)
		return BAD_STATE;

	int sqpos = -1;
	int blackCount = 0;
	for(int i=0; i<25; ++i)
	{
		if (data[i] == ' ')
			sqpos = i;
		else if (data[i] == '1')
		{
			unsigned int bitmask = 1 << (i+BOARD_START_BIT);
			res |= bitmask;
			++blackCount;
		}
	}
	res |= sqpos;

	if (sqpos == -1)
		return BAD_STATE;

	if (blackCount != 12)
		return BAD_STATE;

	return res;
}

// Given StateType s, return a vector containing all
// the states (ie board positions) we can reach
// from this state
vector<StateType> nextStates(StateType s)
{
	vector<StateType> res;

	int sqpos = GetSqPos(s);
	assert(sqpos >= 0 && sqpos < 25);
	// Start from the space position and try out each
	// state where a knight can be moved into the space.
	// Reachable says which squares can swap with the space
	for( size_t i=0; i<Reachable[sqpos].size(); ++i)
	{
		// Set new space position and read whether that square
		// currently contains a black or a white knight
		int newsqpos = Reachable[sqpos][i];
		bool fromBit = (s & (1 << (newsqpos+BOARD_START_BIT))) != 0;

		// Store the new space position in our new state
		StateType newstate = s;
		newstate &= ~(0x1F);
		newstate |= newsqpos;
		// Clear out the bit under the space
		newstate &= ~(1<<(newsqpos+BOARD_START_BIT));
		// Move the knight into our old space position
		if (fromBit==true)
			newstate |= (1<<(sqpos+BOARD_START_BIT));
		else
			newstate &= ~(1<<(sqpos+BOARD_START_BIT));

		res.push_back(newstate);
	}
	return res;
}


typedef map<StateType,unsigned char> VisitedType;

// Visited will store each possible board state,
// along with the distance away from the starting state
VisitedType Visited;

// Recursively populate the Visited map, starting at StateType s
bool Explore(StateType s, unsigned char depth)
{
	// Cut off at our maximum depth
	// ie must be solvable in 10 moves or less, else we print
	// unsolvable message
    if (depth == 11)
		return true;
    // I'm curious what the max depth is. I'm sure I could
    // work out how many states there are with a pen and paper.
    // I think it is 25!/(12!*12!) = 67,603,900
    // Would need to implement bfs version to check this.
    // Code crashes if I take out the above if.
    // Not entirely sure why, either.
    bool goDeeper = true;

	// See if we've seen this state before
	VisitedType::iterator it = Visited.find(s);
	if (it == Visited.end())
	{
		// If we haven't seen this state before, then add it
		// to our map, and Explore the next states that we can
		// reach from this one.
		// It's a depth first search.
		Visited.insert( make_pair<StateType,unsigned char>(s, depth) );
	}
	else
	{
		// We've seen this state before, but might need
		// to update the depth if we find we can reach it by
		// a quicker route.
		// Note that we have to search all over again if we update
		// the depth for this state, otherwise all the states
		// that depend on this one will be incorrect.
		// I think that means this really ought to be a breadth-
		// first search rather than depth first.
		// Or would that be just as bad?
		if (it->second > depth)
			it->second = depth;
		else
			goDeeper = false;
	}

	if (goDeeper == true)
	{
		vector<StateType> ns = nextStates(s);
		for(size_t i=0; i<ns.size(); ++i)
		{
			assert(StateCheck(ns[i]));
			Explore( ns[i], depth+1 );
		}
	}

	return true;
}

// Read the test cases in from the file and look them up in
// our pre-computed map
void ReadTestCases()
{
	string line;
	getline(cin, line);
	istringstream parser(line);
	int NoOfTests;
	parser >> NoOfTests;
	//assert(NoOfTests < 14);

	while(NoOfTests-- > 0)
	{
		StateType s = readState();

		// Judge might try to fob us off with garbage. Spec doesn't
		// exactly say he can't. Got this hint from the forum
		if (s==BAD_STATE)
		{
			cout << "Unsolvable in less than 11 move(s)." << endl;
			continue;
		}
		assert(StateCheck(s));

		VisitedType::iterator it = Visited.find(s);
		if (it != Visited.end())
			cout << "Solvable in " << static_cast<int>(it->second) << " move(s)." << endl;
		else
			cout << "Unsolvable in less than 11 move(s)." << endl;
	}
}

void SelfTest()
{
#if 0
	// Quick test to make sure I like the look of
	// the Reachable vector
	for(int i=0; i<25; ++i)
	{
		cout << "R[" << i << "]=";
		for(size_t j=0; j<Reachable[i].size(); ++j)
			cout << Reachable[i][j] << " ";
		cout << endl;
	}
#endif
	assert(Reachable.size() == 25);
	assert(StateCheck(0xffffffff)==false);
}

// Dumps out a C array declaration of hard-coded values
// which will be used in the "instant" version of this code.
// Yes, I think this is cheating.
void MakeCheatFile()
{
	cout << "unsigned int StateLookup[]={" << endl;
	int colCount=1;
	for(VisitedType::iterator it = Visited.begin(); it != Visited.end(); ++it)
	{
		 cout << it->first << ",";
		 if (colCount++ % 7 == 0)
		 {
			 cout << endl;
			 colCount=1;
		 }
	}
	cout << "0xFFFFFFFF};" << endl;

	cout << "unsigned char DepthLookup[]={" << endl;
	colCount=1;
	for(VisitedType::iterator it = Visited.begin(); it != Visited.end(); ++it)
	{
		 cout << static_cast<int>(it->second) << ",";
		 if (colCount++ % 25 == 0)
		 {
			 cout << endl;
			 colCount=1;
		 }
	}
	cout << "0xFFFFFFFF};" << endl;
}

int main()
{
	FillReachable();

	// Explore backwards from the goal state
	Explore(17595372, 0);

#ifndef ONLINE_JUDGE
	SelfTest();
#endif

	//MakeCheatFile();

	ReadTestCases();

	return 0;
}
