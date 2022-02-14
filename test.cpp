#include <iostream>
using namespace std;

int main(){
    double** n1 = new double*[256];

    for(int i=0;i<256;i++)n1[i] = new double(20.0+double(i));
    
   cout << *n1+1 << endl;
    cout << &(**n1) << endl;
    cout << (n1[2]) << endl;
    cout << *(n1[2]) << endl;
    
    return 0;
}