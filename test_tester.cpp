#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;

const char dot = 0x2E;
const char zero = 0x30;
const char nine = 0x39;
const char space = 0x20;
const char tab = 0x09;
const char cr = 0x0D;
const char lf = 0x0A;
const wchar_t big_i = 0x0406;
const wchar_t small_ge = 0x0491;
const wchar_t apost = 0x2019;
const wchar_t wspace = (wchar_t)space;

void compTests(const char* fname1, const char* fname2);
int qcomp(wstring& ws1, wstring& ws2);
void extractq(const char* fname);
int uniToInt(char a, char b, char c);
string wideToMulti(const wstring& str);
wstring multiToWide(const string& str);
void loadingBar(int amount);
int comlen(wchar_t *p, wchar_t *q);
int pstrcmp(const void *p, const void *q);
float simComp(const wstring& s1, const wstring& s2);
float wordComp(const wstring& s1, const wstring& s2);
void markDifs(wstring& s1, wstring& s2);

class Question
{
public:
	Question() = default;
	Question(int n, wstring str) : num(n), quest(str), used(false) {}
	
	float Compare(const Question& q) const
	{
		if(!quest.compare(q.quest))
		{
			return 1.0f;
		}
		
		float sim = simComp(quest, q.quest);
		if(sim > 0.5f)
		{
			return sim;
		}
		float simw = wordComp(quest, q.quest);
		if(simw > 0.5f)
		{
			if(simw == 1.0f)
			{
				return 0.99f;
			}
			return simw;
		}
		
		return sim > simw ? sim : simw;
	}
	
	void Use()
	{
		used = true;
	}
	
	bool IsUsed() const
	{
		return used;
	}
	
	void Write(ofstream& ofs)
	{
		ofs << "<p>" << num << ". ";
		ofs << wideToMulti(quest) << endl;
	}
	
	void WriteSame(Question& q, ofstream& ofs)
	{
		ofs << "<p>" << num << " / " << q.num << ": ";
		ofs << wideToMulti(quest) << endl;
	}
	
	void WriteSim(Question& q, ofstream& ofs)
	{
		markDifs(quest, q.quest);
		ofs << "<p>" << num << ". ";
		ofs << wideToMulti(quest) << endl;
		ofs << "<p>" << q.num << ". ";
		ofs << wideToMulti(q.quest) << endl;
	}
	
private:
	int num;
	wstring quest;
	bool used;
};

struct SimQuests
{
	Question new_quest;
	Question old_quest;
	float sim;
};

int main(int agrc, char *argv[])
{
	if(agrc < 2 || agrc > 3)
	{
    	cout << "Correct usage: " << argv[0] << " 'newtest' 'oldtest' ---> Compare tests" << endl;
    	cout << "Correct usage: " << argv[0] << " 'filename' ---> Extract questions from file" << endl;
    	return 0;
    }
    
	if(agrc == 3)
    {
    	compTests(argv[1], argv[2]);
    }
    else
    {
		extractq(argv[1]);
    }

	return 0;
}

