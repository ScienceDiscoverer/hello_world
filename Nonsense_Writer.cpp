#define WIN32_LEAN_AND_MEAN	
#include <iostream>
#include <fstream>
#include <cstring>
#include <windows.h>
#include "SciMersenne.h"

using namespace std;

char inputchars[10000000]; // 10 Mb of text max
char* word[2000000]; // 2 million words max
const int k = 2; // k-gramm, number of words to search word after
int nword = 0;

int wordncmp(char* p, char* q);
int sortcmp(const void *p, const void *q);
char* skip(char* p, int n);
int strToInt(char* p);
int pows(int num, int pow);
int powRecurse(int n, int res, int pow);

int main(int argc, char** argv)
{
	//SetConsoleOutputCP(CP_UTF8);
	//SetConsoleCP(CP_UTF8);
	setlocale(LC_ALL, "russian");
	
	if(argc != 3)
	{
		cout << "Correct usage:" << endl;
		cout << "Nonsense_Writer 'filename' 'num_words_to_generate'\n";
		return 0;
	}
	
	int wordsleft = strToInt(argv[2]);
	int w_to_gen = wordsleft;
	
	if(wordsleft <= 0)
	{
		cout << "Incorrect number of words to generate!";
		return 0;
	}
	
	ifstream book(argv[1]);
	
	if(!book.is_open())
	{
		cout << "Error opening file!" << endl;
		return 0;
	}
	
	word[0] = inputchars;
	
	while(book >> word[nword])
	{
		word[nword+1] = word[nword] + strlen(word[nword]) + 1; // +1 for null char
		++nword;
	}
	
	book.close();
	cout << "Total number of words: " << nword << endl;
	
	for(int i = 0; i < k; ++i)
	{
		word[nword][i] = 0;
	}
	
	qsort(word, nword, sizeof(word[0]), sortcmp);
	
	char *phrase, *p, *cur_phrase, cmd;
	string story;
	phrase = inputchars;
	cur_phrase = phrase;
	
	while(cmd != 'e')
	{
		int l, m, u;
		cmd = '|';
		wordsleft = w_to_gen;
		phrase = cur_phrase;
		story.clear();
		
		for(int i = 0; i < k; ++i)
		{
			cout << skip(phrase, i) << " ";
		}
		
		for(int i = 0; i < k; ++i)
		{
			story += skip(phrase, i);
			story.push_back(' ');
		}
		
		for(; wordsleft > 0; --wordsleft)
		{
			l = -1;
			u = nword;
			while(l+1 != u) // Binary Search first occurence edition
			{
				m = (l + u)/2;
				if(wordncmp(word[m], phrase) < 0)
				{
					l = m;
				}
				else
				{
					u = m;
				}
			}
			
			// Find total amount of simila k-grams
			unsigned int total;
			for(total = 0; wordncmp(phrase, word[u+total]) == 0; ++total);
			
			SciMersenne rng;
			p = word[u + rng.Range(0, total-1)];
			phrase = skip(p, 1); // Phrase is now 2 words: old one and new one
			if(strlen(skip(phrase, k-1)) == 0)
			{
				break; // Carefull of last phrase in document
			}
			//cout << skip(phrase, k-1) << " ";
			printf("%s ", skip(phrase, k-1));
			story += skip(phrase, k-1);
			story.push_back(' ');
		}
		
		re_menu:
		cout << endl << "-----------------------------------------------------------------\n";
		cout << "r - regen; n - new s. phrase; w - words num.; s - save; e - exit.\n";
		cout << "-----------------------------------------------------------------\n";
		re_enter:
		cout << "Enter command ---> ";
		cin >> cmd;
		
		switch(cmd)
		{
		case 'r':
			break;
		case 'n':
		{
			while(true)
			{
				char tmpchars[1000];
				char* new_phrase[k];
				new_phrase[0] = tmpchars;
				for(int i = 0; i < k; ++i)
				{
					cin >> new_phrase[i];
					new_phrase[i+1] = new_phrase[i] + strlen(new_phrase[i]) + 1; // +1 for null char
				}
				
				int l, m, u;
				l = -1;
				u = nword;
				while(l+1 != u) // Binary Search
				{
					m = (l + u)/2;
					if(wordncmp(word[m], tmpchars) < 0)
					{
						l = m;
					}
					else
					{
						u = m;
					}
				}
				if(wordncmp(word[u], tmpchars) == 0)
				{
					cur_phrase = word[u];
					break;
				}
				else
				{
					cout << "There is no phrase like this in the book.\n";
					//MAKE 5 VARIANTS DIFFERENT!
					cout << "Try one of this:\n" << word[u] << " ";
					for(int i = 1; i < k; ++i)
					{
						cout << skip(word[u], i);
						if(i < k-1)
						{
							cout << " ";
						}
					}
					cout << endl;
					
					char* last_phr = word[u];
					int vars = 4;
					while(vars > 0 && word[++u][0] != 0)
					{
						if(wordncmp(word[u], last_phr))
						{
							for(int i = 0; i < k; ++i)
							{
								cout << skip(word[u], i);
								if(i < k-1)
								{
									cout << " ";
								}
							}
							cout << endl;
							--vars;
							last_phr = word[u];
						}
					}
				}
			}
			break;
		}
		case 'w':
			cin >> w_to_gen;
			break;
		case 's':
		{
			ofstream log("Nonsense_Writer_Progress.txt", ios::out | ios::app);
			if(!log.is_open())
			{
				cout << "Error opening file!" << endl;
				goto re_menu;
			}
			story.pop_back();
			log << "-----------------------------------------";
			log << "----------------------------------------\n";
			log << nword << " words, " << argv[1] << ", " << w_to_gen << " words\n";
			log << story << endl;
			log.close();
			cout << "Story saved successfully!\n";
			goto re_enter;
			break;
		}
		case 'e':
			break;
		default:
			goto re_enter;
			break;
		}
	}
	
	//MessageBox(NULL, "cout is fuked!", "Error", MB_OK | MB_ICONERROR);
	
	return 0;
}

int wordncmp(char* p, char* q)
{
	int n = k;
	for(; *p == *q; ++p, ++q)
	{
		if(*p == 0 && --n == 0)
		{
			return 0;
		}
	}
	return *p - *q;
}

int sortcmp(const void *p, const void *q)
{
	return wordncmp(*((char**)p), *((char**)q));
}

char* skip(char* p, int n)
{
	for(; n > 0; ++p)
	{
		if(*p == 0)
		{
			--n;
		}
	}
	return p;
}

int strToInt(char* p)
{
	const int num_size = strlen(p);
	int nums[num_size];
	for(int i = 0; *p != 0; ++p, ++i)
	{
		if(*p < 48 || *p > 57)
		{
			return -1;
		}
		//nums[i] = *p - 48;
		nums[i] = *p & 15;
	}
	
	int n = 0;
	for(int i = 0; i < num_size-1; ++i)
	{
		n += nums[i] * pows(10, num_size-1-i);
	}
	
	return n + nums[num_size-1];
}

int pows(int num, int pow)
{
	return powRecurse(num, num, pow);
}

int powRecurse(int n, int res, int pow)
{
	if(pow == 0)
	{
		return 1;
	}
	else if(pow == 1)
	{
		return res;
	}
	else
	{
		powRecurse(n, res * n, pow-1);
	}
}