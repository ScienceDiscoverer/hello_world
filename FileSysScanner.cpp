/*
TODO:
---> IMPLEMENT ABILITY TO SAVED SCANNED FILESYSTEM TO FILE AND LOAD
---> MK FILE SYS FULL TREE GRAPH DRAW WIDTH OF BRANCH MUST BE PROPORTIONAL TO SIZE OF FILE/FOLDER IN BYTES
---> ??? MK CONSOLE OUTPUT UNICOD, ALL CHARS!!!!
*/
///*DEBUG*/ wcout << "d0" << endl;

#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <conio.h>

using namespace std;

typedef unsigned long long ulong;

class File;

class Fname
{
public:
	Fname();
	Fname(const wchar_t* ws);
	Fname(const Fname& other);
	~Fname();
	
	operator const wchar_t*() const;
	Fname& operator+(wchar_t c);
	Fname& operator+(const Fname& other);
	Fname& operator=(const Fname& other);
	
	int Size() const;
	Fname& StripName(bool name = true);
	Fname& StripPath();
	Fname& PopBack();
	Fname& PopFront(int n = 1);
	Fname& PopLastName();
	
private:
	int StrSize(const wchar_t* s) const;
	void Init(const wchar_t* ws);
	
	// Data
	wchar_t* str_;
};

class Files
{
public:
	Files() : files_(nullptr), s_(0) {}
	Files(const Files& other);
	~Files();
	
	Files& operator=(const Files& other);
	
	void Push(const File& f);
	const File& operator[](int i) const;
	int Size() const;
	
private:
	File* files_;
	int s_;
};

class File
{
public:
	File();
	File(const wchar_t* n, ulong b, bool isf, bool hid, bool sys);
	File(const File& other);
	
	File& operator=(const File& other);
	
	const Fname& Name() const; // Mb better make pring function...
	ulong Bytes() const;
	ulong BytesT() const;
	void AddBytes(ulong b);
	void AddBytesT(ulong b);
	void SetBytes(ulong b);
	void SetBytesT(ulong b);
	bool IsFolder() const;
	bool IsHidden() const;
	bool IsSystem() const;
	
	void PushFile(const wchar_t* n, ulong b, bool isf, bool hid, bool sys);
	void PushFile(const File& f);
	const File* GetFile(int i) const;
	int GetMaxFileName() const;
	
	void PrintStats() const;
	
private:
	ulong BytesTaken(ulong b) const;
	// Data
	Fname fn_;
	Files fs_;
	// Folder, Hidden and System attributes stored here in upper bits
	ulong bytes_;
	ulong bytes_taken_;
};

struct Size
{
	ulong s_real;
	ulong s_taken;
};

template<class T>
class Queue
{
	struct Item
	{
		Item* next;
		T data;
	};
	
public:
	Queue();
	~Queue();
	
	void Push(const T& item);
	void Pop();
	void Clear();
	const T& operator[](int i) const;
	const T& Last() const;
	int Size() const;
	
private:
	Item* head_;
	int s_;
};

// Functions declarations
Size scan(File& f, Fname base);
basic_ostream<wchar_t>& operator<<(basic_ostream<wchar_t>&, const Fname&);
void padding(int n, wchar_t c = L' ');
void intPadding(int n);
void printBytes(ulong b);
void pBytesFull(ulong b);
void gotoxy(int x, int y);
void printTotal(const File& f);
int calcYMin(const Fname& n);
void printArrow();
void printPercBar(int p);
void disableCursor();
int digits(ulong x);
void printIntPadded(int i, wchar_t pad = L'0');
void setAllocUnit();
void printInColor(const Fname& s, int col);
void scaleWindow();

// Globals
const ulong scan_update_mod = 12000;
ulong alloc_block = 4096;
ulong tot_files, tot_folders, tot_hidden, tot_system;
Queue<int> folds;
int max_name;
COORD original_size;

