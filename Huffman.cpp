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

/*
Node* HuffmanTree(char data[], int count[]){
	class Node *left, *right, *parent;

	priority_queue<Node*, vector<Node*>, compare> heap;

	// Might need to modify this to not add bytes with 0 counts (they are not present in the file)
	for(int i = 0; i < 256; i++){
		heap.push(new LeafNode(data[i], count[i]));
	}

	while (heap.size() != 1) {
		left = heap.top();
		heap.pop();

		right = heap.top();
		heap.pop();

		parent = new ParentNode(left, right);
		heap.push(parent);
	}
	return heap.top();
}
*/

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

/*
Node* HuffmanTree(vector<int> v){
	class Node *left, *right, *parent;

	priority_queue<Node*, vector<Node*>, compare> heap;

	// If a byte is not present in our data we simply just ignore it and keep on going.
	for(int i = 0; i < 256; i++){
		if(v[i] != 0){
			heap.push(new LeafNode(i, v[i]));
		}
	}

	int r = 0;

	while (heap.size() != 1) {
		printf("Round: %d\n", r);
		left = heap.top();
		printf("Left count: %d\n", left->count);
		heap.pop();

		right = heap.top();
		printf("Right count: %d\n", right->count);
		heap.pop();

		parent = new ParentNode(left, right);
		printf("Parent count: %d\n", parent->count);
		heap.push(parent);
		r++;
	}
	return heap.top();
}
*/
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

/*
map<int, vector<bool> > HuffmanCode(Node* n, vector<bool> code){
	map<int, vector<bool> > res = map<int, vector<bool> >();

	// If we reach a leaf node, we are at one of the bytes present in the file
	// and we add the code we got so far together with the byte value into the map
	if(LeafNode* ln = dynamic_cast<LeafNode*>(n)){
		res[ln->data] = code;
	}
	// Otherwise we keep traversing down our tree adding a "0" for left and a "1" for right
	else if(ParentNode* pn = dynamic_cast<ParentNode*>(n)){
		vector<bool> left = code;
		left.push_back(false);
		map<int, vector<bool> > lRet = HuffmanCode(pn->left, left);
		res.insert(lRet.begin(), lRet.end());

		vector<bool> right = code;
		right.push_back(true);
		map<int, vector<bool> > rRet = HuffmanCode(pn->right, right);
		res.insert(rRet.begin(), rRet.end());
	}
	return res;
}
*/

// Use the above functions and make a file based on the encoding created.
void HuffmanEncode(map<int, int>  m, string fileName, char* memblock, size_t fileSize){
	char* retMemblock;

	Node* root = HuffmanTree(m);
	map<int, string > encodingTable = HuffmanCode(root, "");

	ofstream outputFile ("Encoded"+fileName, ofstream::binary);

	// Write heading to the file,
	outputFile.put('{');
	for(map<int, int>::const_iterator it = m.begin(); it != m.end(); ++it){
		if(it->first == 256){
			continue;
		}
		if(it != m.begin()){
			outputFile.put('|');
		}
		outputFile.put(it->first);
		outputFile.put(':');
		outputFile.put((it->second + '0'));
	}
	outputFile.put('}');

	uint8_t c = 0x00;
	int bitPos = 7;
	int counter = 0;

	while(counter <= fileSize + 1){
		int input = memblock[counter];

		if(input == -1){
			for(int i = 0; i < encodingTable.find(256)->second.size(); i++){
				if((encodingTable.find(256)->second)[i] == '0'){
					c &= ~(1 << bitPos);
				}
				else{
					c |= 1 << bitPos;
				}
				if(bitPos == 0){
					outputFile.put(c);
					c = 0x00;
					bitPos = 7;
				}
				else{
					bitPos--;
				}
			}
			outputFile.put(c);
			break;
		}

		else{
			for(int i = 0; i < encodingTable.find(input)->second.size(); i++){
				if((encodingTable.find(input)->second)[i] == '0'){
					c &= ~(1 << bitPos);
				}
				else{
					c |= 1 << bitPos;
				}
				if(bitPos == 0){
					outputFile.put(c);
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
	outputFile.close();
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
	// Since we can't put "EOF" in the frequency table in the file, we just add it here manually
	countTable.insert(make_pair(256, 1));
	// Gets the first {
	ifs.get();
	char c;
	while((c = ifs.get()) != '}'){
		int count;
		// Gets the :
		ifs.get();

		string cp = "";
		// Get the count of char c, need a while loop since we only get 1 byte at a time, so if count is 13 we need to loop 2 times to get it
		// first byte is 1 and second byte is 3.
		char c2;
		while((c2 = ifs.get())!= '|' && c2 != '}'){
			cp += string(1,c2);
			count = stoi(cp);
		}
		countTable.insert(make_pair(c, count));
		if(c2 == '}'){
			break;
		}
	}

	// Build the tree from the count table
	Node* root = HuffmanTree(countTable);
	// Need this for the size of our "EOF" char (might be a better way to get it, but can't come up with it right now)
	map<int, string> encodingTable = HuffmanCode(root, "");

	// Here we travese our huffman tree 1 "bit" at a time and reset once we reach a leaf node and get a char.
	// Keep going until we hit the end of the file.
	string temp = "";
	string end = encodingTable.find(256)->second;
	Node* currentNode = root;

	c = ifs.get();
	string bitString = getByteAsBits(c);

	while(c != -1){
		// If we are at a leaf we have found our wanted char, so we reset the currentNode and add the char to our outstream
		if(LeafNode* ln = dynamic_cast<LeafNode*>(currentNode)){
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
				bitString += getByteAsBits(c);
			}
		}
		// If our current bitstring is smaller than the size of the "EOF" char, we need to read more bytes.
		// Will happen when the file contains alot of different bytes
		if(bitString.size() < end.size()){
			double sizeDiff = abs(bitString.size() - end.size());
			double readsNeeded = ceil(sizeDiff);
			for(int i = 0; i < (int)readsNeeded; i++){
				c = ifs.get();
				bitString += getByteAsBits(c);
			}
		}
	}
}
