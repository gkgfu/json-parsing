#include<iostream>
#include"json-parsing.hpp"
using namespace std;
int main(){
    json_array arr(R"([
    { "name":"gkgfu" , "url":"I don't have this yet" }, 
    { "name":"google" , "url":"www.google.com" }, 
    { "name":"微博" , "url":"www.weibo.com" },
    { "name":"百度" , "url":"www.baidu.com"}
    ])");
    vector<json_object> obj = arr.getArray();
    
    return 0;
}
