/*
 * UVA_10085_Most_Distant_State.cpp
 *
 *  Created on: 19 Nov 2011
 *      Author: Adrian Dale
 *
 * Accepted on second try.
 * First failure was an annoying lack of attention, printing a space, instead of a zero
 * in the puzzle definition.
 *
 * Total Submissions Users that tried it Users that solved it
 *   1085 				403 				340
 * Your best accepted try
 * Ranking Submission Run Time Language Submission Date
 *    198      9486354 5.312     C++     2011-11-19 14:28:00
 *
 * Think I could easily beat that run time by pre-calculating and assuming
 * each tile has a unique number. Feels a bit cheaty, though, given that problem
 * statement doesn't make it clear whether or not that is allowed.
 */

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <assert.h>

#ifndef ONLINE_JUDGE
#include <Windows.h>
#endif

using namespace std;

#ifndef ONLINE_JUDGE
class TimedSection
{
public:
	TimedSection(string name = "") : mName(name)
	{
		QueryPerformanceFrequency((LARGE_INTEGER *)&mFreq);
		QueryPerformanceCounter((LARGE_INTEGER *)&mStart);
	}
	~TimedSection()
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&mFinish);
		cout << "Section: " << mName << " " << ((mFinish-mStart)*1.0 / mFreq) << " seconds" << endl;
	}
private:
	__int64 mStart;
	__int64 mFinish;
	__int64 mFreq;
	string mName;
};
#endif

typedef unsigned int StateBitsType;

class StateType
{
public:
	StateType():mState(0) {};
	~StateType() {};
	bool ReadNextState();
	bool operator<(const StateType &rhs) const;

	void DumpState() const;
	bool isMoveAllowed(char move);
	void MakeMove(char move);
private:
	int GetSpPos() const { return ((mState & (0xF << 27)) >> 27); };

	int GetSqAtPos(int p) const
	{
		assert(p>=0 && p<=8);
		assert(p != GetSpPos());
		return 1 + ((mState >> (p*3)) & 0x07);
	}

	void SetState(int p, int v);

	StateBitsType mState;
};

bool StateType::operator<(const StateType &rhs) const
{
	return mState < rhs.mState;
}

bool StateType::isMoveAllowed(char move)
{
	assert(move=='U' || move=='D' || move=='L' || move=='R');
	int sp = GetSpPos();
	if (sp<3 && move=='U') return false;
	if (sp>5 && move=='D') return false;
	if (sp%3 == 0 && move=='L') return false;
	if (sp%3 == 2 && move=='R') return false;
	return true;
}

void StateType::MakeMove(char move)
{
	assert(move=='U' || move=='D' || move=='L' || move=='R');
	assert(isMoveAllowed(move));
	int sp = GetSpPos();
	int nsp = sp;

	switch(move)
	{
	case 'U':
		nsp -= 3;
		break;
	case 'D':
		nsp += 3;
		break;
	case 'L':
		nsp -= 1;
		break;
	case 'R':
		nsp += 1;
		break;
	};

	int sqatpos = GetSqAtPos(nsp);
	SetState(sp, sqatpos);
	SetState(nsp, 0);
}

// Set state to have piece v at position p.
// If v is 0, then put space at position p
inline void StateType::SetState(int p, int v)
{
	assert(p>=0 && p<=8);
	assert(v>=0 && v<=8);

	if (v==0)
	{
		// Set square at previous space position to 0
		// - causes a breakage!
		// Shouldn't be needed anyway.
		//int sp = GetSpPos(s);
		//res &= ~(0x07 << (sp*3));

		mState &= ~(0x0F << 27);
		mState |= (p << 27);
		mState &= ~(0x07 << (p*3));
	}
	else
	{
		mState &= ~(0x07 << (p*3));
		mState |= ((v-1) << (p*3));
	}
}

void StateType::DumpState() const
{
	//cout << "S=" << static_cast<unsigned int>(s) << endl;
	int sppos = GetSpPos();
	for(int bpos=1; bpos<=9; ++bpos)
	{
		if ( bpos-1 == sppos )
			cout << "0";
		else
			cout << GetSqAtPos(bpos-1);

		if (bpos%3==0)
			cout << endl;
		else
			cout << " ";
	}
}

