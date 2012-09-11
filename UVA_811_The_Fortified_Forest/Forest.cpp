// Adrian Dale
// 12/09/2012
// Solves online judge problem: 811 - The Fortified Forest
// 10590104 	811 	The Fortified Forest 	Accepted 	C++ 	0.240 	2012-09-11 12:55:19
// Accepted first try, ranked 136th
// Uses code from http://www.algorithmist.com/index.php/Monotone_Chain_Convex_Hull.cpp
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <assert.h>
using namespace std;

struct Tree
{
	int x;
	int y;
	int v;
	int l;
	Tree(int ix, int iy, int iv, int il) : x(ix), y(iy), v(iv), l(il) {}
	Tree() : x(0), y(0), v(0), l(0) {}
};

int ForestNo;
vector<Tree> Forest;

////////////
// http://www.algorithmist.com/index.php/Monotone_Chain_Convex_Hull.cpp

typedef int coord_t;         // coordinate type
typedef long long coord2_t;  // must be big enough to hold 2*max(|coordinate|)^2
 
struct Point {
	coord_t x, y;
 
	bool operator <(const Point &p) const {
		return x < p.x || (x == p.x && y < p.y);
	}
};
 
// 2D cross product of OA and OB vectors, i.e. z-component of their 3D cross product.
// Returns a positive value, if OAB makes a counter-clockwise turn,
// negative for clockwise turn, and zero if the points are collinear.
coord2_t cross(const Point &O, const Point &A, const Point &B)
{
	return (A.x - O.x) * (coord2_t)(B.y - O.y) - (A.y - O.y) * (coord2_t)(B.x - O.x);
}
 
// Returns a list of points on the convex hull in counter-clockwise order.
// Note: the last point in the returned list is the same as the first one.
vector<Point> convex_hull(vector<Point> &P)
{
	int n = P.size(), k = 0;
	vector<Point> H(2*n);
 
	// Sort points lexicographically
	sort(P.begin(), P.end());
 
	// Build lower hull
	for (int i = 0; i < n; i++) {
		while (k >= 2 && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
		H[k++] = P[i];
	}
 
	// Build upper hull
	for (int i = n-2, t = k+1; i >= 0; i--) {
		while (k >= t && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
		H[k++] = P[i];
	}
 
	H.resize(k);
	return H;
}
//
////////////

// Calculate the perimeter of the forest, ignoring any tree with
// a 1 bit set in choppedTrees
// This uses the convex_hull code above, so we have some inefficient
// memory copying and passing, but I did it this way as I wanted to leave
// the convex_hull function untouched. (Save changing it to pass by reference)
double ForestPerimeter(int choppedTrees, int &choppedValue, int &fenceLength, int &treesChopped)
{
	// Fill pts with all of the unchopped trees
	vector<Point> pts;
	choppedValue = 0;
	fenceLength = 0;
	treesChopped = 0;
	for(unsigned int i=0; i<Forest.size(); ++i)
	{
		if (((choppedTrees >> i) & 1) == 0)
		{
			// Include tree
			Point pt;
			pt.x = Forest[i].x;
			pt.y = Forest[i].y;
			pts.push_back(pt);
		}
		else
		{
			// Tree has been chopped down, so add it to the
			// value of the chopped wood and the length of fence we can
			// make from it
			choppedValue += Forest[i].v;
			fenceLength += Forest[i].l;
			++treesChopped;
		}
	}
	
	// Get the points that are on the fence
	vector<Point> result = convex_hull(pts);
	
	// Calculate the perimeter - it's the sum of the lengths from each point to the next.
	// Nicely helped by the way our borrowed function returns the points.
	double perim = 0.0;
	if (result.size() < 2) // eg when all trees are chopped down
		return 0.0;

	for(unsigned int i=0; i<result.size()-1; ++i)
	{
		perim += sqrt( static_cast<double>((result[i+1].x - result[i].x)*(result[i+1].x - result[i].x) +
								(result[i+1].y - result[i].y)*(result[i+1].y - result[i].y)) );
	}

	// Debug output code - svg drawing instructions
	// My gcc doesn't like the lambda fn - needs to be above 4.5 and mine is 4.4.3
	// Online judge one should be OK, though.
	/*
	// Draw perimeter
	for(unsigned int i=0; i<result.size()-1; ++i)
		cout << "<line x1=\"" << result[i].x << "\" y1=\"" << result[i].y << "\" x2=\""
			<< result[i+1].x << "\" y2=\"" << result[i+1].y << "\" />" << endl;
	// Draw all trees
	for_each(Forest.begin(), Forest.end(), [](Tree t) {
		cout << "<circle cx=\"" << t.x << "\" cy=\"" << t.y << "\" r=\"0.1\" />" << endl;
	});
	*/
	return perim;
}

void solveForest()
{
	int bestTreesChopped = Forest.size() + 1;
	int bestChoppedValue = numeric_limits<int>::max();
	int bestFenceLength = numeric_limits<int>::max();
	int bestCombination = -1;
	double bestPerimeter = 0.0;
	
	// Try every possible combination of trees chopped
	// There are a maximum of 15 trees, which is only 65536 possible combinations.
	int maxcomb = static_cast<int>(pow(2.0, static_cast<double>(Forest.size())) );
	for(int chopped=0; chopped < maxcomb; ++chopped)
	{
		int choppedValue;
		int fenceLength;
		int treesChopped;
		
		double perimeter =  ForestPerimeter(chopped, choppedValue, fenceLength, treesChopped);
		
		// See if we've chopped enough trees to make a fence big enough
		if (perimeter <= fenceLength)
		{
			if (choppedValue < bestChoppedValue)
			{
				bestChoppedValue = choppedValue;
				bestFenceLength = fenceLength;
				bestCombination = chopped;
				bestPerimeter = perimeter;
				bestTreesChopped = treesChopped;
			}
			else if (choppedValue == bestChoppedValue)
			{
				// Tie-break
				if (treesChopped < bestTreesChopped)
				{
					bestTreesChopped = treesChopped;
					bestCombination = chopped;
					bestFenceLength = fenceLength;
					bestPerimeter = perimeter;
				}
			}
		}
	}

	cout << "Forest " << ForestNo << endl;
	
	// NB Spec doesn't say what to do if no perimeter is
	// possible.
	cout << "Cut these trees:";
	for(unsigned int i=0; i<Forest.size(); ++i)
	{
		if (((bestCombination >> i) & 1) == 1)
		{
			cout << " " << i+1;
		}
	}
	cout << endl;

	cout << "Extra wood: " << fixed << setprecision(2) 
		<< static_cast<double>(bestFenceLength) - bestPerimeter << endl;
}

// Read the test cases in from the file
void ReadTestCases()
{
	ForestNo = 0;
	do
	{
		string line;
		getline(cin, line);
		istringstream parser(line);
		int NoOfTrees;
		parser >> NoOfTrees;
		if (NoOfTrees == 0)
			break;
		assert(NoOfTrees >= 2 && NoOfTrees <= 15);

		// So we get blank line between (but not after) test cases
		if (ForestNo != 0 )
			cout << endl;
			
		++ForestNo;
		Forest.clear();
		while(NoOfTrees-- > 0)
		{
			getline(cin, line);
			parser.str(line);
			parser.clear();
			Tree newTree;
			parser >> newTree.x >> newTree.y >> newTree.v >> newTree.l;
			Forest.push_back( newTree );
		}
		
		solveForest();
	}
	while(true);
}

int main()
{
	ReadTestCases();

	return 0;
}
