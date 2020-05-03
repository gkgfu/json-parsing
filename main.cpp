#include<iostream>
#include"json-parsing.hpp"
using namespace std;
int main(){
    // json_array arr(R"([{"name" : "gkgfu"},{"age" : "unknow"},{"[}" : "{]"}])");
    json_array arr("[{\"1\\\"23\":\"456\"},{\"gkg\" : \"fu\"}]");
    vector<json_object> obj = arr.getArray();
    json_object o = obj.at(0);
    cout<<o.getKey()<<(string)o.getValue()<<endl;
    return 0;
}