int main()
{
	setlocale(LC_ALL, "ukrainian");
	disableCursor();
	setAllocUnit();
	
	HMODULE h_mod = GetModuleHandleW(NULL);
	
	wchar_t* path = new wchar_t[MAX_PATH];
	GetModuleFileNameW(h_mod, path, MAX_PATH);
	
	Fname base = path;
	Fname tmp = L"\\\\?\\";
	base = tmp + base;
	
	delete[] path;
	
	system("cls");
	
	File root(L"", 0, true, false, false);
	
	int time = GetTickCount();
	
	if(!scan(root, base.StripName()).s_real)
	{
		wcout << endl << "Scan function failed!" << endl;
		system("pause");
		return 1;
	}
	
	time = GetTickCount() - time;
	
	system("cls");
	max_name = root.GetMaxFileName();
	scaleWindow();
	
	printTotal(root);
	
	base.PopFront(4);
	wcout << base << endl;
	wcout << "Scanning took " << time/1000/60 << " m ";
	wcout << time/1000 % 60 << " s " << time % 1000 << " ms"<< endl;
	
	root.PrintStats();
	
	Queue<const File*> f_op;
	f_op.Push(&root);
	
	int ym = calcYMin(base); // Minimal y
	int y = 0;
	if(folds.Size())
	{
		gotoxy(max_name, folds[0] + ym);
		printArrow();
	}
	
	while(true)
	{
		int c = _getch();
		switch(c)
		{
		case 224:
			c = _getch();
			if(c == 80) // Down arrow
			{
				if(y == folds.Size()-1 || !folds.Size())
				{
					break;
				}
				
				gotoxy(max_name, folds[y] + ym);
				wcout << L" ";
				gotoxy(max_name, folds[++y] + ym);
				printArrow();
			}
			else if(c == 72) // Up arrow
			{
				if(!y)
				{
					break;
				}
				
				gotoxy(max_name, folds[y] + ym);
				wcout << L" ";
				gotoxy(max_name, folds[--y] + ym);
				printArrow();
			}
			break;
		
		case 8: // Backspace
			if(f_op.Size() == 1)
			{
				break;
			}
			
			f_op.Pop();
			
			system("cls");
			max_name = f_op.Last()->GetMaxFileName();
			scaleWindow();
			
			printTotal(*f_op.Last());
			
			wcout << base.PopLastName() << endl << endl;
			
			f_op.Last()->PrintStats();
			y = 0;
			ym = calcYMin(base);
			gotoxy(max_name, folds[0] + ym);
			printArrow();		
			break;
			
		case 13: // Enter
			if(!folds.Size())
			{
				break;
			}
			
			f_op.Push(f_op.Last()->GetFile(folds[y]));
			
			system("cls");
			max_name = f_op.Last()->GetMaxFileName();
			scaleWindow();
			
			printTotal(*f_op.Last());
			base = base + L'\\' + f_op.Last()->Name();
			
			wcout << base << endl << endl;
			
			f_op.Last()->PrintStats();
			y = 0;
			ym = calcYMin(base);
			
			if(folds.Size())
			{
				gotoxy(max_name, folds[0] + ym);
				printArrow();
			}
			break;
			
		case 27: // Escape
			goto exit;
			break;
		default:
			break;
		}
	}
	exit:
	
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), original_size);
	
	return 0;
}

// Functions definitions
Size scan(File& f, Fname base)
{	
	HANDLE file;
	_WIN32_FIND_DATAW fdata;
	Size sub_s;
	
	Fname name = base + L'\\' + f.Name();
	
	if(f.Name().Size() == 0)
	{
		name.PopBack();
		base.PopBack();
	}
	
	file = FindFirstFileW(name + L'\\' + L'*', &fdata);
	
	if(tot_files % scan_update_mod == 0)
	{
		gotoxy(0, 0);
		padding(50);
		gotoxy(0, 0);
		wcout << L"Scanning... " << tot_files << L" " << tot_folders << L" " << name.Size();
	}
	
	
	
	if(file == INVALID_HANDLE_VALUE)
	{
		//wcout << L"FindFirstFileW() Failed! Error# " << GetLastError() << endl;
		Size tmp;
		tmp.s_real = 0;
		tmp.s_taken = 0;
		return tmp;
	}

	do
	{
		// Ignore "." and ".." hidden super secret system sneaky folders
		const wchar_t* fn = fdata.cFileName;
		if((fn[0] == L'.' && fn[1] == 0) || (fn[1] == L'.' && fn[2] == 0))
		{
			continue;
		}
		// Actual size
		const ulong fsize = ((ulong)fdata.nFileSizeHigh * (0xFFFFFFFFLL + 1)) + (ulong)fdata.nFileSizeLow;
		const bool isf = fdata.dwFileAttributes & 0x10;
		const bool hid = fdata.dwFileAttributes & 0x2;
		const bool sys = fdata.dwFileAttributes & 0x4;
		
		if(hid)
		{
			++tot_hidden;
		}
		
		if(sys)
		{
			++tot_system;
		}
		
		File found(fdata.cFileName, fsize, isf, hid, sys);
		
		sub_s = scan(found, base);
		
		if(isf)
		{
			f.AddBytes(sub_s.s_real);
			f.SetBytesT(f.BytesT() + sub_s.s_taken);
			found.SetBytes(sub_s.s_real);
			found.SetBytesT(sub_s.s_taken);
			++tot_folders;
		}
		else
		{
			f.AddBytes(fsize);
			f.AddBytesT(fsize);
			++tot_files;
		}
		
		f.PushFile(found);
	}
	while(FindNextFileW(file, &fdata));
	
	Size to_return;
	to_return.s_real = f.Bytes();
	to_return.s_taken = f.BytesT();
	
	FindClose(file);
	
	return to_return;
}

