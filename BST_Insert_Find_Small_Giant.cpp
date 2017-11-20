#include <iostream>
#include <array>

using namespace std;

class testnode
{
	long long value; //8 bytes
	testnode* left; //8 bytes
	testnode* right; // 8 bytes
};

class Node
{
public:
	int value; //4 bytes + 4 bytes for faster acsess on x64 PC
	Node* left; //8 bytes
	Node* right; //8 bytes
	
	Node() : left(NULL), right(NULL) {}
	Node(int& val) : value(val), left(NULL), right(NULL) {}
	
	Node* find(int v);
	Node* find_iterate(int v);
};

Node* insert_node(Node* root, int val);
void draw_tree(Node* root);
void iter_insert(Node* &root, int val);

Node *small = NULL, *giant = NULL;

Node* Node::find(int v)
{
	if(this->left == NULL && this->right == NULL && value != v)
		return NULL;
	else if(v == value)
		return this;
	else if(v < value)
		return left->find(v);
	else
		return right->find(v);
}

Node* Node::find_iterate(int v)
{
	Node* curr_node = this;
	while(curr_node)
	{
		if(v == curr_node->value)
			return curr_node;
		else if(v < curr_node->value)
			curr_node = curr_node->left;
		else
			curr_node = curr_node->right;
	}
	
	return curr_node;
}

int main()
{
	array<int, 9> small_ints = {9,7,13,5,8,2,6,17,16};
	
	testnode test2;
	Node test;
	cout << "Size of Node: " << sizeof(test) << "bytes" << endl;
	cout << "Size of testNode: " << sizeof(test2) << "bytes" << endl;
	//Same size! Compiler optimized Node and used +4 bytes for int
	//for better accsess speed on 64 bit computer!!!
	
	
	cout << "Constructing SMALL tree..." << endl;
	for(auto& i : small_ints)
		small = insert_node(small, i);
	
	cout << "Constructing GIANT tree..." << endl;
	for(int i = 300000; i >= 0; --i)
	{
		iter_insert(giant, i);
		if(!(i % 500)) cout << (float)(300000-i)/300000 * 100.0f << "%               ";
		cout << "\r";
	}
	cout << endl;
	
	//cout << "SMALL adress: " << small << endl;
	draw_tree(small);
	
	while(true)
	{
		cout << "Enter Node to find: ";
		int srch;
		cin >> srch;
		cout << "Searching for " << srch << "..." << endl;
		Node* tmp = giant->find_iterate(srch);
		if(tmp)
			cout << "Node Found! Adress: " << tmp << " Value: " << tmp->value;
		else
			cout << "Node not found. Plese, try again!";
		cout << endl;
	}
	
	return 0;
}

void iter_insert(Node* &root, int val)
{
	Node* curr_node = root;
	
	if(!root)
		root = new Node(val);
	else
	{
		Node* pre_node; //previous node
		enum Direction { TO_LEFT, TO_RIGHT } dir;
		
		while(curr_node)
		{
			//cout << "Searching place for node " << val << endl;
			if(val < curr_node->value)
			{
				pre_node = curr_node;
				curr_node = curr_node->left;
				dir = TO_LEFT;
			}
			else
			{
				pre_node = curr_node;
				curr_node = curr_node->right;
				dir = TO_RIGHT;
			}
		}
		
		//cout << "Constructing node " << val << endl;
		if(dir == TO_LEFT)
			pre_node->left = new Node(val);
		else
			pre_node->right = new Node(val);
	}
}

Node* insert_node(Node* root, int val)
{
	if(root && !(val % 10000)) cout << "Searching place for node " << val << endl;
	else if(!(val % 10000)) cout << "Constructing node " << val << " ---> ";
	
	if(!root)
	{
		root = new Node(val);
		//cout << "Node adress: " << root << endl;
	}
	else if(val < root->value)
		root->left = insert_node(root->left, val);
	else
		root->right = insert_node(root->right, val);
	
	return root;
}

void draw_tree(Node* root) //this draws only preset tree!!!
{
	cout << "   " << root->value << endl;
	cout << "  " << root->left->value << " " << root->right->value << endl;
	cout << " " << root->left->left->value << " " << root->left->right->value
	<< " " << root->right->right->value <<endl;
	cout << root->left->left->left->value << " " << root->left->left->right->value
	<< " " << root->right->right->left->value << endl;
}