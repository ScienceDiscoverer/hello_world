#include <iostream>
#include <bitset>
#include <cmath>
// Sorry for the ugly code :D

using namespace std;

int main()
{
    //float x = 9.234f;
    //setting float to infinity
	bitset<32> float_inf;
	float_inf[31] = 1;
	for(int i = 30; i > 22; --i)
		float_inf[i] = 1;
		
	for(int i = 22; i >= 0; --i)
		float_inf[i] = 0;
		
	cout << "Inf float raw: " << float_inf << endl;
	unsigned long ul = float_inf.to_ulong();
	cout << "Inf as int: " << ul << endl;
	unsigned long* ulp = &ul;
	float* fp = (float*)ulp;
	
	cout << "Inf float: " << *fp << endl;
	cout << "<--------------------------------------------------->" << endl;
	//////////////////////////////////////
	
	
	while(1)
    {
		cout << "Enter 2 float numbers to multiply: ";
		float x, y;
		cin >> x >> y;
		x *= y;
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
		
		float mant = (expon == -127 || expon == 128? 0.0f:1.0f) + (float)mantissa.to_ulong() / pow(2, 23);
		cout << " = " << mant;
		if(expon == -127 && !mant) cout << " (-127 exp. & 0 mant. = 0)";
		else if(expon == -127) cout << " (-127 exp. & non-0 mant. = denormalised number)";
		else if(expon == 128 && !mant) cout << " (128 exp. & 0 mant. = infinity)";
		cout << endl;
		
		cout << "Proof: ";
		if(expon == 128 && !mant)
		{
			if(float_int[31]) cout << '-';
			cout << 1.0e+30f * 1.0e+38f << endl;
		}
		else
		{
			cout << mant << " * " << pow(2.0f, (float)expon) << " = ";
			if(float_int[31]) cout << '-';
			cout << mant * pow(2.0f, (float)expon) << endl;
		}
		
		cout << "<--------------------------------------------------->" << endl;
	}
    
	return 0;    
}