basic_ostream<wchar_t>& operator<<(basic_ostream<wchar_t>& os, const Fname& f)
{
	return os << (const wchar_t*)f;
}

void padding(int n, wchar_t c)
{
	for(int i = 0; i < n; ++i)
	{
		wcout << c;
	}
}

void intPadding(int n)
{
	int pad = 0;
	if(n < 10)
	{
		pad += 3;
	}
	else if(n < 100)
	{
		pad += 2;
	}
	else if(n < 1000)
	{
		pad += 1;
	}
	
	padding(pad);
}

void printBytes(ulong b)
{
	const ulong kb = 1024;
	const ulong one = 1;
	if(b >= kb && b < one << 20) // 1 KB > 1 MB
	{
		const int by = b % kb;
		intPadding(b/kb);
		wcout << b/kb << "K ";
		intPadding(by);
		if(!by)
		{
			wcout << L"  ";
		}
		else
		{
			wcout << by << "B";
		}
	}
	else if(b >= one << 20 && b < one << 30) // 1 MB > 1 GB
	{
		const int kby = (b % (one << 20))/kb;
		intPadding(b/(one << 20));
		wcout << b/(one << 20) << "M ";
		intPadding(kby);
		if(!kby)
		{
			wcout << L"  ";
		}
		else
		{
			wcout << kby << "K";
		}
	}
	else if(b >= one << 30 && b < one << 40) // 1 GB > 1 TB
	{		
		const int mb = (b % (one << 30))/(one << 20);
		intPadding(b/(one << 30));
		wcout << b/(one << 30) << "G ";
		intPadding(mb);
		if(!mb)
		{
			wcout << L"  ";
		}
		else
		{
			wcout << mb << "M";
		}
	}
	else
	{
		intPadding(b);
		wcout << b << "B      "; // 1 B
	}
}

void pBytesFull(ulong b)
{
	if(b < 1000)
	{
		wcout << b;
	}
	else if(b < 1000000)
	{
		wcout << b/1000 << L' ';
		printIntPadded(b % 1000);
	}
	else if(b < 1000000000)
	{
		wcout << b/1000000 << L' ';
		printIntPadded((b % 1000000)/1000);
		wcout << L' ';
		printIntPadded((b % 1000000) % 1000);
	}
	else if(b < 1000000000000)
	{
		wcout << b/1000000000 << L' ';
		printIntPadded((b % 1000000000)/1000000);
		wcout << L' ';
		printIntPadded(((b % 1000000000) % 1000000)/1000);
		wcout << L' ';
		printIntPadded(((b % 1000000000) % 1000000) % 1000);
	}
	else
	{
		wcout << b;
	}
}

void gotoxy(int x, int y)
{
	COORD c;
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void printTotal(const File& f)
{
	ulong b = f.Bytes();
	wcout << L" Real  Bytes |   Taken: ";
	pBytesFull(b);
	wcout << L" | ";
	pBytesFull(f.BytesT());
	wcout << endl;
	
	int sps = digits(b)/3;
	sps = digits(b) % 3 ? sps : sps - 1;
	wcout << L"Total  Files | Folders: ";
	padding(sps + digits(b) - digits(tot_files));
	wcout << tot_files << L" | " << tot_folders << endl;
	wcout << L"Total Hidden |  System: ";
	padding(sps + digits(b) - digits(tot_hidden));
	wcout << tot_hidden << L" | " << tot_system << endl << endl;
}

int calcYMin(const Fname& n)
{
	const int base_y = 5;
	CONSOLE_SCREEN_BUFFER_INFO bi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &bi);
	const int s = bi.dwSize.X;
	
	const int anti = n.Size() % s ? 0 : 1;
	
	return base_y + 1 + n.Size()/s - anti;
}

