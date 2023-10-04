// CONSOLE
#include <stdp>

#include <chrono>

// Default MT19937 Mersenne Twister

class SciMersenne
{
public:
	// Constructors
	SciMersenne();
	SciMersenne(uint32_t seed);
	// Destructor
	~SciMersenne();
	
	// Interface functions
	uint32_t Random();
	uint32_t Range(uint32_t min, uint32_t max);
	
private:
	// Private functions
	void Init(uint32_t seed);
	// Data
	uint32_t* stateTable;
	uint32_t curEntry; // Current Entry of table
	uint32_t reserved;
};

// Default MT19937 Mersenne Twister values
const uint32_t cN = 624; 		// Table array size
const uint32_t cM = 397; 		// Lag for blending
const uint32_t cR = 31; 		// Masking bits
const uint32_t cA = 0x9908B0DF; // Twist value
const uint32_t cU = 11;			// Tempering parameters
const uint32_t cS = 7;
const uint32_t cT = 15;
const uint32_t cL = 18;
const uint32_t cB = 0x9D2C5680;
const uint32_t cC = 0xEFC60000;

const uint32_t lowerMask = ((uint32_t)(0x00000001) << cR) - 1; // Lower R bits
const uint32_t upperMask = 0xFFFFFFFF << cR; // Upper (32 - R) bits
const uint32_t twistMask = 0x00000001;
const uint32_t mag01[2] = {0, cA}; // Twist fast ternary operation array

// Constructors
SciMersenne::SciMersenne()
{
	std::chrono::system_clock::time_point cur_time = std::chrono::system_clock::now();
	std::chrono::system_clock::duration seed = cur_time.time_since_epoch();
	Init((uint32_t)seed.count()); // Take seed automatically from system time
}

SciMersenne::SciMersenne(uint32_t seed)
{
	Init(seed);
}
// End of Constructors

// Destructor
SciMersenne::~SciMersenne()
{
	delete[] stateTable;
}
// End of Destructor

// Interface functions
uint32_t SciMersenne::Random()
{
	// Regenerate state table if all values was used
	if(curEntry >= cN)
	{
		// First N-M values
		for(uint32_t k = 0; k < cN-cM; ++k)
		{
			uint32_t tmp = ((stateTable[k]&upperMask) | (stateTable[k+1]&lowerMask));
			// new state value: (value from M away) XOR (new blended value shifted) XOR (twist)
			stateTable[k] = stateTable[k+cM] ^ (tmp >> 1) ^ mag01[tmp & twistMask];
		}
		// Next, all but the last one
		for(uint32_t k = cN-cM; k < cN-1; ++k)
		{
			uint32_t tmp = ((stateTable[k]&upperMask) | (stateTable[k+1]&lowerMask));
			stateTable[k] = stateTable[k + (cM-cN)] ^ (tmp >> 1) ^ mag01[tmp & twistMask];
		}
		// Last value
		uint32_t tmp = ((stateTable[cN-1]&upperMask) | (stateTable[0]&lowerMask));
		stateTable[cN-1] = stateTable[cM-1] ^ (tmp >> 1) ^ mag01[tmp & twistMask];
		curEntry = 0;
	}
	
	// Get next value and increment counter
	uint32_t y = stateTable[curEntry++];
	
	// Temper it in fire of Fierce Bit Shifts!
	y ^= y >> cU;
	y ^= (y << cS) & cB;
	y ^= (y << cT) & cC;
	y ^= y >> cL;
	
	return y;
}

uint32_t SciMersenne::Range(uint32_t min, uint32_t max)
{
	if(min > max)
	{
		return 0;
	}
	
	++max; // Increment max, to include it in range
	uint32_t y;
	uint32_t max_r = 0xFFFFFFFF - 0xFFFFFFFF % (max - min);
	
	do
	{
		y = Random();
	} while (y >= max_r);
	
	return y % (max - min) + min;
}
// End of Interface functions

// Private functions
void SciMersenne::Init(uint32_t seed)
{
	stateTable = new uint32_t[cN];
	// Fill state table with initial random values
	stateTable[0] = seed;
	for(uint32_t i = 1; i < cN; ++i)
	{
		stateTable[i] = (1812433253UL * (stateTable[i-1] ^ (stateTable[i-1] >> 30)) + i);
	}
	curEntry = cN;
}
// End of Private functions

#define GAMES 1200000

