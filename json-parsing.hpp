#include<string>
#include<vector>
#include<stack>
#include<iostream>
enum value_type{//json值的不同类型
    STRING,
    INT,
    FLOAT,
    BOOLEAN,
    ARRAY,
    OBJECT,
    NULLTYPE,
    UNKNOW
};
// class json_parsing{//解析器主类
// public:
//     json_parsing(std::string json_text){
        
//     }
// };
class formatError{//格式错误异常
public:
    formatError(){
        std::cerr<<"json文件格式错误"<<std::endl;
    }
};
class json_object{//json对象
private:
    std::string key;
    std::string value;
    value_type type;
public:
    json_object(std::string json_text){
        this->type = UNKNOW;
        std::stack<char> pairBracket;
        for(char ch:json_text){//判断括号是否成对
            if(ch=='{'||ch=='['||ch=='('){
                pairBracket.push(ch);
            }
            else if(ch=='}'||ch==']'||ch==')'){
                pairBracket.pop();
            }
        }
        if(!pairBracket.empty()){//如果括号不成对，抛出格式错误异常
            throw formatError();
        }
        bool inString = false;//判断当前读到的字符是否在字符串内
        bool isKey = true;//如果是在字符串内，判断读到的是否是key值
        for(char ch:json_text){
            if(ch=='"'&&!inString){//如果读到引号并不在字符串里，开启字符串读取模式
                inString = true;
            }
            else if(ch=='"'&&inString){//如果读到引号并在字符串里，关闭读字符串模式
                inString = false;
            }
            else if(inString&&isKey){//如果在读字符串并在读key,将当前字符加入key的字符串
                this->key.push_back(ch);
            }
            else  if(ch==':'&&!inString){//如果读到冒号并不在字符串内，将isKey设为false
                isKey = false;
            }
            else if(!isKey){
                if(!inString){//如果不是key并不在字符串中
                    if(ch>='0'&&ch<='9')
                        this->type = INT;
                    else if(ch=='.'){
                        this->type = FLOAT;
                        break;
                    }
                    else if(ch=='['){
                        this->type = ARRAY;
                        break;
                    }
                    else if(ch=='t'){
                        this->value = "true";
                        this->type = BOOLEAN;
                        break;
                    }
                    else if(ch=='f'){
                        this->value = "false";
                        this->type = BOOLEAN;
                        break;
                    }
                    else if(ch=='n'){
                        this->type = NULLTYPE;
                        this->value = "null";
                        break;
                    }
                    else if(ch=='{'){
                        this->type = OBJECT;
                        break;
                    }
                }
                else{
                    this->type = STRING;
                    break;
                }
            }
        }
        bool inValue = false;
        inString = false;
        for(char ch:json_text){
            if(ch == ':'){
                inValue = true;
            }
            else if(ch=='"'&&!inString){
                inString = true;
            }
            else if(ch=='"'&&inString){
                inString = false;
            }
            if(inValue){
                switch(this->type){
                    case OBJECT:
                    case ARRAY:
                    case INT:
                    case FLOAT:
                        this->value.push_back(ch);
                        break;
                    case NULLTYPE:
                    case UNKNOW:
                        break;
                    case STRING:
                        if(inString){
                            this->value.push_back(ch);
                        }
                }
            }
        }
    }
};
class json_array{//json数组
private:
    std::vector<json_object> array;
};