void printArrow()
{
	CONSOLE_SCREEN_BUFFER_INFO bi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &bi);
	const WORD attrs = bi.wAttributes;
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
		FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	wcout << L'<';
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attrs);
}

void printPercBar(int p)
{
	CONSOLE_SCREEN_BUFFER_INFO bi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &bi);
	const WORD attrs = bi.wAttributes;
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
		FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	for(int i = 0; i < p; ++i)
	{
		wcout << L'|';
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attrs);
}

void disableCursor()
{
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

int digits(ulong x)
{
	if(!x)
	{
		return 1;
	}
	
	int d = 0;
	while(x)
	{
		x /= 10;
		++d;
	}
	return d;
}

void printIntPadded(int i, wchar_t pad)
{
	if(i < 10)
	{
		wcout << pad << pad << i;
	}
	else if(i < 100)
	{
		wcout << pad << i;
	}
	else
	{
		wcout << i;
	}
}

void setAllocUnit()
{
	long unsigned int p[4];
	if(GetDiskFreeSpaceA(NULL, &p[0], &p[1], &p[2], &p[3]))
	{
		alloc_block = p[1] * p[0];
	}
	else
	{
		wcout << "GetDiskFreeSpace() failed! Default allocation block of ";
		wcout << alloc_block << " Bytes set." << endl;
		system("pause");
	}
}

void printInColor(const Fname& s, int col)
{
	CONSOLE_SCREEN_BUFFER_INFO bi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &bi);
	const WORD attrs = bi.wAttributes;
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), col | FOREGROUND_INTENSITY);
	wcout << s;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attrs);
}

void scaleWindow()
{
	const int max_afn = 73; // Max After Name string length
	CONSOLE_SCREEN_BUFFER_INFO bi;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(h, &bi);
	
	if(!original_size.X)
	{
		original_size = bi.dwSize;
	}
	
	if(max_name + max_afn > bi.dwSize.X)
	{
		COORD new_s = { short(max_name + max_afn + 1), short(bi.dwSize.Y) };
		
		SetConsoleScreenBufferSize(h, new_s);
	}
}

// Fname's Public Functions
Fname::Fname() : str_(nullptr)
{
	str_ = new wchar_t[1];
	str_[0] = 0;
}

Fname::Fname(const wchar_t* ws) : str_(nullptr)
{
	Init(ws);
}

Fname::Fname(const Fname& other)
{
	if(this != &other)
	{
		Init(other.str_);
	}
}

inline Fname::~Fname()
{
	delete[] str_;
}

inline Fname::operator const wchar_t*() const
{
	return str_;
}

Fname& Fname::operator+(wchar_t c)
{
	int ns = Size()+2;
	wchar_t* tmp = new wchar_t[ns];
	if(str_ != nullptr)
	{
		memcpy(tmp, str_, (ns-2) * sizeof(wchar_t));
	}
	tmp[ns-2] = c;
	tmp[ns-1] = 0;
	delete[] str_;
	str_ = tmp;
	
	return *this;
}

Fname& Fname::operator+(const Fname& other)
{
	const int s1 = Size();
	const int s2 = other.Size();
	
	wchar_t* tmp = new wchar_t[s1+s2+1];
	
	memcpy(tmp, str_, s1 * sizeof(wchar_t));
	memcpy(tmp+s1, other.str_, (s2+1) * sizeof(wchar_t));
	
	delete[] str_;
	str_ = tmp;
	
	return *this;
}

Fname& Fname::operator=(const Fname& other)
{
	if(this != &other)
	{
		int s = other.Size()+1;
		wchar_t* tmp = new wchar_t[s];
		memcpy(tmp, other.str_, s * sizeof(wchar_t));
		delete[] str_;
		str_ = tmp;
	}
	
	return *this;
}

inline int Fname::Size() const
{
	return StrSize(str_);
}

Fname& Fname::StripName(bool name)
{
	if(str_ == nullptr)
	{
		return *this;
	}
	
	const int s = Size()+1;
	
	int slash = -1;
	for(int i = 0; i < s; ++i)
	{
		slash = str_[i] == '\\' ? i : slash;
	}
	
	if(name)
	{
		// Stripping slash too...
		wchar_t* tmp = new wchar_t[slash+1];
		memcpy(tmp, str_, slash * sizeof(wchar_t));
		tmp[slash] = 0;
		delete[] str_;
		str_ = tmp;
	}
	else
	{
		const int ns = s-2-slash;
		wchar_t* tmp = new wchar_t[ns+1];
		memcpy(tmp, str_+slash+1, (ns) * sizeof(wchar_t));
		tmp[ns] = 0;
		delete[] str_;
		str_ = tmp;
	}
	
	return *this;
}

