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
#include <queue>

#include <cmath>
#include <iomanip>

#include <fstream>
#include "Huffman.cpp"

using namespace std;

double log256(double p){
	return (log(p)/log(256));
}

double entropy(map<int, int> table, int fileSize){
	double e = 0;

	for(auto it = table.begin(); it != table.end() ; it++){
		// Check here so that a count is never 0, as that neither adds any entropy and may cause errors when doing log(0)
		if(it->second == 0){
			continue;
		}

		double p = ((double)(it->second)) / (double)fileSize;
		// Using 256 here will give the entropy in bytes, as a byte is 8 bits which is 256 different values.
		e -= p * log256(p);
	}
	return e;
}

int main(int argc, char* argv[]){
  ios::sync_with_stdio(false);
  cin.tie(NULL);

	string fileName = "";
	fileName = argv[1];
	streampos fileSize;

	ifstream file(fileName, ios::in|ios::binary|ios::ate);

	if(file.is_open()){
		fileSize = file.tellg();
		file.seekg(0, ios::beg);
	}

	// Loop through
	map<int, int> table;
	int counter = 0;

	int input = file.get();
	while (!file.eof()) {
		if(table.find(input) != table.end()){
			table[input]++;
		}
		else{
			table.insert(make_pair(input, 1));
		}
		counter++;
		input = file.get();
	}
	if(file.eof()){
		table.insert(make_pair(256, 1));
	}

	// Clear all the flags such as eof flag nd set the file pointer to
	// the beginning of the file, so that we can reuse the same stream.
	file.clear();
	file.seekg(0, ios::beg);

	ofstream outputFile("Encoded" + fileName, ios::binary|ios::out|ios::trunc);
	cout << "Encoding file: " << fileName << endl;
	HuffmanEncode(table, outputFile, file);
	cout << setprecision(10) << "File entropy before compression: " << entropy(table, fileSize) << endl;
	file.close();
return 0;
}
