#include <string>
#include <vector>
#include <stack>
#include <iostream>
enum value_type
{ //json值的不同类型
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
class json_property;
class json_array;
class json_object; //对这三个类的声明

class TYPE_ERROR
{
public:
    TYPE_ERROR(value_type need, value_type fact)
    {
        std::cerr << "value error!" << std::endl
                  << "need type : " << need << std::endl
                  << "fact type : "
                  << fact << std::endl;
    }
};

class value_object
{ //将value整合成一个类而不是字符串
private:
    std::string value;
    value_type type;

public:
    friend json_property;
    void push_back(char &ch)
    { //为了兼容之前把value当作string的操作，提供一些接口
        this->value.push_back(ch);
    }
    void operator=(const char *ch)
    {
        this->value = ch;
    }

    //类型转换操作符重载
    operator int()
    {
        if (this->type == INT)
            return atoi(value.c_str());
        else
        {
            throw TYPE_ERROR(INT, this->type);
        }
    }
    operator double()
    {
        if (this->type == FLOAT)
            return atof(value.c_str());
        else
        {
            throw TYPE_ERROR(FLOAT, this->type);
        }
    }
    operator std::string()
    {
        if (this->type == STRING)
            return this->value;
        else
        {
            throw TYPE_ERROR(STRING, this->type);
        }
    }
    operator bool()
    {
        if (this->type == BOOLEAN)
            if (value == "true")
            {
                return true;
            }
            else if (value == "false")
            {
                return false;
            }
            else
            {
                return false;
            }
        else
        {
            throw TYPE_ERROR(BOOLEAN, this->type);
        }
    }
    operator json_property();
    operator json_array();
    operator json_object(); //由于这三个类的声明在下面，因此只能先将这两个函数声明
};
class formatError
{ //格式错误异常
public:
    formatError()
    {
        std::cerr << "json文件格式错误" << std::endl;
    }
};

class json_property
{ //json属性
private:
    std::string key;
    value_object value;

public:
    json_property(std::string json_text)
    {
        this->value.type = UNKNOW;
        std::stack<char> pairBracket;
        for (char ch : json_text)
        { //判断括号是否成对
            if (ch == '{' || ch == '[' || ch == '(')
            {
                pairBracket.push(ch);
            }
            else if (ch == '}' || ch == ']' || ch == ')')
            {
                pairBracket.pop();
            }
        }
        if (!pairBracket.empty())
        { //如果括号不成对，抛出格式错误异常
            throw formatError();
        }
        bool inString = false; //判断当前读到的字符是否在字符串内
        bool isKey = true;     //如果是在字符串内，判断读到的是否是key值
        for (std::string::iterator ch = json_text.begin(); ch != json_text.end(); ch++)
        {
            if (*ch == '"' && !inString)
            { //如果读到引号并不在字符串里，开启字符串读取模式
                inString = true;
            }
            else if (*ch == '"' && inString && *(ch - 1) != '\\')
            { //如果读到引号并在字符串里，关闭读字符串模式
                inString = false;
            }
            else if (inString && isKey)
            { //如果在读字符串并在读key,将当前字符加入key的字符串
                if (*ch == '\\' && *(ch + 1) == '"')
                    continue;
                this->key.push_back(*ch);
            }
            else if (*ch == ':' && !inString)
            { //如果读到冒号并不在字符串内，将isKey设为false
                isKey = false;
            }
            else if (!isKey)
            {
                if (!inString)
                { //如果不是key并不在字符串中
                    if (*ch >= '0' && *ch <= '9')
                        this->value.type = INT;
                    else if (*ch == '.')
                    {
                        this->value.type = FLOAT;
                        break;
                    }
                    else if (*ch == '[')
                    {
                        this->value.type = ARRAY;
                        break;
                    }
                    else if (*ch == 't')
                    {
                        this->value = "true";
                        this->value.type = BOOLEAN;
                        break;
                    }
                    else if (*ch == 'f')
                    {
                        this->value = "false";
                        this->value.type = BOOLEAN;
                        break;
                    }
                    else if (*ch == 'n')
                    {
                        this->value.type = NULLTYPE;
                        this->value = "null";
                        break;
                    }
                    else if (*ch == '{')
                    {
                        this->value.type = OBJECT;
                        break;
                    }
                }
                else
                {
                    this->value.type = STRING;
                    break;
                }
            }
        }
        bool inValue = false;
        inString = false;
        bool atLast = false;
        bool atFirst = true;
        for (std::string::iterator ch = json_text.begin(); ch != json_text.end(); ch++)
        {
            if (*ch == ':' && !inValue && !inString)
            {
                inValue = true;
                continue;
            }
            else if (*ch == '"' && !inString)
            {
                inString = true;
            }
            else if (*ch == '"' && inString && *(ch - 1) != '\\')
            { //判断是否进入字符串模式
                inString = false;
            }
            if (inValue)
            {
                if (*ch == '{' || *ch == '[')
                { //这个是为了判断是否在一个对象或数组中
                    pairBracket.push(*ch);
                }
                // else if(*ch=='}'){
                //     std::cout<<"让我康康"<<std::endl;
                // }
                else if (!pairBracket.empty() && ((*ch == '}' && '{' == pairBracket.top()) || (*ch == ']' && '[' == pairBracket.top())))
                {
                    pairBracket.pop();
                }
                switch (this->value.type)
                { //对不同的类型进行不同的操作
                case OBJECT:
                case ARRAY:
                    if (!pairBracket.empty())
                    {
                        this->value.push_back(*ch);
                        atLast = true;
                    }
                    else if (pairBracket.empty() && atLast)
                    {
                        this->value.push_back(*ch);
                        atLast = false;
                    }
                    break;
                case INT:
                case FLOAT:
                    this->value.push_back(*ch);
                    break;
                case NULLTYPE:
                case UNKNOW:
                case BOOLEAN:
                    break;
                case STRING:
                    if (inString)
                    {
                        if (atFirst)
                        {
                            atFirst = false;
                            continue;
                        }
                        this->value.push_back(*ch);
                        atLast = true;
                    }
                }
            }
        }
    }
    value_object getValueObject()
    {
        return this->value;
    }
    value_type getType()
    {
        return this->value.type;
    }
    std::string getKey()
    {
        return this->key;
    }
};

//json对象
class json_object
{
private:
    std::vector<json_property> object;

public:
    json_object(std::string object_text)
    {
        std::stack<char> pairBracket;
        for (char ch : object_text)
        { //判断括号是否成对
            if (ch == '{' || ch == '[' || ch == '(')
            {
                pairBracket.push(ch);
            }
            else if (ch == '}' || ch == ']' || ch == ')')
            {
                pairBracket.pop();
            }
        }
        if (!pairBracket.empty())
        { //如果括号不成对，抛出格式错误异常
            throw formatError();
        }

        std::string::iterator begin = object_text.begin(), end = object_text.end(); //开头及结尾的{}
        std::vector<std::string::iterator> split;                                   //分隔，即中间的逗号
        // std::stack<char> inside;
        char theLast = '}';
        bool inString = false;
        bool foundLeft = false;
        // bool foundRight = false;
        for (std::string::iterator ch = object_text.begin(); ch != object_text.end(); ch++)
        {
            if (*ch == '{' && !inString && !foundLeft)
            { //找到开头的{
                foundLeft = true;
                begin = ch;
            }
            else if (*ch == '}' && !inString && pairBracket.empty())
            { //找到结尾的}
                end = ch;
            }
            else if (((*ch) == '{' || (*ch) == '[') && !inString && *(ch - 1) != '\\')
            {
                pairBracket.push(*ch);
                // theLast = *ch;
            }
            else if ((*ch == '}' || *ch == ']') && *(ch - 1) != '\\' && !pairBracket.empty())
            {
                if (pairBracket.top() == ((*ch == '}') ? '{' : '['))
                    pairBracket.pop();
                else
                    throw formatError();
            }
            else if (*ch == '"' && !inString)
            {
                inString = true;
            }
            else if (*ch == '"' && inString && *(ch - 1) != '\\')
            {
                inString = false;
            }
            else if (*ch == ',' && !inString && pairBracket.empty())
            {
                split.push_back(ch);
            }
        }
        // for(auto s:split){
        if (split.empty())
        {
            this->object.push_back(json_property(std::string(begin + 1, end)));
        }
        for (std::vector<std::string::iterator>::iterator s = split.begin(); s != split.end(); s++)
        { //分隔，然后丢到vector容器里
            if (s == split.begin())
            { //如果s指向了第一个逗号
                this->object.push_back(json_property(std::string(begin + 1, *s)));
            }
            if (s == split.end() - 1)
            { //如果s指向了最后一个逗号
                this->object.push_back(json_property(std::string(*s, end)));
                break;
            }
            this->object.push_back(json_property(std::string(*s, *(s + 1))));
        }
    }
    std::vector<json_property> getProperties()
    {
        return this->object;
    }
};

class json_array
{
private:
    std::vector<value_object> array;

public:
    json_array(std::string array_text)
    {
        std::stack<char> pairBracket;
        for (char ch : array_text)
        { //判断括号是否成对
            if (ch == '{' || ch == '[' || ch == '(')
            {
                pairBracket.push(ch);
            }
            else if (ch == '}' || ch == ']' || ch == ')')
            {
                pairBracket.pop();
            }
        }
        if (!pairBracket.empty())
        { //如果括号不成对，抛出格式错误异常
            throw formatError();
        }
        std::string::iterator begin = array_text.begin(), end = array_text.end();
        bool inString = false;
        char theLast = '\000';
        std::vector<std::string::iterator> split;
        for (std::string::iterator ch = array_text.begin(); ch != array_text.end(); ch++)
        {
            if (*ch == '[' && begin == array_text.begin() && !inString)
            { //找到开头的[
                begin = ch;
            }
            else if (*ch == ']' && end == array_text.end() && !inString)
            { //找到结尾的]
                end = ch;
            }
            else if (*ch == '"' && !inString)
            {
                inString = true;
            }
            else if (*ch == '"' && inString && *(ch - 1) != '\\')
            {
                inString = false;
            }
            else if (((*ch) == '{' || (*ch) == '[' || (*ch) == '}' || (*ch) == ']') && !inString)
            {
                theLast = *ch;
            }
            else if (*ch == ',' && !inString && (theLast == ']' || theLast == '}' || theLast == '\000'))
            {
                split.push_back(ch);
            }
        }
        std::string values;

        for (std::vector<std::string::iterator>::iterator s = split.begin(); s != split.end(); s++)
        { //分隔，然后丢到vector容器里
            if (s == split.begin())
            { //如果s指向了第一个逗号
                values.assign(":");
                values.append(begin + 1, *s);
                this->array.push_back(json_property(values).getValueObject());
            }
            else if (s == split.end() - 1)
            { //如果s指向了最后一个逗号
                values.assign(":");
                values.append(*s + 1, end);
                this->array.push_back(json_property(values).getValueObject());
                break;
            }
            values.assign(":");
            values.append(*s + 1, *(s + 1));
            this->array.push_back(json_property(values).getValueObject());
        }
    }
    std::vector<value_object> getArray()
    {
        return this->array;
    }
};

value_object::operator json_property()
{

    return json_property(this->value);
}

value_object::operator json_object()
{
    if (this->type == OBJECT)
        return json_object(this->value);
    else
    {
        throw TYPE_ERROR(OBJECT, this->type);
    }
}

value_object::operator json_array()
{
    if (this->type == ARRAY)
        return json_array(this->value);
    else
    {
        throw TYPE_ERROR(ARRAY, this->type);
    }
}