void compTests(const char* fname1, const char* fname2)
{
	vector<Question> test1;
	vector<Question> test2;
	
	ifstream tst1(fname1, ios::in | ios::binary);
	if(!tst1.is_open())
	{
		cout << "ERROR OPENING FILE 1!" << endl;
		return;
	}
	
	char b = ']';
	string tmp;
	int i = 0;
	while(tst1.get(b))
	{
		if(b == cr)
		{
			continue;
		}
		else if(b == lf)
		{
			++i;
			test1.push_back(Question(i, multiToWide(tmp)));
			cout << "                                                                 \r";
			cout << "Loading new questions: " << i << ' ';
			loadingBar(i);
			tmp.clear();
			continue;
		}
		tmp.push_back(b);
	}
	cout << endl;
	
	tst1.close();
	
	ifstream tst2(fname2, ios::in | ios::binary);
	if(!tst2.is_open())
	{
		cout << "ERROR OPENING FILE 2!" << endl;
		return;
	}
	
	b = ']';
	tmp.clear();
	i = 0;
	while(tst2.get(b))
	{
		if(b == cr)
		{
			continue;
		}
		else if(b == lf)
		{
			++i;
			test2.push_back(Question(i, multiToWide(tmp)));
			cout << "                                                                 \r";
			cout << "Loading old questions: " << i << ' ';
			loadingBar(i);
			tmp.clear();
			continue;
		}
		tmp.push_back(b);
	}
	cout << endl;
	
	tst2.close();
	
	ofstream cmpres("comp_result.html", ios::out | ios::binary);
	cmpres << "<html>\n<head>\n";
	cmpres << "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">" << endl;
	cmpres << "<title>New VS Old Test</title>\n</head>\n<body>\n<pre>\n";
	
	vector<Question> sameq;
	vector<Question> newq;
	vector<SimQuests> simq;
	
	for(i = 0; i < test1.size(); ++i)
	{
		vector<SimQuests> insimq;
		for(int j = 0; j < test2.size(); ++j)
		{
			if(test2[j].IsUsed())
			{
				continue;
			}
			
			float sim = test1[i].Compare(test2[j]);
			
			if(sim == 1.0f)
			{
				sameq.push_back(test1[i]);
				sameq.push_back(test2[j]);
				test2[j].Use();
				goto endcheck;
			}
			else if(sim > 0.5f)
			{
				SimQuests tmp;
				tmp.new_quest = test1[i];
				tmp.old_quest = test2[j];
				tmp.sim = sim;
				insimq.push_back(tmp);
			}
		}
		if(insimq.empty())
		{
			newq.push_back(test1[i]);
		}
		else
		{
			SimQuests max_sim;
			float max = 0.0f;
			for(auto q : insimq)
			{
				if(q.sim > max)
				{
					max = q.sim;
					max_sim = q;
				}
			}
			simq.push_back(max_sim);
		}
		endcheck:;
		cout << "                                                                 \r";
		cout << "Questions compared: " << i+1 << ' ';
		loadingBar(i);
	}
	
	cmpres << "<h2>" << sameq.size()/2 << " (" << (sameq.size()/2)/(float)test1.size()*100.0 << "%) same questions:</h2>\n";
	for(int i = 0; i < sameq.size(); i += 2)
	{
		sameq[i].WriteSame(sameq[i+1], cmpres);
	}
	
	cmpres << "<h2>" << newq.size() << " (" << newq.size()/(float)test1.size()*100.0 << "%) new questions:</h2>\n";
	for(auto q : newq)
	{
		q.Write(cmpres);
	}
	
	cmpres << "<h2>" << simq.size() << " (" << simq.size()/(float)test1.size()*100.0 << "%) similar questions:</h2>\n";
	for(auto q : simq)
	{
		q.new_quest.WriteSim(q.old_quest, cmpres);
		cmpres << "<hr>\n";
	}
	
	cmpres << "</pre>\n</body>\n</html>";
	cmpres.close();
}

int qcomp(wstring& ws1, wstring& ws2)
{
	int diffs = 0;
	int small_sz = ws1.size() >= ws2.size() ? (int)ws2.size() : (int)ws1.size();
	
	for(int i = 0; i < small_sz; ++i)
	{
		if(ws1[i] != ws2[i])
		{
			++diffs;
		}
	}
	
	return diffs;
}

void extractq(const char* fname)
{
	string qfn(fname);
	string afn(fname);
	for(int i = 0; i < 4; ++i)
	{
		qfn.pop_back();
		afn.pop_back();
	}
	
	ifstream inp(fname, ios::in | ios::binary);
	
	if(!inp.is_open())
	{
		cout << "ERROR OPENING FILE!" << endl;
		return;
	}
	
	ofstream qest(qfn.append("_q.txt").c_str(), ios::out | ios::binary);
	ofstream answ(afn.append("_a.txt").c_str(), ios::out | ios::binary);
	
	int q_fund = 0, q_end = 0;
	
	bool q_on = false;
	char b0 = ']';
	char b1 = ']';
	char b2 = ']';
	char b3 = ']';
	char b4 = ']';
	string q, a;
	while(inp.get(b0))
	{
		if(q_on)
		{
			q.push_back(b0);
		}
		else
		{
			a.push_back(b0);
		}
		
		if((b2 >= zero && b2 <= nine) && b1 == dot && (b0 == space || b0 == tab) && uniToInt(b4,b3,b2) > q_fund)
		{
			q_on = true;
			answ << a;
			a.clear();
			++q_fund;
		}
		
		if(b0 == cr && q_on)
		{
			q_on = false;
			a.push_back(cr); // CR gets eaten by question!
			qest << q << endl;
			q.clear();
			++q_end;
		}
		
		b4 = b3;
		b3 = b2;
		b2 = b1;
		b1 = b0;
		
		cout << q_fund << " questions found, " << q_end << " questions ended! " << q_fund/500.0 * 100.0 << "%\r";
	}
	
	inp.close();
	qest.close();
	answ.close();	
}

