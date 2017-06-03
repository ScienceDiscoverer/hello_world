#include <iostream>

using namespace std;

class Stack
{
	int t_size; //total size
	int size; //number of elements
	char* q; //do not change!
	char* head;
	public:
		Stack()
		{
			t_size = 10;
			size = 0;
			q = new char[t_size];
			head = 0; //null
		}
		~Stack()
		{
			delete[] q;
		}
		
		void Push(char chr)
		{
			if(size < t_size)
			{
				if(!size)
				{
					head = q;
					head[0] = chr;
					size++;
				}
				else
				{
					head[1] = chr;
					head++;
					size++;
				}
			}
			else
			{
				changeSize(5);
				head = q+size;
				head[0] = chr;
				size++;
			}
		}
		char Pop()
		{
			if(size)
			{
				if(size-1 == t_size-10 && size >= 5)
				{
					changeSize(-5);
					head = q+(size-2);
					size--;
					return head[1];
				}
				else if(size-1 == 0)
				{
					char* tmp = head;
					head = 0; //null
					size--;
					return tmp[0];
				}
				else
				{
					head--;
					size--;
					return head[1];
				}
			}
			else
				return 0;
		}
		char Peek()
		{
			if(size)
				return head[0];
			else
				return 0;
		}
		int Size()
		{
			return size;
		}
		
	private:
		void changeSize(int n)
		{
			t_size += n;
			char* tmp = new char[t_size];
			for(int i = 0; i < size; ++i) //copy array
				tmp[i] = q[i];
			delete[] q;
			q = tmp;
		}
};

class Queue
{
	int t_size; //total size
	int size; //number of elements
	char* q; //do not change!
	char* head;
	public:
		Queue()
		{
			t_size = 10;
			size = 0;
			q = new char[t_size];
			head = 0; //null
		}
		~Queue()
		{
			delete[] q;
		}
		
		void Enqueue(char chr)
		{
			if(size < t_size)
			{
				if(!size)
				{
					head = q;
					head[0] = chr;
					size++;
				}
				else
				{
					for(int i = size-1; i >= 0; --i) //move queue
						q[i+1] = q[i];
					q[0] = chr;
					head++;
					size++;
				}
			}
			else
			{
				changeSize(5);
				head = q+size;
				for(int i = size-1; i >= 0; --i) //move queue
					q[i+1] = q[i];
				q[0] = chr;
				size++;
			}
		}
		char Dequeue()
		{
			if(size)
			{
				if(size-1 == t_size-10 && size >= 5)
				{
					changeSize(-5);
					head = q+(size-2);
					size--;
					return head[1];
				}
				else if(size-1 == 0)
				{
					char* tmp = head;
					head = 0; //null
					size--;
					return tmp[0];
				}
				else
				{
					head--;
					size--;
					return head[1];
				}
			}
			else
				return 0;
		}
		char Peek()
		{
			if(size)
				return head[0];
			else
				return 0;
		}
		int Size()
		{
			return size;
		}
		
	private:
		void changeSize(int n)
		{
			t_size += n;
			char* tmp = new char[t_size];
			for(int i = 0; i < size; ++i) //copy array
				tmp[i] = q[i];
			delete[] q;
			q = tmp;
		}
};

int main()
{
    //testing stuck below!
	Stack books; //stuck is like a pile of books IRL!
    
    string str = "Hello_world!";
    
    cout << "Stack size: " << books.Size() << endl;
    cout << "Adding \"Hello_world!\" string to Stack!" << endl;
    for(int i = 0; i < str.size(); ++i)
    	books.Push(str[i]);
    	
    cout << "Stack size: " << books.Size() << endl;
    
    cout << "Peeking inside: " << books.Peek() << endl;
    cout << "Stack size: " << books.Size() << endl;
    cout << "Poping top item: " << books.Pop() << endl;
    cout << "Stack size: " << books.Size() << endl;
    
    cout << "Poping 6 items!" << endl;
    for(int i = 0; i < 6; ++i)
    	cout << "----Poping top item: " << books.Pop() << endl;
    cout << "Stack size: " << books.Size() << endl;
    
    cout << "Adding item \"$\"!" << endl;
    books.Push('$');
    cout << "Stack size: " << books.Size() << endl;
    cout << "Peeking inside: " << books.Peek() << endl;
    cout << "Stack size: " << books.Size() << endl;
    cout << "Poping top item: " << books.Pop() << endl;
    cout << "Stack size: " << books.Size() << endl;
    cout << "Poping last 5 items!" << endl;
    for(int i = 0; i < 5; ++i)
    	cout << "----Poping top item: " << books.Pop() << endl;
    cout << "Stack size: " << books.Size() << endl;
    cout << "Peeking inside: " << books.Peek() << endl;
    cout << "Poping top item: " << books.Pop() << endl;
    cout << "Stack size: " << books.Size() << endl;
    
    //testing queue below!
	Queue ppl; //queue is like a queue of people IRL!
    
    string str = "Hello_world!";
    
    cout << "Queue size: " << ppl.Size() << endl;
    cout << "Adding \"Hello_world!\" string to Queue!" << endl;
    for(int i = 0; i < str.size(); ++i)
    	ppl.Enqueue(str[i]);
    	
    cout << "Queue size: " << ppl.Size() << endl;
    
    cout << "Peeking inside: " << ppl.Peek() << endl;
    cout << "Queue size: " << ppl.Size() << endl;
    cout << "Dequeuing top item: " << ppl.Dequeue() << endl;
    cout << "Queue size: " << ppl.Size() << endl;
    
    cout << "Dequeuing 6 items!" << endl;
    for(int i = 0; i < 6; ++i)
    	cout << "----Dequeuing top item: " << ppl.Dequeue() << endl;
    cout << "Queue size: " << ppl.Size() << endl;
    
    cout << "Adding item \"$\"!" << endl;
    ppl.Enqueue('$');
    cout << "Queue size: " << ppl.Size() << endl;
    cout << "Peeking inside: " << ppl.Peek() << endl;
    cout << "Queue size: " << ppl.Size() << endl;
    cout << "Dequeuing top item: " << ppl.Dequeue() << endl;
    cout << "Queue size: " << ppl.Size() << endl;
    cout << "Dequeuing last 5 items!" << endl;
    for(int i = 0; i < 5; ++i)
    	cout << "----Dequeuing top item: " << ppl.Dequeue() << endl;
    cout << "Queue size: " << ppl.Size() << endl;
    cout << "Peeking inside: " << ppl.Peek() << endl;
    cout << "Dequeuing top item: " << ppl.Dequeue() << endl;
    cout << "Queue size: " << ppl.Size() << endl;
    	
    return 0;
}