Fname& Fname::StripPath()
{
	StripName(false);
	return *this;
}

Fname& Fname::PopBack()
{
	int s = Size();
	if(s > 0)
	{
		str_[s-1] = 0;
	}
	
	return *this;
}

Fname& Fname::PopFront(int n)
{
	int s = Size() - n;
	wchar_t* tmp = new wchar_t[s+1];
	memcpy(tmp, str_+n, s * sizeof(wchar_t));
	tmp[s] = 0;
	delete[] str_;
	str_ = tmp;
	
	return *this;
}

Fname& Fname::PopLastName()
{
	for(int i = Size(); i >= 0; --i)
	{
		if(str_[i] == L'\\')
		{
			wchar_t* tmp = new wchar_t[i+1];
			memcpy(tmp, str_, i * sizeof(wchar_t));
			tmp[i] = 0;
			delete[] str_;
			str_ = tmp;
			break;
		}
	}
	
	return *this;
}

// Fname's Private Functions
int Fname::StrSize(const wchar_t* s) const
{
	if(s == nullptr)
	{
		return 0;
	}
	
	int i = 0;
	
	while(s[i])
	{
		++i;
	}
	
	return i;
}

void Fname::Init(const wchar_t* ws)
{
	int s = StrSize(ws);
	str_ = new wchar_t[s+1];
	memcpy(str_, ws, s * sizeof(wchar_t));
	str_[s] = 0;
}

// Files' functions
Files::Files(const Files& other) : s_(other.s_)
{
	files_ = new File[s_];
	for(int i = 0; i < s_; ++i)
	{
		files_[i] = other.files_[i];
	}
}

Files::~Files()
{
	delete[] files_;
}

Files& Files::operator=(const Files& other)
{
	if(this != &other)
	{
		File* tmp = files_;
		if(other.s_ != s_)
		{
			s_ = other.s_;
			tmp = new File[s_];
		}
		
		for(int i = 0; i < s_; ++i)
		{
			tmp[i] = other.files_[i];
		}
		
		delete[] files_;
		files_ = tmp;
	}
	
	return *this;
}

void Files::Push(const File& f)
{
	if(!s_)
	{
		files_ = new File[++s_];
		files_[0] = f;
		return;
	}

	File* tmp = new File[++s_];
	
	for(int i = 0; i < s_-1; ++i)
	{
		tmp[i] = files_[i];
	}
	
	tmp[s_-1] = f;
	delete[] files_;
	files_ = tmp;
}

inline const File& Files::operator[](int i) const
{
	return files_[i];
}

inline int Files::Size() const
{
	return s_;
}

// File's functions
File::File() : bytes_(0), bytes_taken_(0) {}

File::File(const wchar_t* n, ulong b, bool isf, bool hid, bool sys) : fn_(n), bytes_(b)
{
	bytes_taken_ = b ? BytesTaken(b) : 0;
	
	if(isf)
	{
		bytes_ |= 0x8000000000000000;
	}
	
	if(hid)
	{
		bytes_ |= 0x4000000000000000;
	}
	
	if(sys)
	{
		bytes_ |= 0x2000000000000000;
	}
}

File::File(const File& other) : bytes_(other.bytes_), bytes_taken_(other.bytes_taken_)
{
	fn_ = other.fn_;
	fs_ = other.fs_;
}
	
File& File::operator=(const File& other)
{
	bytes_ = other.bytes_;
	bytes_taken_ = other.bytes_taken_;
	fn_ = other.fn_;
	fs_ = other.fs_;
	
	return *this;
}
	
inline const Fname& File::Name() const { return fn_; }

inline ulong File::Bytes() const { return bytes_ & 0x1FFFFFFFFFFFFFFF; }

inline ulong File::BytesT() const {	return bytes_taken_; }

inline void File::AddBytes(ulong b)
{
	SetBytes(Bytes() + b);
}

inline void File::AddBytesT(ulong b)
{
	bytes_taken_ += BytesTaken(b);
}

void File::SetBytes(ulong b)
{
	bytes_ &= 0xE000000000000000;
	bytes_ |= b;
}

