#include <queue>
#include <map>
#include <iostream>
#include <fstream>
#include <iterator>
#include "string.h"
#include <bitset>

using namespace std;

/*
Basic example of the tree
P = ParentNode
L and R = LeafNode

		 P
		/	\
	 L  R
*/

class Node{
public:
	unsigned count;

	Node(unsigned c) : count(c){}
	virtual ~Node() = default;
};

class LeafNode : public Node{
	public:
	int data;

	LeafNode(int d, unsigned c) : data(d), Node(c){}
};

class ParentNode: public Node{
	public:
	Node *left, *right;

	ParentNode(Node *l, Node *r) : Node(l->count + r->count), left(l), right(r){}
};

struct compare{
	bool operator()(Node* left, Node* right){
		return (left->count > right->count);
	}
};

Node* HuffmanTree(map<int, int> table){
	priority_queue<Node*, vector<Node*>, compare> heap;

	for(auto it = table.begin(); it != table.end(); ++it){
		heap.push(new LeafNode(it->first, it->second));
	}

	while (heap.size() != 1) {
		Node *left, *right, *parent;

		left = heap.top();
		heap.pop();

		right = heap.top();
		heap.pop();

		parent = new ParentNode(left, right);
		heap.push(parent);
	}

	return heap.top();
}

map<int, string> HuffmanCode(Node* n, string c){
	map<int, string> codeMap;
	if(LeafNode* ln = dynamic_cast<LeafNode*>(n)){
		codeMap.insert(make_pair(ln->data, c));
	}
	else if(ParentNode* pn = dynamic_cast<ParentNode*>(n)){
		string lc = c + "0";
		string rc = c + "1";

		map<int, string> lRet = HuffmanCode(pn->left, lc);
		codeMap.insert(lRet.begin(), lRet.end());

		map<int, string> rRet = HuffmanCode(pn->right, rc);
		codeMap.insert(rRet.begin(), rRet.end());
	}
	return codeMap;
}

// Use the above functions and make a file based on the encoding created.
void HuffmanEncode(map<int, int>  m, ofstream &ofs, char* memblock, size_t fileSize){
	char* retMemblock;

	Node* root = HuffmanTree(m);
	map<int, string > encodingTable = HuffmanCode(root, "");

	/*
	for(auto it = encodingTable.begin(); it != encodingTable.end(); it++){
		cout << it->first << " " << it->second << endl;
	}
	*/

	// Write heading to the file,
	ofs.put('<');
	ofs.put('\\');
	for(auto it = m.begin(); it != m.end(); ++it){
		if(it->first == 256){
			continue;
		}
		if(it != m.begin()){
			ofs.put('|');
		}
		ofs.put(it->first);
		ofs.put(':');
		string numStr = to_string(it->second);
		for(size_t i = 0; i < numStr.size() ; i++){
			ofs.put(numStr[i]);
		}
	}
	ofs.put('/');
	ofs.put('>');

	uint8_t c = 0x00;
	int bitPos = 7;
	int counter = 0;
	int writtenBytesCounter = 0;

	while(counter <= fileSize + 1){
		int input = memblock[counter];

		if(input == -1){
			for(size_t i = 0; i < encodingTable.find(256)->second.size(); i++){
				if((encodingTable.find(256)->second)[i] == '0'){
					c &= ~(1 << bitPos);
				}
				else{
					c |= 1 << bitPos;
				}
				if(bitPos == 0){
					writtenBytesCounter++;
					ofs.put(c);
					c = 0x00;
					bitPos = 7;
				}
				else{
					bitPos--;
				}
			}
			writtenBytesCounter++;
			ofs.put(c);
			break;
		}

		else{
			for(size_t i = 0; i < encodingTable.find(input)->second.size(); i++){
				if((encodingTable.find(input)->second)[i] == '0'){
					c &= ~(1 << bitPos);
				}
				else{
					c |= 1 << bitPos;
				}
				if(bitPos == 0){
					writtenBytesCounter++;
					ofs.put(c);
					c = 0x00;
					bitPos = 7;
				}
				else{
					bitPos--;
				}
			}
		}
		counter++;
	}
	ofs.close();
	cout << "Wrote: " << writtenBytesCounter << " bytes to file!" << endl;
	cout << "Encoding done!" << endl;
}

