#include <iostream>
#include <Windows.h>

using namespace std;

typedef unsigned long long ulong;

class File;

const ulong alloc_block = 4096;

class Fname
{
public:
	Fname();
	Fname(const wchar_t* ws);
	Fname(const Fname& other);
	~Fname();
	
	operator const wchar_t*() const;
	Fname& operator+(wchar_t c);
	Fname& operator=(const Fname& other);
	
	int Size() const;
	Fname& StripName(bool name = true);
	Fname& StripPath();
	
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
	~Files();
	
	void Push(const File& f);
	const File& operator[](int i);
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
	
	const Fname& Name() const; // Mb better make pring function...
	ulong Bytes() const;
	void AddBytes(ulong b);
	bool IsFolder() const;
	bool IsHidden() const;
	bool IsSystem() const;
	
	void PushFile(const wchar_t* n, ulong b, bool isf, bool hid, bool sys);
	void PushFile(const File& f);
	
private:
	void SetBytes(ulong b);
	ulong BytesTaken(ulong b) const;
	// Data
	Fname fn_;
	Files fs_;
	// Folder, Hidden and System attributes stored here in upper bits
	ulong bytes_;
	ulong bytes_taken_;
};

// Functions declarations
bool scan(File& f);
basic_ostream<wchar_t>& operator<<(basic_ostream<wchar_t>&, Fname&);

int main()
{
	HMODULE h_mod = GetModuleHandleW(NULL);
	
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(h_mod, path, MAX_PATH);
	
	Fname name = path;
	Fname name2 = name;
	Fname name3 = name;
	
	const wchar_t* p = name;
	
	wcout << p << endl;
	wcout << name << L" Size: " << name.Size() << endl;
	wcout << name2.StripName() << endl;
	wcout << name3.StripPath() << endl;
	//wcout << name2 + L'*' << endl;
	
	File root(name.StripName(), 0, true, false, false);
	
	if(!scan(root))
	{
		wcout << "Scan function failed!" << endl;
		return 1;
	}
	
	return 0;
}

// Functions definitions
bool scan(File& f)
{
	HANDLE file;
	_WIN32_FIND_DATAW fdata;
	
	Fname name = f.Name();
	
	file = FindFirstFileW(name + L'\\' + L'*', &fdata);
	
	//////////////// DEBUGGING ////////////////
	wcout << L"Scanning...: " << name << endl;
	//////////////// DEBUGGING ////////////////
	
	if(file == INVALID_HANDLE_VALUE)
	{
		wcout << L"FindFirstFileW() Failed! Error# " << GetLastError() << endl;
		return false;
	}
	//void PushFile(const wchar_t* n, ulong b, bool isf, bool hid, bool sys);
	do
	{
		// Ignore "." and ".." hidden super secret system sneaky folders
		if(fdata.cFileName[0] == L'.')
		{
			continue;
		}
//////////////// DEBUGGING ////////////////
		wcout << L"File Found: " << fdata.cFileName << endl;
//////////////// DEBUGGING ////////////////
		// Actual size
		const ulong fsize = (fdata.nFileSizeHigh * (0xFFFFFFFF + 1)) + fdata.nFileSizeLow;
//////////////// DEBUGGING ////////////////
		wcout << L"1" << endl;
//////////////// DEBUGGING ////////////////
		const bool isf = fdata.dwFileAttributes & 0x10;
		const bool hid = fdata.dwFileAttributes & 0x2;
		const bool sys = fdata.dwFileAttributes & 0x4;
		File found(fdata.cFileName, fsize, isf, hid, sys);
//////////////// DEBUGGING ////////////////
		wcout << L"2" << endl;
//////////////// DEBUGGING ////////////////
		f.PushFile(found);
//////////////// DEBUGGING ////////////////
		wcout << L"3" << endl;
//////////////// DEBUGGING ////////////////
		f.AddBytes(fsize);
//////////////// DEBUGGING ////////////////
		wcout << L"4" << endl;
//////////////// DEBUGGING ////////////////
		scan(found);
//////////////// DEBUGGING ////////////////
		wcout << L"5" << endl;
//////////////// DEBUGGING ////////////////		
	}
	while(FindNextFileW(file, &fdata));
	
	FindClose(file);
	return true;
}

basic_ostream<wchar_t>& operator<<(basic_ostream<wchar_t>& os, Fname& f)
{
	return os << (const wchar_t*)f;
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
Files::~Files()
{
	delete[] files_;
}

void Files::Push(const File& f)
{
	if(!s_)
	{
//////////////// DEBUGGING ////////////////
		wcout << L"Inside new pointer if... s: " << s_ << endl;
//////////////// DEBUGGING ////////////////
		files_ = new File[++s_];
		files_[0] = f;
		return;
	}
//////////////// DEBUGGING ////////////////
		wcout << L"push_1... s: " << s_ << endl;
//////////////// DEBUGGING ////////////////
	File* tmp = new File[++s_];
//////////////// DEBUGGING ////////////////
		wcout << L"push_2... s: " << s_ << endl;
//////////////// DEBUGGING ////////////////
	memcpy(tmp, files_, (s_-1) * sizeof(File));
//////////////// DEBUGGING ////////////////
		wcout << L"push_3... s: " << s_ << endl;
//////////////// DEBUGGING ////////////////
	tmp[s_-1] = f;
//////////////// DEBUGGING ////////////////
		wcout << L"push_4... s: " << s_ << endl;
//////////////// DEBUGGING ////////////////
	delete[] files_;
//////////////// DEBUGGING ////////////////
		wcout << L"push_5... s: " << s_ << endl;
//////////////// DEBUGGING ////////////////
	files_ = tmp;
}

inline const File& Files::operator[](int i)
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
	bytes_taken_ = BytesTaken(b);
	
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
	
inline const Fname& File::Name() const { return fn_; }

inline ulong File::Bytes() const { return bytes_ & 0x1FFFFFFFFFFFFFFF; }

inline void File::AddBytes(ulong b)
{
	SetBytes(Bytes() + b);
	bytes_taken_ += BytesTaken(b);
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

// File's Privat Functions
void File::SetBytes(ulong b)
{
	bytes_ &= 0xE000000000000000;
	bytes_ |= b;
}

ulong File::BytesTaken(ulong b) const
{
	if(b < alloc_block)
	{
		return alloc_block;
	}
	
	return b + b % alloc_block;
}
