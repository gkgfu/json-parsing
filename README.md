# JSON PARSING

## 项目背景

我近来想要写一个天气预报的软件，想了想决定用c++语言开发。可是我申请的数据端口返回的是json文件。我发现似乎c++没有什么好用的json文件解析器，于是决定自己写一个。

## 安装

把_json_parsing.hpp_文件下载到你的项目里直接包含就行了

## 如何使用

### 将字符串转为类

首先，你需要把一个json文件转成std::string,然后使用 _json_object_类来初始化一个json对象。

例：

```c++
json_object obj("{\"name\":\"gkgfu\",\"age\":\"unknow\"}");
```

### 将一个对象分为几个不同的属性

在json中，一个对象有很多个属性。在我的项目里，我使用 _json_property_ 类来存放单个的属性。

例：

```c++
json_property property("\"age\":12");
```

当然，我们不会直接用构造函数来设置一个属性。我们可以使用_json_property_类中的_getProperties()_这一成员函数来获取一个json对象的属性。需要注意的是，它返回的是一个_vector_容器。

例：

```c++
std::vector<json_property> properties = obj.getProperties();
json_property property = properties.at(0);
```

### 属性中的key与value

在json中，一个属性包含一个key与一个value。

对于key值，由于其都是一个字符串，因此你都可以用_json_property_中的_getKey()_这一成员函数来获取。该成员函数的返回值是std::string。

例：

```c++
std::string key = property.getValue();
```

对于value值，因为它可能拥有不同的类型，因此我设计了一个_value_object_类来保存value值。它能够通过类型转换直接转换为c++里拥有的类型与我设计的_json_object_类以及下文将要提到的_json_array_类。它们分别对应了json里的对象与数组。类似的，你可以使用_json_property_中_getValue()_这一成员函数来获取它

例：

```c++
value_object val = property.getValue();
std::cout<<"age : "<<(int)(val.getValue())<<std::endl;//假定property中保存的是"age":12
```

### 关于数组

在我的项目里，为数组设计了一个类_json_array_。你可以使用它的_getArray()_这一成员函数来获取一个类型为_value_object_的_vector_数组。

例：

```c++
json_object obj("{\"score\":[100,99,60]}");
json_property property = obj.getProperties().at(0);
json_array array = property.getValue();//自动类型转换
std::vector<value_object> scores = array.getArray();
int score1 = scores.at(0);
std::cout<<score1<<std::endl;
```

