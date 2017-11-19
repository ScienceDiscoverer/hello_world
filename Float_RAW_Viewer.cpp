#include <iostream>
#include <bitset>
#include <cmath>

using namespace std;

int main()
{
    //float x = 9.234f;
    while(1)
    {
		float x;
		cin >> x;
		float* p = &x;
	    int* p_i = (int*)p;
		bitset<32> float_int(*p_i);
	    
	    cout << "Raw float " << x << ": " << float_int << endl;
	    cout << "Sign: " << float_int[31] << (float_int[31] ? " = -" : " = +") << endl;
	    cout << "Exponent: ";
	    
	    bitset<8> exp;
		for(int i = 30; i > 22; --i)
		{
			cout << float_int[i];
			exp[i-23] = float_int[i];
		}
		
		int expon = (int)exp.to_ulong() - 127;
		cout << " = 2 ^ " << expon << " (" << exp.to_ulong() << " - bias[127])" << endl;
		cout << "Mantissa: ";
		
		bitset<23> mantissa;
		for(int i = 22; i >= 0; --i)
		{
			cout << float_int[i];
			mantissa[i] = float_int[i];
		}
		
		float mant = 1.0f + (float)mantissa.to_ulong() / pow(2, 23);
		cout << " = " << mant << endl;
		
		cout << "Proof: " << mant << " * " << pow(2.0f, (float)expon) << " = ";
		if(float_int[31]) cout << '-';
		cout << mant * pow(2.0f, (float)expon) << endl;
		
		cout << "<--------------------------------------------------->" << endl;
	}
    
	return 0;    
}
