/*
 * UVA_10055_Hashmat.cpp
 *
 *  Created on: 12 Nov 2011
 *      Author: Adrian Dale
 */
#include <iostream>
#include <sstream>
using namespace std;

int main()
{
	string line;
	while(getline(cin, line))
	{
		istringstream parser(line);
		long long a, b;
		parser >> a >> b;
		long long aminusb = a - b;
		// Couldn't find an abs fn to work with long long type
		if (aminusb < 0)
			aminusb *= -1;
		cout << aminusb << endl;
	}
	return 0;
}
