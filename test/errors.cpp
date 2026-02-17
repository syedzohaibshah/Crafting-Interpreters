#include <stdexcept>
#include<iostream>
using namespace std;
#include<vector>
int main()
try {
     //cout << "Success!\n";
 //cout << "Success!\n;
   //  cout << "Success" << !\n"
    //cout << success << ’\n’;
    // int res = 7;
    // vector<int>v(10);
    // v[5] = res;
    // cout << "5Success!\n";
     // vector<int> v(10);
     // v[5] = 7;
     // if (v[5]==7) cout << "Success!\n";


     // if (cout) cout << "Success!\n";
     // else cout << "Fail!\n";

     // string s = "ape";
     // bool c = string("fool") > string("ape");
     // if (c)
     //     cout << "Success!\n";
     //
     //
     //string s = "fool"; if (s=="fool") cout << "Success!\n";
   //  string s = "ape"; if (s=="fool") cout << "Success!\n";
     // string s = "ape";
     // if (s+string("fool")=="apefool") cout << "Success!\n";
    // vector<char> v(5); for (int i = 0; i<v.size(); ++i) ; cout << "Success!\n";
   //  vector<char> v(5); for (int i = 0; i<=v.size(); ++i) ; cout << "Success!\n";

    // if (true)  cout << "Success!\n"; else cout << "Fail!\n";
     //int x = 2000; int c = x; if (c==2000) cout << "Success!\n";
     //
     //
     // string s = "Success!\n";
     // for (int i = 0; i<10; ++i) cout << s[i];
     //vector<int> v(5); for (int i = 0; i<=v.size(); ++i) ; cout << "Success!\n";
      // int i = 0; int j = 9; while (i<10) i++; ++j;
      // if (j<=i) cout << "Success!\n";
    //  int x = 4;
    //  if(x==2){
    //      throw std::runtime_error("division by 0");
    //  }
    // double d = 5/(x-2);
    //  if (d==2*x+0.5) cout << "Success!\n";
      // string s = "Success!";
      // for (int i = 0; i<=s.size(); ++i) cout<<s[i];
     // int i = 0; while (i<10) ++j; if (j<i) cout << "Success!\n";
      int x = 2; double d = 5/(x-2); if (d==2*x+0.5) cout << "Success!\n";
     // [25] cin << "Success!\n";
return 0;


}catch (exception& e) {
cerr << "catch  e error: " << e.what() << '\n';



return 1;
}catch (...) {

cerr << "Oops: unknown exception!\n";
return 2;

}
