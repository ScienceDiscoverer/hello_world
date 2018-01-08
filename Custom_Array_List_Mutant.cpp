#include <cassert>
#include <cstring>
#include <iostream>

using namespace std;

template <class T>
class SciArray
{
public:
	// Constructors
	SciArray() : size_(0), cur_size(0), data(nullptr), indices(nullptr), ds(20) {}
	SciArray(int size);

	// Rule of 5
	~SciArray();
	SciArray(const SciArray<T>& other) = delete;
	SciArray(SciArray<T>&& other) = delete;
	SciArray<T>& operator=(const SciArray<T>& other) = delete;
	SciArray<T>& operator=(SciArray<T>&& other) = delete;

	// Access operators
	T& operator[](int i);
	const T& operator[](int i) const;
	int Size() const { return cur_size; }

	// Data manipulation
	void Push(const T& obj); // Add object at the end of array
	void Pop(int index); // Remove object at index

private:
	// Private functions
	void AllocArrays();
	void AddObj(const T& obj);
	void ExpandArrays();
	void CleanUp();

	// Data
	const int ds; // Delta Size
	int cur_size; // Current size used
	int size_; // Real size
	int* indices;
	T* data;
};

// Constructors
template <class T>
SciArray<T>::SciArray(int size) : size_(size), cur_size(0), ds(20)
{
	assert(size >= 0);

	if(size)
	{
		AllocArrays();
	}
	else
	{
		data = nullptr;
		indices = nullptr;
	}
}
// End of Constructors

// Rule of 5
template <class T>
SciArray<T>::~SciArray()
{
	CleanUp();
}
// End of Rule of 5

// Access operators
template <class T>
T& SciArray<T>::operator[](int i)
{
	assert(i >= 0 && i < cur_size);
	for(int j = 0; j < size_; ++j)
	{
		if(indices[j] == i)
		{
			return data[j];
		}
	}
}

template <class T>
const T& SciArray<T>::operator[](int i) const
{
	return *this[i];
}
// End of Access operators

// Data manipulation
// Add object at the end of array
template <class T>
void SciArray<T>::Push(const T& obj)
{
	switch(size_)
	{
	case 0:
		size_ = ds;
		AllocArrays();
		AddObj(obj);
		break;

	default:
		if(cur_size == size_)
		{
			ExpandArrays();
			AddObj(obj);
		}
		else
		{
			AddObj(obj);
		}
		break;
	}
}

// Remove object at index
template <class T>
void SciArray<T>::Pop(int index)
{
	assert(index >= 0 && index < cur_size);

	for(int i = 0; i < size_; ++i)
	{
		if(indices[i] == index)
		{
			// Decrement all indices that is larger than deleted index
			for(int j = 0; j < size_; ++j)
			{
				if(indices[j] > indices[i])
				{
					--indices[j];
				}
			}
			// Mark deleted index's cell as free
			indices[i] = -1;
			--cur_size;
			break;
		}
	}
}
// End of Data manipulation

// Private functions
template <class T>
void SciArray<T>::AllocArrays()
{
	data = new T[size_];
	indices = new int[size_];
	// Mark all indices as free cell
	memset(indices, -1, size_*sizeof(int));
}

template <class T>
void SciArray<T>::AddObj(const T& obj)
{
	for(int i = 0; i < size_; ++i)
	{
		if(indices[i] == -1)
		{
			// Store object at free array cell
			data[i] = obj;
			// Set i-th object's index to cur.size and increment it
			indices[i] = cur_size++;
			break;
		}
	}
}

template <class T>
void SciArray<T>::ExpandArrays()
{
	int new_size = size_ + ds;
	T* new_data = new T[new_size];
	int* new_indices = new int[new_size];

	// Set all new indicies as free cells
	memset(new_indices, -1, new_size*sizeof(int));
	// Copy data to new arrays
	memcpy(new_data, data, size_*sizeof(T));
	memcpy(new_indices, indices, size_*sizeof(int));

	CleanUp();

	size_ = new_size;
	data = new_data;
	indices = new_indices;
}

template <class T>
void SciArray<T>::CleanUp()
{
	if(data)
	{
		delete[] data;
	}

	if(indices)
	{
		delete[] indices;
	}
}
// End of Private functions

int main()
{
	SciArray<int> ints(2);
	int count = 6;
	while(count--)
	{
		cout << "Current array (size " << ints.Size() << "): ";
		for(int i = 0; i < ints.Size(); ++i)
			cout << ints[i] << ", ";
		cout << endl;
		
		int x;
		cout << "Enter int to add to array: ";
		cin >> x;
		
		ints.Push(x);
		
		cout << "Array after adding number (size " << ints.Size() << "): ";
		for(int i = 0; i < ints.Size(); ++i)
			cout << ints[i] << ", ";
		cout << endl;
	}
	
	while(true)
	{
		int d;
		cout << "Choose element to delete from array (enter negative to not delete):" << endl;
		cin >> d;
		if(d >= 0)
			ints.Pop(d);
		
		cout << "Array after deleting number (size " << ints.Size() << "): ";
		for(int i = 0; i < ints.Size(); ++i)
			cout << ints[i] << ", ";
		cout << endl;
	}
	
	return 0;
}
