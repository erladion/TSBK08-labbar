#include <queue>
#include <map>
#include <iostream>
#include <fstream>
#include <iterator>

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
		heap.push(new LeafNode(table->first, table->second));
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
		codeMap.insert(make_pair(ln->data, string));
	}
	else if(ParentNode* pn = dynamic_cast<ParentNode*>(n)){
		string lc = code + "0";
		string rc = code + "1";

		map<int, string> lRet = HuffmanCode(pn->left, lc);
		codeMap.insert(lRet.begin(), lRet.end());

		map<int, string> rRet = HuffmanCode(pn->right, rc);
		codeMap.insert(rRet.begin(), rRet.end());
	}
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
void HuffmanEncode(){
	char* retMemblock;

	Node* root = HuffmanTree(v);

	map<int, vector<bool> > m = HuffmanCode(root, "");

	for(map<int, vector<bool> >::const_iterator it = m.begin(); it != m.end(); ++it){
		char t = it->first;
		cout << t << " ";
		copy(it -> second.begin(), it->second.end(), ostream_iterator<bool>(cout));
		cout << endl;
	}

	ofstream outputFile ("Encoded"+fileName);

	uint8_t c = 0;
	int bitPos = 7;
	//printf("%d\n",fileSize);

	for(int i = 0; i < fileSize; i++){
		//printf("%d\n",i);
		//printf("Searching for %d\n", memblock[i]);
		vector<bool> b = m.find(memblock[i])->second;

		for(int j = 0; j < b.size(); j++){
			printf("%d\n", bitPos);
			if(b[j] == false){
				c &= ~(1 << bitPos);
			}
			else{
				c |= 1 << bitPos;
			}
			printf("%d\n", c);
			if(bitPos == 0){
				outputFile.put(c);
				c = 0;
				printf("\n");
				bitPos = 7;
			}
			else{
				bitPos--;
			}
		}
	}
	outputFile.close();
}

void HuffmanDecode(){

}
