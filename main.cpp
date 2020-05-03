#include<iostream>
#include<fstream>
#include<sstream>
#include"json-parsing.hpp"
using namespace std;
int main(){
    json_array arr(R"([2,3,4])");
    vector<value_object> val_obj = arr.getArray();
    value_object val = val_obj.at(1);
    int a = (int)val;
    cout<<a<<endl;
    return 0;
}