void noHost()
{
	SciMersenne rng;
	
	ui64 a = 0, b = 0, c = 0;
	
	ui64 w = 0;
	
	for(ui64 i = 0; i < GAMES; ++i)
	{
		ui64 car = rng.Range(0, 2);
		ui64 choice = rng.Range(0, 2);
		
		if(choice == car)
		{
			++w;
		}
		
		if(choice == 0)
		{
			++a;
		}
		else if(choice == 1)
		{
			++b;
		}
		else if(choice == 2)
		{
			++c;
		}
	}
	
	p|a|N|b|N|c|N;
	p|G|w|" WINS! "|V|"CONGRATULATIONS.MP4"|N;
	float quo = (float)w/(float)GAMES;
	txt quo_t = i2t((ui64)(quo * 10000.0f));
	txti(quo_t, 2, '.');
	p|quo_t|"% chance"|N;
}

void hostSameChoice()
{
	SciMersenne rng;
	
	ui64 a = 0, b = 0, c = 0;
	
	ui64 w = 0;
	
	for(ui64 i = 0; i < GAMES; ++i)
	{
		ui64 car = rng.Range(0, 2);
		ui64 choice = rng.Range(0, 2);
		
		ui64 goat0, goat1;
		if(car == 0)
		{
			goat0 = 1, goat1 = 2;
		}
		else if(car == 1)
		{
			goat0 = 0, goat1 = 2;
		}
		else // car == 2
		{
			goat0 = 0, goat1 = 1;
		}
		
		ui64 goat_rev = rng.Range(0, 1);
		ui64 goat_sneak;
		if(goat_rev == 0)
		{
			goat_rev = goat0, goat_sneak = goat1;
		}
		else
		{
			goat_rev = goat1, goat_sneak = goat0;
		}
		
		// This would be extremely retarded to NOT change a choice when your initial is a GOAT
		if(choice == goat_rev)
		{
			choice = rng.Range(0, 1);
			choice = choice == 0 ? car : goat_sneak;
		}
		
		// If intial choice was not revealed as a goat, stick with original choice
		
		if(choice == car)
		{
			++w;
		}
		
		if(choice == 0)
		{
			++a;
		}
		else if(choice == 1)
		{
			++b;
		}
		else if(choice == 2)
		{
			++c;
		}
	}
	
	p|a|N|b|N|c|N;
	p|G|w|" WINS! "|V|"CONGRATULATIONS.MP4"|N;
	float quo = (float)w/(float)GAMES;
	txt quo_t = i2t((ui64)(quo * 10000.0f));
	txti(quo_t, 2, '.');
	p|quo_t|"% chance"|N;
}

void hostChangeChoice()
{
	SciMersenne rng;
	
	ui64 a = 0, b = 0, c = 0;
	
	ui64 w = 0;
	
	for(ui64 i = 0; i < GAMES; ++i)
	{
		ui64 car = rng.Range(0, 2);
		ui64 choice = rng.Range(0, 2);
		
		ui64 goat0, goat1;
		if(car == 0)
		{
			goat0 = 1, goat1 = 2;
		}
		else if(car == 1)
		{
			goat0 = 0, goat1 = 2;
		}
		else // car == 2
		{
			goat0 = 0, goat1 = 1;
		}
		
		ui64 goat_rev = rng.Range(0, 1);
		ui64 goat_sneak;
		if(goat_rev == 0)
		{
			goat_rev = goat0, goat_sneak = goat1;
		}
		else
		{
			goat_rev = goat1, goat_sneak = goat0;
		}
		
		// This would be extremely retarded to NOT change a choice when your initial is a GOAT
		if(choice == goat_rev)
		{
			choice = rng.Range(0, 1);
			choice = choice == 0 ? car : goat_sneak;
		}
		else // Change choice even if the goat was revealed in another door
		{
			choice = choice == car ? goat_sneak : car;
		}
		
		if(choice == car)
		{
			++w;
		}
		
		if(choice == 0)
		{
			++a;
		}
		else if(choice == 1)
		{
			++b;
		}
		else if(choice == 2)
		{
			++c;
		}
	}
	
	p|a|N|b|N|c|N;
	p|G|w|" WINS! "|V|"CONGRATULATIONS.MP4"|N;
	float quo = (float)w/(float)GAMES;
	txt quo_t = i2t((ui64)(quo * 10000.0f));
	txti(quo_t, 2, '.');
	p|quo_t|"% chance"|N;
}

i32 wmain()
{
	p|DC|"NUMBER OF GAMES: "|B|GAMES|N;
	
	while(1)
	{
		p|"NO HOST:"|N;
		noHost();
		p|YD|"----------------------------------------"|N;
		p|"WITH HOST "|G|"SAME CHOICE:"|N;
		hostSameChoice();
		p|YD|"----------------------------------------"|N;
		p|"WITH HOST "|R|"CHANGE CHOICE:"|N;
		hostChangeChoice();
		p|YD|"----------------------------------------"|N;
		
		p|G|"DONE!!!"|N|P;
	}
	
	return 0;
}