bool StateType::ReadNextState()
{
	// TODO - Check spec, but I don't think it specifically says
	// numbers have to be unique, or in a given range.
	// They are space separated integers
	mState = 0;
	string data;
	for(int i=0; i<3; ++i)
	{
		string line;
		getline(cin, line);
		// NB Don't put #'s in the test data!
		data += line + " ";
	}

	// TODO - map the numbers I read to 0-9.
	// Keep map for outputting again later
	istringstream Parser(data);
	for(int i=0; i<9; ++i)
	{
		int num;
		Parser >> num;

		SetState(i, num);
	}
	return true;
}


// Return count of how many states are reachable from
// this pos. Stores states in pso, which should have
// room for up to four states
int NextStates(StateType current, StateType *pso, char *pc)
{
	assert(pso != NULL);
	assert(pc != NULL);

	int sp = 0;

	char mv[]={'U','L','D','R'};
	for(int d=0; d<4; ++d)
	{
		if (current.isMoveAllowed(mv[d]))
		{
			*pc++=mv[d];
			StateType ns = current;
			ns.MakeMove(mv[d]);
			*pso++ = ns;
			++sp;
		}
	}

	return sp;
}

struct StateProperties
{
	unsigned char depth;
	StateType previous;
	char move;
};

typedef map<StateType,StateProperties> VisitedType;

VisitedType Visited;

// BFS
int Explore(StateType s)
{
	int maxdepth = 0;
	StateProperties sp;
	sp.depth = 0;
	sp.previous = StateType();
	sp.move = 'x';

	Visited.insert( make_pair<StateType, StateProperties>(s, sp) );
	queue<StateType> Q;
	Q.push( s );

	while(Q.empty() == false)
	{
		StateType u = Q.front();
		Q.pop();

		VisitedType::iterator uit = Visited.find(u);

		StateType ns[5];
		char nsm[5];
		int nsc = NextStates(u, ns, nsm);
		for(int i=0; i<nsc; ++i)
		{
			VisitedType::iterator vit = Visited.find(ns[i]);
			if (vit == Visited.end())
			{
				StateProperties nsp;
				nsp.depth = uit->second.depth + 1;
				nsp.move = nsm[i];
				nsp.previous = uit->first;

				Visited.insert( make_pair<StateType, StateProperties>(ns[i], nsp) );
				Q.push( ns[i] );

				if ( nsp.depth > maxdepth )
					maxdepth = nsp.depth;
			}
		}
	}
	return maxdepth;
}

string PathToStart(StateType s)
{
	string res = "";

	VisitedType::iterator it = Visited.find(s);
	assert(it != Visited.end());

	// Push path steps onto stack to reverse it
	stack<char> path;
	while(it != Visited.end() && it->second.move != 'x')
	{
		path.push(it->second.move);

		it = Visited.find(it->second.previous);
	}

	while( path.empty() == false)
	{
		cout << path.top();
		path.pop();
	}
	cout << endl;
	return res;
}

void ReadTestCases()
{
	string line;
	getline(cin, line);
	istringstream parser(line);
	int TestCases;
	parser >> TestCases;
	int TestNo = 1;
	while(TestCases-- > 0)
	{
		// Blank line follows each test case
		getline(cin, line);

		StateType s;
		s.ReadNextState();
		//DumpState(s);

		// 131478592 is 0123456789 state
		Visited.clear();
		int maxdepth = Explore(s);

		cout << "Puzzle #" << TestNo++ << endl;

		for( VisitedType::iterator it = Visited.begin(); it != Visited.end(); ++it )
		{
			if (it->second.depth == maxdepth )
			{
				it->first.DumpState();
				PathToStart(it->first);
				// Only want to report first solution we find
				break;
			}
		}

		// Output an empty line after each test case
		cout << endl;
	}
}

int main()
{
	ReadTestCases();

	return 0;
}
