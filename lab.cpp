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
#include "Huffman.cpp"


using namespace std;

double log256(double p){
	return (log10(p)/log10(256));
}

double entropy(vector<int> count, int fileSize){
	double entropy = 0;

	for (size_t i = 0; i < count.size(); i++)
	{
		// Check here so that a count is never 0, as that neither adds any entropy and may cause errors when doing log(0)
		if (count[i] == 0)
		{
			continue;
		}

		double p = 1 * count[i] / fileSize;
		// Using 256 here will give the entropy in bytes, as a byte is 8 bits which is 256 different values.
		entropy -= p * log256(p);
	}
	return entropy;
}

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
  ios::sync_with_stdio(false);
  cin.tie(NULL);

	string fileName = "";
	fileName = argv[1];
	streampos fileSize;
	char * memblock;

	ifstream file(fileName, ios::in|ios::binary|ios::ate);

	// Read file to memory
	if(file.is_open()){
		fileSize = file.tellg();
		memblock = new char [fileSize + 1];
		file.seekg(0, ios::beg);
		file.read(memblock, fileSize);
		file.seekg(0,ios::beg);
		memblock[fileSize] = -1;
	}

	map<int, int> table;
	int counter = 0;

	while (true) {
		int input = file.get();
		if(input == -1){
			table.insert(make_pair(256, 1));
			break;
		}
		if(table.find(input) != table.end()){
			table[input]++;
		}
		else{
			table.insert(make_pair(input, 1));
		}
		counter++;
	}

	/*
	for(auto it = table.begin();  it != table.end() ; it++){
		cout << it->first << " " << it->second << endl;
	}
	*/


	vector<string> splitStr = string_split(fileName, '.');
	ofstream outputFile("Encoded" + fileName, ios::binary|ios::out|ios::trunc);
	cout << "Encoding file: " << fileName << endl;
	HuffmanEncode(table,outputFile, memblock, fileSize);
	file.close();
return 0;
}
