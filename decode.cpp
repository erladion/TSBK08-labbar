//
// Author: Johan Jansson
//

#include <iostream>
#include <algorithm>

#include <vector>
#include <set>
#include <map>
#include <stack>
#include <unordered_set>
#include <unordered_map>

#include <cmath>
//#include <iomanip>

//#include "point.h"

#include <fstream>
#include <string>
#include "Huffman.cpp"

using namespace std;

vector<string> string_split(string s, const char delimiter)
{
    size_t start=0;
    size_t end=s.find_first_of(delimiter);

  	vector<string> output;

    while (end <= string::npos)
    {
	    output.emplace_back(s.substr(start, end-start));

	    if (end == string::npos)
	    	break;

    	start=end+1;
    	end = s.find_first_of(delimiter, start);
    }

    return output;
}

// Argument 1 = huffman or lz77
// Argument 2 = file name

int main(int argc, char* argv[]){
	//ios::sync_with_stdio(false);
  //cin.tie(NULL);

	string fileName = "";
	fileName = argv[1];
	streampos filesize;
	char * memblock;

	ifstream file(fileName, ios::in|ios::binary|ios::ate);

	if(file.is_open()){
		filesize = file.tellg();
		memblock = new char [filesize + 1];
		file.seekg(0, ios::beg);
		file.read(memblock, filesize);
		file.seekg(0,ios::beg);
		memblock[filesize] = -1;
	}
	vector<string> splitStr = string_split(fileName, '.');

	ofstream outputFile("decoded" + splitStr[0], ofstream::binary);

	HuffmanDecode(file, outputFile);
	file.close();
}
