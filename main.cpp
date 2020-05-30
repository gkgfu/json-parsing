#include <iostream>
#include <fstream>
#include <sstream>
#include "json-parsing.hpp"
using namespace std;
int main()
{
    json_object obj(R"({
    "sites": [
    { "name":"gkg\"fu" , "url":"I don't have this yet" }, 
    { "name":"google" , "url":"www.google.com" }, 
    { "name":"微博" , "url":"www.weibo.com" },
    { "name":"百度" , "url":"www.baidu.com"}
    ]
}
    )");
    json_array array = obj.getProperties().at(0).getValueObject();
    json_object fuck = (array.getArray().at(0));
    cout<<(string)(fuck.getProperties().at(0).getValueObject())<<endl;
    return 0;
}