inline void File::SetBytesT(ulong b)
{
	bytes_taken_ = b;
}

inline bool File::IsFolder() const { return bytes_ & 0x8000000000000000; }

inline bool File::IsHidden() const { return bytes_ & 0x4000000000000000; }

inline bool File::IsSystem() const { return bytes_ & 0x2000000000000000; }

	
inline void File::PushFile(const wchar_t* n, ulong b, bool isf, bool hid, bool sys)
{
	fs_.Push(File(n, b, isf, hid, sys));
}

inline void File::PushFile(const File& f)
{
	fs_.Push(f);
}

inline const File* File::GetFile(int i) const
{
	return &fs_[i];
}

int File::GetMaxFileName() const
{
	int mn = 0;
	for(int i = 0; i < fs_.Size(); ++i)
	{
		if(fs_[i].Name().Size() > mn)
		{
			mn = fs_[i].Name().Size();
		}
	}
	
	return mn;
}

void File::PrintStats() const
{
	const int max_bar = 40;
	max_name = GetMaxFileName(); // Global variable
	
	folds.Clear();
	for(int i = 0; i < fs_.Size(); ++i)
	{
		if(fs_[i].IsFolder())
		{
			folds.Push(i);
		}
	}
	
	for(int i = 0; i < fs_.Size(); ++i)
	{
		padding(max_name - fs_[i].Name().Size());
		
		if(fs_[i].IsHidden() && fs_[i].IsSystem())
		{
			printInColor(fs_[i].Name(), FOREGROUND_RED);
		}
		else if(fs_[i].IsHidden())
		{
			printInColor(fs_[i].Name(), FOREGROUND_BLUE);
		}
		else if(fs_[i].IsSystem())
		{
			printInColor(fs_[i].Name(), FOREGROUND_GREEN | FOREGROUND_RED);
		}
		else
		{
			wcout << fs_[i].Name();
		}
		wcout << L"  ";
		
		printBytes(fs_[i].Bytes());
		wcout << L" ";
		printBytes(fs_[i].bytes_taken_);
		wcout << L" ";
		
		double perc = (double)fs_[i].bytes_taken_/(double)bytes_taken_;
		printPercBar(int((double)max_bar * perc));
		
		wcout << fixed << setprecision(2) << perc * 100.0 << L"%";
		
		wcout << endl;
	}
}

// File's Privat Functions
ulong File::BytesTaken(ulong b) const
{
	const ulong mod = b % alloc_block;
	if(!mod)
	{
		return b;
	}
	
	return alloc_block - mod + b;
}

// Queue's functions
template<class T>
Queue<T>::Queue() : head_(nullptr), s_(0) {}

template<class T>
Queue<T>::~Queue()
{
	Clear();
}

template<class T>
void Queue<T>::Push(const T& item)
{
	if(!s_)
	{
		head_ = new Item;
		head_->next = nullptr;
		head_->data = item;
	}
	else
	{
		Item* last = head_;
		
		while(last->next != nullptr)
		{
			last = last->next;
		}
		
		last->next = new Item;
		last->next->next = nullptr;
		last->next->data = item;
	}
	
	++s_;
}

template<class T>
void Queue<T>::Pop()
{
	if(s_ == 1)
	{
		delete head_;
		head_ = nullptr;
	}
	else if(!s_)
	{
		return;
	}
	else
	{
		Item* last = head_;
		Item* prelast = head_;
		
		while(last->next != nullptr)
		{
			prelast = last;
			last = last->next;
		}
		delete last;
		prelast->next = nullptr;
	}
	
	--s_;
}

template<class T>
void Queue<T>::Clear()
{
	if(!s_)
	{
		return;
	}
	
	s_ = 0;
	Item* past = head_;
	Item* cur = past;
	head_ = nullptr;
	
	while(cur->next != nullptr)
	{
		cur = past->next;
		delete past;
		past = cur;
	}
	
	delete cur;
}

template<class T>
const T& Queue<T>::operator[](int i) const
{
	Item* res = head_;
	int k = 0;
	while(k != i)
	{
		res = res->next;
		++k;
	}
	
	return res->data;
}

template<class T>
const T& Queue<T>::Last() const
{
	Item* last = head_;
		
	while(last->next != nullptr)
	{
		last = last->next;
	}
	
	return last->data;
}

template<class T>
inline int Queue<T>::Size() const
{
	return s_;
}
