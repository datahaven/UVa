// Problem C: Ancient Messages
// Adrian Dale
// 28/10/2011
// This version of the code has been converted to only use C++
// features compatible with the online judge
// Runs in 0.164 seconds on online judge. 7th place
// Version 1 took 0.248 seconds
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <set>
#include <vector>

using namespace std;

static const int MAX_W = 50;
static const int MAX_H = 200;

typedef pair<int,int> CoordType;

// Globals that will be set per test case
int W;
int H;
int CaseNum;

// Craftily initialise enough memory to hold the largest possible
// test case (plus room for a border), so that we don't need to
// keep new'ing and deleting memory.
// The border means we don't ever need to test for pixels outside
// the bitmap
// This will be obsolete once the scan line algorithm works
int Pixels[MAX_W*4+2][MAX_H+2];

typedef pair<int,int> RunType;
vector< vector<RunType> > ScanLines;

// Map to count number of holes for each shape
vector<int> VoidsMap;



inline void getRealCoords( int sx, int sy, int &rsx, int &rex )
{
	rsx = 0;
	rex = 0;
	int scanStart=0;
	for(int s=0; s<ScanLines[sy].size(); ++s)
	{
		rsx = scanStart;
		rex = rsx + ScanLines[sy][s].second - 1;
		if (s >= sx)
			break;

		scanStart += ScanLines[sy][s].second;

	}

	//cout << rsx << " " << rex << endl;
}


inline int CellToInt(char cell)
{
	// Convert character to numeric value
	if ( cell >= '0' && cell <= '9')
		cell -= '0';
	else
		cell = cell - 'a' + 10;
	return cell;
}

bool readTestCaseIntoScanLines()
{
	// Assuming H and W have been read in

	ScanLines.clear();
	// We know how many rows there are in image, although we won't know how many
	// runs of pixels there are on each line until we read them
	ScanLines.reserve(H);

	// Read in the rows and unpack them into Scan Lines
	for(int i=0; i<H; ++i)
	{
		string row;
		cin >> row;

		vector<RunType> line;

		int pixx = 0;

		// See what the first pixel of the row is
		int currentPix = (CellToInt(row[0]) & 0x80) != 0 ? 1 : 0;

		for(size_t j=0; j<row.size(); ++j)
		{
			char cell = CellToInt(row[j]);

			int bit = 0x08;
			while(bit > 0)
			{
				int pix; // next pixel
				pix = (cell & bit) != 0 ? 1 : 0;

				if ( pix == currentPix )
				{
					// Extend the current scanline
					if (cell == 0 || cell == 15)
					{
						// Minor optimisation to save us another three
						// runs round this loop in the common case of
						// solid run of pixels in the input
						// TODO - Removed as it causes a breakage - see test03.tst
						// where it miscalculates rows 5,6,7
						//pixx+=4;
						//bit = 0x01; // exit while loop early
						++pixx;
					}
					else
						++pixx;
				}
				else
				{
					// Start a new scanline
					// cout << "(" << currentPix << "," << pixx << ") ";
					line.push_back( make_pair<int, int>(currentPix, pixx) );
					if (cell==0 || cell == 15)
					{
						pixx = 4;
						bit = 0x01;
					}
					else
						pixx=1;
					currentPix = pix;
				}

				bit >>= 1;
			}

		}
		// scanline ends at end of row
		line.push_back( make_pair<int, int>(currentPix, pixx) );
		//cout << "(" << currentPix << "," << pixx << ") " << endl;

		ScanLines.push_back(line);
	}

	// Set W to the width in Pixels as this will
	// be easier to work with
	// TODO - may not actually be needed with scan line algorithms
	W = W*4;

	return true;
}



// Here sy is the row number and sx is the Run number in that row
int floodFill_ScanLines( int sx, int sy, int searchCol, int fillCol )
{
	int lastTouched = 0;

	// CoordType is the coords of the Run, NOT the pixel
	set<CoordType> unvisited;
	unvisited.insert( make_pair(sx, sy) );

	while(unvisited.empty() == false)
	{
		// Remove the first item from the unvisited set and inspect it
		CoordType vc = *unvisited.begin();
		unvisited.erase( unvisited.begin() );

		RunType &run = ScanLines[vc.second][vc.first];

		if (run.first == searchCol)
		{
			// If the pixel at this coord is the colour we're looking for,
			// then fill it with the fill colour and push all of the
			// adjacent pixels that match the search colour into the set,
			// so that they can be searched in turn.
			// Using set so that I don't have to care about pushing the same
			// pixel more than once.
			run.first = fillCol;

			// lastTouched is whatever colour our left or right neighbouring run is
			// We only care about this value for internal voids, so it shouldn't matter
			// if we have single run scanlines and don't populate last touched here
			if (vc.first > 0)
				lastTouched = ScanLines[vc.second][vc.first-1].first;
			if (vc.first < ScanLines[vc.second].size()-1)
				lastTouched = ScanLines[vc.second][vc.first+1].first;

			// Need to locate all adjoining scan lines and push into unvisited list
			// - get actual x coords of start and end of our run
			int xstart, xend;
			getRealCoords(vc.first, vc.second, xstart, xend);

			// Only need to search above and below - no need to worry about horizontal neighbours,
			// although we'll need to know these to update lastTouched
			if (vc.second > 0) // Check we're not on top row
			{
				// Search above for all segments that overlap the one we're searching
				for(size_t srch=0; srch<ScanLines[vc.second - 1].size(); ++srch)
				{
					int sstart, send;
					getRealCoords(srch, vc.second-1, sstart, send);
					if ( (sstart <= xstart && send >= xstart) ||
						(sstart <= xend && send >= xstart) )
					{
						//overlap
						unvisited.insert( make_pair<int,int>(srch, vc.second-1) );
					}
				}
			}
			if (vc.second < H-1) // Check we're not on top row
			{
				// TODO - fix this horrible duplicated code.
				// Am planning to replace this method soon, anyway
				// Search above for all segments that overlap the one we're searching
				for(size_t srch=0; srch<ScanLines[vc.second + 1].size(); ++srch)
				{
					int sstart, send;
					getRealCoords(srch, vc.second+1, sstart, send);
					if ( (sstart <= xstart && send >= xstart) ||
						(sstart <= xend && send >= xstart) )
					{
						//overlap
						unvisited.insert( make_pair<int,int>(srch, vc.second+1) );
					}
				}
			}
				//int pix = Pixels[vc.first + DX[i]][vc.second + DY[i]];
				//if ( pix == searchCol )
				//	unvisited.insert( make_pair(vc.first + DX[i], vc.second + DY[i]) );
				//else if (pix != fillCol)
				//	lastTouched = pix;

		}
	}
	return lastTouched;
}


