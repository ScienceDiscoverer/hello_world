#include <iostream>
//#include <random>
#include <chrono>
#include <thread>
#include <conio.h>

using namespace std;

int main()
{

    int a(0);
    while(a++ < 100)
    {
    	cout << '\r' << "Loading: " << a << '%';
    	this_thread::sleep_for(chrono::milliseconds(120));
	}
	cout << "    Press any button to exit";
    cout << endl << endl;
    cout << "Time since 00:00:00 1 Jan 1970 UTC" << endl << endl;
	while(!_kbhit())
	{
		chrono::system_clock::time_point cur_time = chrono::system_clock::now();
    	chrono::system_clock::duration dur = cur_time.time_since_epoch();
    	chrono::hours hrs = chrono::duration_cast<chrono::hours>(dur);
    	chrono::minutes min = chrono::duration_cast<chrono::minutes>(dur - hrs);
    	chrono::seconds sec = chrono::duration_cast<chrono::seconds>(dur - hrs - min);
    	chrono::milliseconds milisec = chrono::duration_cast<chrono::milliseconds>(dur - hrs - min - sec);
    	chrono::microseconds microsec = chrono::duration_cast<chrono::microseconds>(dur - hrs - min - sec - milisec);
    	chrono::nanoseconds nan = dur - hrs - min - sec - milisec - microsec;
    	int days = hrs.count()/24;
    	int years = days/365;
		cout << '\r' << "Y: " << years << " D: " << days-years*365 << " H: " << hrs.count() - days*24;
		cout << " M: "; if(min.count()<10) cout << 0;
		cout << min.count() << " S: "; if(sec.count()<10) cout << 0;
		cout << sec.count() << " MS: "; if(milisec.count()<10) cout << "00"; else if(milisec.count()<100) cout << 0;
		cout << milisec.count();
		cout << " MICS: "; if(microsec.count()<10) cout << "00"; else if(microsec.count()<100) cout << 0;
		cout << microsec.count() << " NS: " << nan.count();
	}
    
    return 0;
}