string getBit(unsigned char byte, int position){
	return ((byte >> position) & 0x1) ? "1" : "0";
}

string getByteAsBits(unsigned char b){
	string str = "";
	for(int i = 7; i >= 0; i--){
		str += getBit(b, i);
	}
	return str;
}

void HuffmanDecode(ifstream &ifs, ofstream &ofs){
	//
	//	Get the count table
	//
	map<int, int> countTable;
	// Gets the first <
	ifs.get();
	// Gets the following backslash "\"
	ifs.get();
	char c;
	while((c = ifs.get()) != -1){
		if(c == '/'){
			char check = ifs.get();
			if(check == '>'){
				break;
			}
		}
		else{
			// Gets the :
			ifs.get();
		}

		int count;
		string cp = "";
		// Get the count of char c, need a while loop since we only get 1 byte at a time, so if count is 13 we need to loop 2 times to get it
		// first byte is 1 and second byte is 	3.
		char c2;
		while((c2 = ifs.get())!= '|' && c2 != '/'){
			cp += string(1,c2);
			count = stoi(cp);
		}
		countTable.insert(make_pair(c, count));
		if(c2 == '/'){
			ifs.get();
			break;
		}
	}
	// Since we can't put "EOF" in the frequency table in the file, we just add it here manually
	countTable.insert(make_pair(256, 1));

	/*
	for(auto it = countTable.begin(); it != countTable.end(); it++){
		cout << it->first << " " << it->second << endl;
	}
	*/

	// Build the tree from the count table
	Node* root = HuffmanTree(countTable);
	// Need this for the size of our "EOF" char (might be a better way to get it, but can't come up with it right now)
	map<int, string> encodingTable = HuffmanCode(root, "");
	/*
	for(auto it = encodingTable.begin(); it != encodingTable.end(); it++){
		cout << it->first << " " << it->second << endl;
	}
	*/

	// Here we travese our huffman tree 1 "bit" at a time and reset once we reach a leaf node and get a char.
	// Keep going until we hit the end of the file.
	string temp = "";
	string end = encodingTable.find(256)->second;
	Node* currentNode = root;

	c = ifs.get();
	//cout << "Read: " << (int)c << endl;
	string bitString = getByteAsBits(c);
	int counter = 0;

	int readsDone = 1;

	while(true){
		//cout << "Round: " << counter << endl;
		//cout << "Current bitstring sequence: " + bitString << endl;
		// If we are at a leaf we have found our wanted char, so we reset the currentNode and add the char to our outstream
		if(LeafNode* ln = dynamic_cast<LeafNode*>(currentNode)){
			//cout << "Found a leaf: " << ln->data << " with encoding: " << encodingTable.find(ln->data)->second << endl;
			if(ln->data == 256){
				break;
			}
			ofs.put((char)(ln->data));
			currentNode = root;
		}
		// If we have not found a leaf node we keep traversing down and remove the first char in our string
		else if(ParentNode* pn = dynamic_cast<ParentNode*>(currentNode)){
			if(bitString.substr(0,1) == "0"){
				currentNode = pn->left;
				bitString.erase(0, 1);
			}
			else{
				currentNode = pn->right;
				bitString.erase(0, 1);
			}
			if(bitString.size() == 0){
				c = ifs.get();
				readsDone++;
				bitString += getByteAsBits(c);
			}
		}
		// If our current bitstring is smaller than the size of the "EOF" char, we need to read more bytes.
		// Will happen when the file contains alot of different bytes
		//cout << "Size of bitstring: " << bitString.size() << " Size of EOF: " << end.size() << endl;
		if(bitString.size() < end.size() && end.size() > 7){
			double sizeDiff = abs(end.size() - bitString.size());
			//cout << "Size difference: " << sizeDiff << endl;
			double readsNeeded = ceil(sizeDiff/8);
			//cout << "Reads needed: " << readsNeeded << endl;
			for(int i = 0; i < (int)readsNeeded; i++){
				c = ifs.get();
				readsDone++;
				bitString += getByteAsBits(c);
			}
		}
		counter++;
		//cout << "Current c: " << (int)c << endl;
	}
	ofs.close();
	cout << "Bytes read: " << readsDone << endl;
	cout << "Decoding done!" << endl;
}
