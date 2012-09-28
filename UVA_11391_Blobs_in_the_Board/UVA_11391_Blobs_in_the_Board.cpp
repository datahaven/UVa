// UVa 11391 Blobs in the Board
// http://uva.onlinejudge.org/index.php?option=com_onlinejudge&Itemid=8&category=437&page=show_problem&problem=2386
// Adrian Dale 25/09/2012
// Accepted 0.060 seconds, ranked 26th (ie joint 16th out of 150 solvers)
// Relatively simple Depth First Search solution with the minor wrinkle of a Memo solution cache
// that stores solutions between test cases. Also a tiny tweak to treat rxc boards the same as cxr boards
// so that we get better Memo re-use.
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cstring>
using namespace std;

int R, C, N;

class BoardStateType
{
public:
	BoardStateType() : b(0) {};

	void set(int r, int c, bool isBlob)
	{
		if (isBlob)
			b |= (1<<(r*4+c));
		else
			b &= ~(1<<(r*4+c));
	}
	
	bool get(int r, int c)
	{
		return (b & (1<<(r*4+c))) != 0;
	}

	void DumpBoard()
	{
		for(int r=0; r<R; ++r)
		{
			for(int c=0; c<C; ++c)
				cout << this->get(r, c) ? "o" : "-";
			cout << endl;
		}
	}

	// Used to quickly compare b to powers of two to see if
	// there is only one blob left.
	// Obviously I could just make this public, instead
	int Hack() {return b;}
private:
	int b;
};

// Re-use the Memo across test runs. We're likely to get the same
// board positions arising even with different starting configurations
// First two params allow for different row/col values
int Memo[4][4][65536];

#define DO_CHECK(a1,a2,b1,b2,c1,c2) { \
		if ( a1<R && a2<C && b1<R && b2<C && c1<R && c2<C) \
		{ \
			if (b.get(a1,a2) == true && b.get(b1,b2) == true && b.get(c1,c2) == false) { \
				BoardStateType newb = b; \
				newb.set(a1,a2, false); \
				newb.set(b1,b2, false); \
				newb.set(c1,c2, true); \
				results.push_back(newb); \
			} \
			if (b.get(a1,a2) == false && b.get(b1,b2) == true && b.get(c1,c2) == true) { \
				BoardStateType newb = b; \
				newb.set(a1,a2, true); \
				newb.set(b1,b2, false); \
				newb.set(c1,c2, false); \
				results.push_back(newb); \
			} \
		} \
	}

// Return all possible moves from state b in vector results
inline void AllPossibleMoves(BoardStateType b, vector<BoardStateType> &results)
{
	// Look for all right/left jumps from first/second col postion
	for(int r=0; r<R; ++r)
	{
		for(int c=0; c<min(2, C-2); ++c)
		{
			DO_CHECK(r,c, r,c+1, r,c+2)
		}
	}
	// Same but for up/down
	for(int r=0; r<min(2, R-2); ++r)
	{
		for(int c=0; c<C; ++c)
		{
			DO_CHECK(r,c, r+1,c, r+2, c)
		}
	}
	
	// Now the diagonals
	DO_CHECK(0,0, 1,1, 2,2);
	DO_CHECK(0,1, 1,2, 2,3);
	DO_CHECK(1,0, 2,1, 3,2);
	DO_CHECK(1,1, 2,2, 3,3);

	DO_CHECK(0,2, 1,1, 2,0);
	DO_CHECK(0,3, 1,2, 2,1);
	DO_CHECK(1,2, 2,1, 3,0);
	DO_CHECK(1,3, 2,2, 3,1);
}

int MinWays(BoardStateType b)
{
	if (Memo[R-1][C-1][b.Hack()] != -1)
		return Memo[R-1][C-1][b.Hack()];
	int result = 0;

	vector<BoardStateType> allPoss;
	AllPossibleMoves( b, allPoss );
	// if none, then return 1 if this state has just one blob
	if (allPoss.size() == 0)
	{
		if (b.Hack() == 1 || b.Hack() == 2 || b.Hack() == 4 || b.Hack() == 8 ||
			b.Hack() == 16 || b.Hack() == 32 || b.Hack() == 64 || b.Hack() == 128 ||
			b.Hack() == 256 || b.Hack() == 512 || b.Hack() == 1024 || b.Hack() == 2048 ||
			b.Hack() == 4096 || b.Hack() == 8192 || b.Hack() == 16384 || b.Hack() == 32768 )
			result = 1;
		else
			result = 0;
	}
	else
	{
		// Sum up results from the new positions
		result = 0;
		for(vector<BoardStateType>::iterator it = allPoss.begin(); it != allPoss.end(); ++it)
		{
			result += MinWays(*it);
		}
	}

	Memo[R-1][C-1][b.Hack()] = result;
	return Memo[R-1][C-1][b.Hack()];
}

int Solve(BoardStateType b)
{
	return MinWays(b);
}

void ReadTestCases()
{
	string line;
	getline(cin, line);
	istringstream parser(line);
	int TestCount;
	parser >> TestCount;

	int CaseNo = 1;
	while(TestCount-- > 0)
	{
#ifndef ONLINE_JUDGE
		clock_t starttime = clock();
#endif
	
		getline(cin, line);
		parser.str(line);
		parser.clear();
		parser >> R >> C >> N;

		// Optimisation so we use the same Memo where
		// the board is the same shape
		bool flip = false;
		if (R > C)
		{
			int temp = R;
			R = C;
			C = temp;
			flip = true;
		}

		BoardStateType b;
		for(int i=0; i<N; ++i)
		{
			getline(cin, line);
			parser.str(line);
			parser.clear();
			
			// In spec x,y are row,column respectively - ie back to front.
			int x, y;
			if (!flip)
				parser >> x >> y;
			else
				parser >> y >> x;
			b.set(x-1, y-1, true);
		}
		
		int result = Solve(b);
		cout << "Case " << CaseNo++ << ": " << result << endl;

#ifndef ONLINE_JUDGE
		double endtime = (clock() - starttime) / static_cast<double>(CLOCKS_PER_SEC);

		cout << "Time Taken: " << endtime << " seconds" << endl;
#endif
	}
}

int main()
{
#ifndef ONLINE_JUDGE
	clock_t starttime = clock();
#endif

	// Blank out our Memo
	for(int r=0; r<4; ++r)
		for(int c=r; c<4; ++c)
			for(int i=0; i<65536; ++i)
				Memo[r][c][i] = -1;

	ReadTestCases();
	
#ifndef ONLINE_JUDGE
		double endtime = (clock() - starttime) / static_cast<double>(CLOCKS_PER_SEC);

		cout << "Total Time Taken: " << endtime << " seconds" << endl;
#endif
	return 0;
}