int uniToInt(char a, char b, char c)
{
	if((a >= zero && a <= nine) && ((b > nine || b < zero) || (c > nine || c < zero)))
	{
		return 0;
	}
	
	int num;
	switch(c)
	{
		case 0x30: num = 0; break;
		case 0x31: num = 1; break;
		case 0x32: num = 2; break;
		case 0x33: num = 3; break;
		case 0x34: num = 4; break;
		case 0x35: num = 5; break;
		case 0x36: num = 6; break;
		case 0x37: num = 7; break;
		case 0x38: num = 8; break;
		case 0x39: num = 9; break;
		default: num = 0; break;
	}
	
	switch(b)
	{
		case 0x30: num += 0; break;
		case 0x31: num += 10; break;
		case 0x32: num += 20; break;
		case 0x33: num += 30; break;
		case 0x34: num += 40; break;
		case 0x35: num += 50; break;
		case 0x36: num += 60; break;
		case 0x37: num += 70; break;
		case 0x38: num += 80; break;
		case 0x39: num += 90; break;
		default: num += 0; break;
	}
	
	switch(a)
	{
		case 0x30: num += 0; break;
		case 0x31: num += 100; break;
		case 0x32: num += 200; break;
		case 0x33: num += 300; break;
		case 0x34: num += 400; break;
		case 0x35: num += 500; break;
		case 0x36: num += 600; break;
		case 0x37: num += 700; break;
		case 0x38: num += 800; break;
		case 0x39: num += 900; break;
		default: num += 0; break;
	}
	
	return num;
}

string wideToMulti(const wstring& str)
{
	int s_need = WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0, NULL, NULL);
	string tst_short(s_need, 0);
	WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), &tst_short[0], (int)tst_short.size(), NULL, NULL);
	return tst_short;
}

wstring multiToWide(const string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    wstring wstr( size_needed, 0 );
    MultiByteToWideChar (CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
    return wstr;
}

void loadingBar(int amount)
{
	float perc = amount/500.0f;
	int bars = (int)(20*perc);
	cout << '[';
	for(int i = 0; i < bars; ++i)
	{
		cout << '|';
	}
	for(int i = 0; i < 20-bars; ++i)
	{
		cout << ' ';
	}
	cout << "] " << perc*100.0f << '%';
	cout << "\r";
}

int comlen(wchar_t *p, wchar_t *q)
{
	int i = 0;
	while(*p && (*p++ == *q++))
	{
		++i;
	}
	return i;
}

int pstrcmp(const void *p, const void *q)
{
	return wcscmp(*((wchar_t**)p), *((wchar_t**)q));
}

float simComp(const wstring& s1, const wstring& s2)
{
	wstring comb = s1 + s2;
	const int size = comb.size();
	wchar_t* suff[size];
	for(int i = 0; i < size; ++i)
	{
		suff[i] = &comb[i];
	}
	
	qsort(suff, size, sizeof(wchar_t*), pstrcmp);
	
	int maxlen = -1, maxi;
	for(int i = 0; i < size; ++i)
	{
		int coml = comlen(suff[i], suff[i+1]);
		
		if(coml > maxlen)
		{
			maxlen = coml;
			maxi = i;
		}
	}

	return maxlen/((s1.size() + s2.size())/2.0f);
}

float wordComp(const wstring& s1, const wstring& s2)
{
	vector<wstring> words1;
	vector<wstring> words2;
	
	wstring word;
	for(const wchar_t& wc : s1)
	{
		if((wc >= big_i && wc <= small_ge) || wc == apost)
		{
			word.push_back(wc);
		}
		else if(wc == wspace && !word.empty())
		{
			words1.push_back(word);
			word.clear();
		}
	}
	words1.push_back(word);
	
	word.clear();
	for(const wchar_t& wc : s2)
	{
		if((wc >= big_i && wc <= small_ge) || wc == apost)
		{
			word.push_back(wc);
		}
		else if(wc == wspace && !word.empty())
		{
			words2.push_back(word);
			word.clear();
		}
	}
	words2.push_back(word);
	float mean_w = (words1.size()+words2.size())/2.0f;
	
	int sim_words = 0;
	for(int i = 0; i < words1.size(); ++i)
	{
		for(int j = 0; j < words2.size(); ++j)
		{
			if(!words1[i].compare(words2[j]))
			{
				++sim_words;
				words2.erase(words2.begin()+j);
				break;
			}
		}
	}
	
	return sim_words/mean_w;
}

void markDifs(wstring& s1, wstring& s2)
{
	int i = 0;
	while(s1[i] != 0 && s2[i] != 0 && s1[i] == s2[i])
	{
		++i;
	}
	s1.insert((wstring::size_type)i, L"<b><u>");
	s2.insert((wstring::size_type)i, L"<b><u>");
	i = s1.size();
	int j = s2.size();
	
	while(s1[i] == s2[j] && i >= 0 && j >= 0)
	{
		--i;
		--j;
	}
	s1.insert((wstring::size_type)i+1, L"</b></u>");
	s2.insert((wstring::size_type)j+1, L"</b></u>");
}