// This function checks off all 0 pixels visitable from the edge
// of the image. These pixels are therefore outside of any shapes
// in the image.
void fillBackground()
{
	// Scan along top and bottom edges flood filling from any
	// 0 pixel we find. Use value 3 to denote background
	for( size_t x=0; x<ScanLines[0].size(); ++x)
		if (ScanLines[0][x].first == 0)
			floodFill_ScanLines(x,0,0,3);
	for( size_t x=0; x<ScanLines[H-1].size(); ++x)
		if (ScanLines[H-1][x].first == 0)
			floodFill_ScanLines(x,H-1,0,3);

	// Same for left and right edges
	for( size_t y=0; y<ScanLines.size(); ++y)
	{
		if (ScanLines[y][0].first == 0)
			floodFill_ScanLines(0,y,0,3);
		size_t rhs = ScanLines[y].size()-1;
		if (ScanLines[y][rhs].first == 0)
			floodFill_ScanLines(rhs,y,0,3);
	}
}

// Flood fill all of the shapes we can find.
// Mark each one in the pixel map with a unique number,
// starting from 3 which was the number used to denote pixels
// in the background outside the shapes
// Return a count of how many shapes we spot.
int fillShapes()
{
	int ShapeNo = 3;
	// Scan entire grid looking for 1 pixels
	for( size_t y=0; y<ScanLines.size(); ++y)
	{
		for( size_t x=0; x<ScanLines[y].size(); ++x)
		{
			if ( ScanLines[y][x].first == 1 )
			{
				// If we find a 1 pixel that means we've got a
				// shape we haven't yet visited, so fill it with the
				// ShapeNo
				++ShapeNo;
				floodFill_ScanLines(x, y, 1, ShapeNo);
			}
		}
	}
	return ShapeNo-3;
}

// Fill in the centres of each shape.
// For each one, note which shape it touches
// and keep a count of how many holes touch each shape.
// ShapesFound says how many shapes to expect
void fillVoids(int ShapesFound)
{
	VoidsMap.clear();

	if (ShapesFound == 0)
		return;

	// Because VoidsMap is empty it'll get resized to the
	// desired size with each entry initialised to zero
	VoidsMap.resize(ShapesFound, 0);

	// Scan entire grid looking for remaining 0 pixels
	for( size_t y=0; y<ScanLines.size(); ++y)
	{
		for( size_t x=0; x<ScanLines[y].size(); ++x)
		{
			if ( ScanLines[y][x].first == 0 )
			{
				int lastTouched = floodFill_ScanLines(x, y, 0, 2);
				//assert(lastTouched <= ShapesFound);
				if (lastTouched > 3)
					++VoidsMap[lastTouched-4];
			}
		}
	}
}

void outputResults()
{
	// Hieroglyphs in order from 0 to 5 of how many holes they have
	char hieroglyphs[]={'W','A','K','J','S','D'};

	string results;

	for( size_t i=0; i<VoidsMap.size(); ++i)
	{
		int holeCount = VoidsMap[i];
		results.push_back(hieroglyphs[holeCount]);
	}

	sort( results.begin(), results.end() );

	cout << "Case " << CaseNum << ": " << results << endl;
}

bool nextTestCase()
{
	cin >> H;
	cin >> W;
	if (H==0 && W==0)
		return false;

	//readTestCase();
	readTestCaseIntoScanLines();

	fillBackground();

	int ShapesFound = fillShapes();

	fillVoids(ShapesFound);

	//DebugPrintScanLines();

	outputResults();

	++CaseNum;
	return true;
}

void Run()
{
#ifdef _DEBUG
	cout << "WARNING: DEBUG BUILD" << endl;
	// Switch so we read our input from a file.
	// Means I don't have to set up a command line in visual studio to
	// run with the debugger
	streambuf *backup, *psbuf;
	ifstream filestr;
	//filestr.open("ancient.in");
	filestr.open("test3.txt");
	backup = cin.rdbuf();
	psbuf = filestr.rdbuf();
	cin.rdbuf(psbuf);
#endif
	CaseNum=1;
	bool moreTests=true;
	while(moreTests)
	{
		//const long long start = counter();
		moreTests = nextTestCase();
		//const long long finish = counter();
		//cout << "Case " << CaseNum << " took " << format_time(start, finish) << endl;
	}

#ifdef _DEBUG
	// Put our input stream back to normal
	// Not terribly important if we're about to exit anyway!
	cin.rdbuf(backup);
	filestr.close();
#endif
}

int main()
{
	//const long long start = counter();
	Run();
	//const long long finish = counter();
	//cout << "Total Run Time: " << format_time(start, finish) << endl;
	return 0;
}
