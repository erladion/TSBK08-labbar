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
#include <iomanip>

#include <fstream>
#include <string>
#include "Huffman.cpp"

using namespace std;

int main(int argc, char* argv[]){
	string fileName = "";
	fileName = argv[1];

	ifstream file(fileName, ios::in|ios::binary);
	ofstream outputFile("Decoded" + fileName, ios::binary);

	cout << "Decoding file: " << fileName << endl;
	HuffmanDecode(file, outputFile);
	file.close();
}
