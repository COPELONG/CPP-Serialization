# C++序列化：从设计到实现

## 介绍

序列化是将对象的状态信息转化为可以存储或者传输的形式的过程，在序列化期间，对象将其当前状态写入到临时或者持久性存储区，以后可以通过从存储区中读取或者反序列化对象的状态，重新创建该对象。

## 知识点：

1.函数重载

2.模板、枚举

3.可变参数

4.抽象类

5.大小端转换

......

## main

```
	
	DataStream ds;
	A a("hello", 18);
	ds << a;
	A b;
	ds >> b;
	
	//ds << 123 << 1.23 << true << "hello";
	//int a;
	//double b;
	//bool c;
	//string d;
	//ds >> a >> b >> c >> d;
	//cout << a << " " << b << "  " << c << "  " << d << endl;
	//
	//map<string, int> m;
	//m["hello"] = 123;
	//ds.write(m);
	//map<string, int> m1;
	//ds.read(m1);
	//cout << m1["hello"] << endl;
	// 
	//list<int> v{ 1,2,3 };
	//ds.write(v);
	//list<int> v2;
	//ds.read(v2);
	//cout << *v2.begin() << endl;
```



## 基本类型序列化+反序列化

### 基本数据类型编码

![1](https://github.com/COPELONG/CPP-Serialization/blob/main/C%2B%2B--Serialization/figs/1.png)

实现思路：

定义DataStrream类，数据成员：vector<char> m_buf;里面存储Type字节和value字节。定义wirte()和read()函数读入和读出数据。

```c++
	void reserve(int len);//实现容器的扩容。读入字节后的总大小需要小于m_buf.capacity()
	void write(const char* data, int len);
	void write(bool value);
	void write(char value);
	void write(int32_t value);
	void write(int64_t value);
	void write(float value);
	void write(double value);
	void write(const char* value);
	void write(string value);
```

#### 函数实现：

```
void DataStream::write(const char* data, int len)//将数据都转换成此类型，传入数据的长度。
{
	reserve(len);//扩容
	int size = m_buf.size();
	m_buf.resize(size + len);//必须提前手动改变size。
	memcpy(&m_buf[size], data, len);//简单的拷贝，不会改变size大小。
}
```



```c++
void DataStream::write(int32_t value)
{
	char type = DataType::INT32;//type等于枚举中的type。
	write((char*)&type, sizeof(char));//调用重载函数。写入数据类型。
	write((char*)&value, sizeof(int32_t));//写入value。
}
```

```c++
void DataStream::write(const char* value)//传入字符串
{
	char type = DataType::STRING;
	write((char*)&type, sizeof(char));
	int32_t len = strlen(value);
	write(len);//需要把字符串的长度数据也写入。
	write(value, len);
}
```

```c++
	bool read(bool& value);
	bool read(char& value);
	bool read(int32_t& value);
	bool read(int64_t& value);
	bool read(float& value);
	bool read(double& value);
	bool read(string& value);
	bool read(char* value, int len);
```

#### 函数实现：

```c++
bool DataStream::read(int32_t& value)//读取int32类型的数据
{
	if (m_buf[m_pos] != DataStream::INT32) {
		return false;//m_pos记录数组中的现读取位置。
	}
	value = *(int32_t*)&m_buf[++m_pos];//因为存储类型是char*类型，所以先取地址然后类型转换、再解引用。
	m_pos+=4;//读取后移动相应的字节。
	return true;
}
```

```c++
bool DataStream::read(string& value)
{
	if (m_buf[m_pos] != DataStream::STRING) {
		return false;
	}
	++m_pos;//根据存储形式，首先需要读取字符串的长度。
	int len = 0;
	read(len);
	value = string(&m_buf[m_pos],len);//调用string构造函数。
	m_pos += len;//移动
	return true;
}
```



## 复合类型序列化+反序列化

### 复合数据类型编码

![image-20230416145903207](D:\typora-image\image-20230416145903207.png)

**注：**模板函数的定义与类模板的成员函数通常放在头文件中。

#### 函数实现：

```c++
template<typename T>
void DataStream::write(const vector<T>& value)
{
	int len = value.size();//根据规则，首先读入数据的个数。
	char type = DataType::VECTOR;
	write(&type, sizeof(char));//读入类型
	write(len);//读入个数
	for (int i = 0; i < len; i++) {
		write(value[i]);//读入每个数据
	}
}
```

```c++
template<typename K, typename V>
inline bool DataStream::read(map<K, V>& value)
{
	value.clear();//清空数据
	if (m_buf[m_pos] != DataType::MAP) {
		return false;
	}
	++m_pos;
	int len;
	read(len);//获取容器中的数据个数
	for (int i = 0; i < len; i++) {
		K k;
		V v;
		read(k);//分别对K、V赋值
		read(v);
		value[k] = v;//存入MAP容器。
	}
	return true;
}
```



## 自定义类型序列化+反序列化

### 自定义对象类型编码

![image-20230416172748970](D:\typora-image\image-20230416172748970.png)



循环包含（circular inclusion）是指在两个或多个头文件中相互包含对方的头文件，导致编译器无法正确解析代码的一种情况。循环包含通常发生在头文件之间存在双向依赖关系时。例如，头文件A包含头文件B，头文件B又包含头文件A，这样就形成了循环包含。循环包含会导致编译器重复解析同一个头文件，从而产生编译错误。

#### 函数实现：

1.定义一个抽象基类。每个派生类都需要实现基类中的序列化函数和反序列化函数。

```c++
#pragma once
//#include"Datastream.h"
class DataStream;//防止循环包含
class Serializable {
public:
	virtual void serialize(DataStream& stream)const = 0;
	virtual bool unserialize(DataStream& stream) = 0;
};
```

2.使用宏定义的方式。

```c++
void write(const Serializable& value);
//当一个类是const时，必须调用const函数才能编译成功。

#define SERIALIZE(...)                            \  //可传入任意个数数据
    void serialize(DataStream& stream)const       \//必须为const
    {                                             \
        char type = DataStream::CUSTOM;           \
        stream.write(&type, sizeof(char));        \
        stream.write_args(__VA_ARGS__);           \  //可变参数函数
    }                                             \
                                                  \
    bool unserialize(DataStream& stream)          \
    {                                             \
        char type;                               \
        stream.read((char*)&type, sizeof(char));  \
        if (type != DataStream::CUSTOM)           \
        {                                         \
            return false;                        \
        }                                         \
        stream.read_args(__VA_ARGS__);             \
        return true;                              \
    }
```

3.使用可变参数传入。

```c++
	template<typename T,typename...Args>
	void write_args(const T& head, const Args&...arg);
	void write_args(){};

template<typename T, typename ...Args>
inline void DataStream::write_args(const T& head, const Args & ...arg)
{
	write(head);
	write_args(arg...);
}
```



## 大端与小端

![image-20230416211908764](D:\typora-image\image-20230416211908764.png)



一个字节由 8 个二进制位组成，而一个十六进制数字对应 4 个二进制位。

因此，一个字节可以用两个十六进制数字表示。

```c++
	enum ByteOrder {
		BigEndian,
		LittleEndian
	};
DataStream::ByteOrder DataStream::byteorder()
{
	int n = 0x12345678;
	char str[4];//0x12、0x34、0x56
	memcpy(str, &n, sizeof(int));
	if (str[0] == 0x12) {
		return BigEndian;
	}
	return LittleEndian;
}

	if (m_byteorder == ByteOrder::BigEndian) {
		char* first = (char*)&value;
		char* second = first + sizeof(float);
		reverse(first, second);//指针颠倒
	}
```







