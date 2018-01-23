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
#include <bitset>

#include <cmath>
#include <iomanip>

using namespace std;

const int MAX_DICT_SIZE = 4095;

// Converts a given number to a bit value with given binaryWidth and sends that as a string.
// So a call like convertIntToBinary(4, 8) would simply convert the number 4 to a binary representation of that number with 8 bits.
// so it would output a string with "00000100" and convertIntToBinary(257, 12) would output "000100000001".
// No input validation is done. So a call with convertIntToBinary(256, 5) would prolly give an error.
string convertIntToBinary(int num, int binaryWidth){
	return bitset<binaryWidth>(num).to_string();
}

void LZWEncode(ifstream &ifs, ofstream &ofs){
	unordered_map<string, unsigned int> dict;
	int i;
	for(i = 0; i < 256; i++){
		dict.insert(make_pair(string(1, (char)i),i));
	}
	dict.insert(make_pair("EOF", ++i));

	string currentBits = "";
	int bitCounter = 0;
	int bitPos = 7;
	int input = ifs.get();
	string newInput = input;
	string output = newInput;

	string outputBits = "";
	uint8_t byte = 0x00;

	while (!ifs.eof()) {
		// We only check less than 4095 here since we want 4095 as our last value, and we increment before we use the actual value.
		if(dict.find(input) == dict.end() && i < MAX_DICT_SIZE){
			dict.insert(make_pair(newInput,++i))

			output = newInput;
			// Convert our dict value for output to a 12 bit string
			outputBits = convertIntToBinary(dict.find(output)->second, 12);
			currentBits += outputBits;

			// If we have more or 8 bits we simply add our bits to our current outgoing byte until it's full and we send it out.
			while(currentBits.size() > 7){
				if(bitPos == 7){
					ofs.put(byte);
					bitPos = 0;
				}

				// Simply check the first bit in our currentBits, and put that value in our outgoing byte and remove that char from currentBits.
				// The first check is prolly not needed as we start with a 0, but doing this just in case we don't happen to reset.
				if(currentBits[0] == "0"){
					byte &= ~(1 << bitPos);
				}
				else{
					byte |= 1 << bitPos;
				}
				currentBits.erase(0,1);
				bitpos--;
			}

			input = ifs.get();
			newInput = string(1, (char)input);
		}
		else{
			output = newInput;
			input = ifs.get();
			newInput += string(1, (char)input);
		}
	}
	if(ifs.eof()){

	}
}
