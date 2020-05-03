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
class json_object;
class json_array;//对这两个类的声明
class value_object{//将value整合成一个类而不是字符串
private:
    std::string value;
public:
    void push_back(char &ch){//为了兼容之前把value当作string的操作，提供一些接口
        this->value.push_back(ch);
    }
    void operator=(const char* ch){
        this->value = ch;
    }

    //类型转换操作符重载
    operator int(){
        return atoi(value.c_str());
    }
    operator double(){
        return atof(value.c_str());
    }
    operator std::string(){
        return this->value;
    }
    operator bool(){
        if(value=="true"){
            return true;
        }
        else if(value=="false"){
            return false;
        }
        else{
            return false;
        }
    }
    operator json_object();
    operator json_array();//由于这两个类的声明在下面，因此只能先将这两个函数声明
};
class formatError{//格式错误异常
public:
    formatError(){
        std::cerr<<"json文件格式错误"<<std::endl;
    }
};
class json_object{//json对象
private:
    std::string key;
    value_object value;
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
        bool atLast = false;
        bool atFirst = true;
        for(char ch:json_text){
            if(ch == ':'&&!inValue){
                inValue = true;
                continue;
            }
            else if(ch=='"'&&!inString){
                inString = true;
            }
            else if(ch=='"'&&inString){//判断是否进入字符串模式
                inString = false;
            }
            if(inValue){
                if(ch=='{'||ch=='['){//这个是为了判断是否在一个对象或数组中
                    pairBracket.push(ch);
                }
                // else if(ch=='}'){
                //     std::cout<<"让我康康"<<std::endl;
                // }
                else if(!pairBracket.empty()&&((ch=='}'&&'{'==pairBracket.top())||(ch==']'&&'['==pairBracket.top()))){
                    pairBracket.pop();
                }
                switch(this->type){//对不同的类型进行不同的操作
                    case OBJECT:
                    case ARRAY:
                        if(!pairBracket.empty()){
                            this->value.push_back(ch);
                            atLast = true;
                        }
                        else if(pairBracket.empty()&&atLast){
                            this->value.push_back(ch);
                            atLast = false;
                        }
                        break;
                    case INT:
                    case FLOAT:
                        this->value.push_back(ch);
                        break;
                    case NULLTYPE:
                    case UNKNOW:
                    case BOOLEAN:
                        break;
                    case STRING:
                        if(inString){
                            if(atFirst){
                                atFirst = false;
                                continue;
                            }
                            this->value.push_back(ch);
                            atLast = true;
                        }
                }
            }
        }
    }
    value_object getValue(){
        return this->value;
    }
    value_type getType(){
        return this->type;
    }
};

//json数组
class json_array{
    private:
    std::vector<json_object> array;
    public:
    json_array(std::string array_text){
        std::stack<char> pairBracket;
        std::string::iterator begin = array_text.begin(),end = array_text.end();//开头及结尾的[]
        std::vector<std::string::iterator>split;//分隔，即中间的逗号
        // std::stack<char> inside;
        char theLast;
        bool inString = false;
        for(std::string::iterator ch = array_text.begin();ch!=array_text.end();ch++){
            if(*ch=='['&&begin==array_text.begin()){//找到开头的[
                begin = ch;
            }
            else if(*ch==']'&&end==array_text.end()){//找到结尾的]
                end = ch;
            }
            // else if((*ch=='['&&ch!=begin)||*ch=='{'||(*ch=='"'&&inside==false)){
            //     inside = true;
            // }
            // else if(inside&&(*ch==']'||*ch=='{'||*ch=='"')){
            //     inside = false;
            // }
            // else if(*ch==','&&!inside){
            //     split.push_back(ch);
            // }
            else if(*ch='{'||*ch=='['||*ch=='}'||*ch==']'){
                // inside.push(*ch);
                theLast = *ch;
            }
            else if(*ch=='"'&&!inString){
                inString = true;
            }
            else if(*ch=='"'&&inString){
                inString = false;
            }
            else if(*ch==','&&(theLast=='}'||theLast==']')&&!inString){
                split.push_back(ch);
            }
        }
        // for(auto s:split){
        for(std::vector<std::string::iterator>::iterator s = split.begin();s!=split.end();s++){//分隔，然后丢到vector容器里
            if(*s==split.at(0)){
                this->array.push_back(json_object(std::string((begin+1),(*s+1))));
            }
            else if(*s==*(split.end()-1)){
                this->array.push_back(json_object(std::string((*s+1),end)));
            }
            else{
                this->array.push_back(json_object(std::string((*s+1),(*(s+1)+1))));
            }
        }
    }
    std::vector<json_object> getArray(){
        return this->array;
    }
};
value_object::operator json_object(){
    return json_object(this->value);
}
