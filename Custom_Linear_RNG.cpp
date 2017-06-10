#include <iostream>
#include <chrono>

using namespace std;
typedef unsigned long long ull;
const ull mod_rng = 0x10000000000; //2^40 modulus for RNG engine!

ull getSeed()
{
	chrono::system_clock::time_point cur_time = chrono::system_clock::now();
    chrono::system_clock::duration seed = cur_time.time_since_epoch();
    return seed.count()%100000000000000/100; //max seed 1*10^12 - 1
}

//Linear Congruential Pseudo Random Number Generator
ull rng(int seed) //seed must be 0 <= seed < mod!
{   //multiplyer also must be multi%8 = 5 !!! For best uniformal distribution!
	const ull multi = 0x4C8F53A795; //multiplier 2.|multi - 1 must be % = 0 for all mod's prime factors|
	const ull inc = 1; //increment 1.|inc and mod must be co-prime|
	const ull mod = mod_rng; //2^40 modulus 3.|multi must be %4 = 0 if mod%4 = 0|
	
	return (multi * seed + inc) % mod;
}

const ull seed = getSeed(); //get seed
ull rnd_num = seed;

//uniform random number distribution of range
int rngRange(int min, int max) //min,max must be non negative!
{
	const ull rngrg = mod_rng;//RNG range
	int range = max - min + 1;
	ull scale = rngrg/range; //big range part to 1 part of range
	rnd_num = rng(rnd_num);
	return rnd_num/scale + min; //downscale
}

int main()
{
	
	//RNG TESTS
	//for(int i = 0; i < 100; ++i)
	//	cout << rngRange(1, 3) << ' ';
	/*
	for(int i = 1; i <= 102*2; ++i)
	{
		int num = rngRange(1, 3);
		for(int j = 0; j < num; ++j)
			cout << '*';
		if(i%6 == 0)
			cout << endl;
		else
			cout << "\t";
	}
	*/
	int one, two, three;
	one = two = three = 0;
	for(int i = 0; i < 1008*2; ++i)
	{
		int rnd_num = rngRange(1, 3);
		switch(rnd_num)
		{
			case 1: one++;
			break;
			case 2: two++;
			break;
			case 3: three++;
		}
	}
	cout << "     " << one << '\t' << two << '\t' << three << endl;
	for(int i = 0; i < 350*2; ++i)
	{
		if(i < 9) cout << "  " << i+1;
		else if(i < 99) cout << ' ' << i+1;
		else cout << i+1;
		cout << ":  ";
		if(one > 0) cout << '*'; else cout << ' ';
		one--;
		cout << " \t ";
		if(two > 0) cout << '*'; else cout << ' ';
		two--;
		cout << " \t ";
		if(three > 0) cout << '*'; else cout << ' ';
		three--;
		cout << endl;
	}
    
    return 0;
}
