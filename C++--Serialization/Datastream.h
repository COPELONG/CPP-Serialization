#pragma once

#include<string>
#include<vector>
#include<list>
#include<map>
#include<set>
#include"Serializable.h"
using namespace std;


class DataStream {
public:
	enum ByteOrder {
		BigEndian,
		LittleEndian
	};
	enum DataType {
		BOOL = 0,
		CHAR,
		INT32,
		INT64,
		FLOAT,
		DOUBLE,
		STRING,
		VECTOR,
		MAP,
		LIST,
		SET,
		CUSTOM
	};
	DataStream():m_pos(0) {
		m_byteorder = byteorder();
	};
	~DataStream() {};

	ByteOrder byteorder();

	void reserve(int len);
	void write(const char* data, int len);
	void write(bool value);
	void write(char value);
	void write(int32_t value);
	void write(int64_t value);
	void write(float value);
	void write(double value);
	void write(const char* value);
	void write(string value);

	template<typename T>
	void write(const vector<T>& value);
	template<typename T>
	void write(const list<T>& value);
	template<typename K,typename V>
	void write(const map<K,V>& value);
	template<typename T>
	void write(const set<T>& value);


	void write(const Serializable& value);
	bool read(Serializable& value);
	DataStream& operator<<(const Serializable& value);
	DataStream& operator>>(Serializable& value);

	template<typename T,typename...Args>
	void write_args(const T& head, const Args&...arg);
	void write_args();

	template<typename T, typename...Args>
	bool read_args( T& value,  Args&...arg);
	bool read_args() { return true; }


	template<typename T>
	bool read(vector<T>& value);
	template<typename T>
	bool read(list<T>& value);
	template<typename K, typename V>
	bool read( map<K, V>& value);
	template<typename T>
	bool read( set<T>& value);

	void show() const;

	DataStream& operator<<(bool value);
	DataStream& operator<<(char value);
	DataStream& operator<<(int32_t value);
	DataStream& operator<<(int64_t value);
	DataStream& operator<<(float value);
	DataStream& operator<<(double value);
	DataStream& operator<<(const char* value);
	DataStream& operator<<(string value);
	template<typename T>
	DataStream& operator<<(const vector<T> value);
	template<typename T>
	DataStream& operator<<(const list<T> value);
	template<typename K,typename V>
	DataStream& operator<<(const map<K,V> value);
	template<typename T>
	DataStream& operator<<(const set<T> value);

	bool read(bool& value);
	bool read(char& value);
	bool read(int32_t& value);
	bool read(int64_t& value);
	bool read(float& value);
	bool read(double& value);
	bool read(string& value);
	bool read(char* value, int len);


	DataStream& operator>>(bool& value);
	DataStream& operator>>(char& value);
	DataStream& operator>>(int32_t& value);
	DataStream& operator>>(int64_t& value);
	DataStream& operator>>(float& value);
	DataStream& operator>>(double& value);
	DataStream& operator>>(string& value);
	template<typename T>
	DataStream& operator>>( vector<T>& value);
	template<typename T>
	DataStream& operator>>( list<T>& value);
	template<typename K, typename V>
	DataStream& operator>>( map<K, V>& value);
	template<typename T>
	DataStream& operator>>( set<T>& value);
private:
	vector<char> m_buf;
	int m_pos;//记录解码的当前位置
	ByteOrder m_byteorder;
};

template<typename T>
void DataStream::write(const vector<T>& value)
{
	int len = value.size();
	char type = DataType::VECTOR;
	write(&type, sizeof(char));
	write(len);
	for (int i = 0; i < len; i++) {
		write(value[i]);
	}
}
template<typename T>
void DataStream::write(const list<T>& value)
{
	int len = 0;
	len = value.size();
	char type = DataType::LIST;
	write((char*)&type, sizeof(char));
	write(len);
	for (auto it = value.begin(); it != value.end(); it++) {
		write(*it);
	}
}

template<typename K, typename V>
inline void DataStream::write(const map<K, V>& value)
{
	int len = value.size();
	char type = DataType::MAP;
	write(&type, sizeof(char));
	write(len);
	for (auto it = value.begin(); it != value.end();it++) {
		write(it->first);
		write(it->second);
	}
}

template<typename T>
inline void DataStream::write(const set<T>& value)
{
	int len = value.size();
	char type = DataType::SET;
	write(&type, sizeof(char));
	write(len);
	for (auto it = value.begin(); it != value.end(); it++) {
		write(*it);
	}
}



template<typename T, typename ...Args>
inline void DataStream::write_args(const T& head, const Args & ...arg)
{
	write(head);
	write_args(arg...);
}

template<typename T, typename ...Args>
inline bool DataStream::read_args(T& value, Args & ...arg)
{
	read(value);
	return read_args(arg...);
}

template<typename T>
bool DataStream::read(vector<T>& value)
{
	value.clear();
	if (m_buf[m_pos] != DataType::VECTOR) {
		return false;
	}
	++m_pos;
	int len = 0;
	read(len);
	for (int i = 0; i < len; i++) {
		T t;
		read(t);
		value.push_back(t);
	}
	return true;
}

template<typename T>
bool DataStream::read(list<T>& value)
{
	value.clear();
	if (m_buf[m_pos] != DataType::LIST) {
		return false;
	}
	++m_pos;
	int len = 0;
	read(len);
	for (int i = 0; i < len; i++) {
		T t;
		read(t);
		value.push_back(t);
	}
	return true;
}

template<typename K, typename V>
inline bool DataStream::read(map<K, V>& value)
{
	value.clear();
	if (m_buf[m_pos] != DataType::MAP) {
		return false;
	}
	++m_pos;
	int len;
	read(len);
	for (int i = 0; i < len; i++) {
		K k;
		V v;
		read(k);
		read(v);
		value[k] = v;
	}
	return true;
	
}

template<typename T>
inline bool DataStream::read(set<T>& value)
{
	value.clear();
	if (m_buf[m_pos] != DataType::SET) {
		return false;
	}
	++m_pos;
	int len;
	read(len);
	for (int i = 0; i < len; i++) {
		T t;
		read(t);
		value.insert(t);
	}
	return true;
}

template<typename T>
inline DataStream& DataStream::operator<<(const vector<T> value)
{
	write(value);
	return *this;
	// TODO: 在此处插入 return 语句
}

template<typename T>
inline DataStream& DataStream::operator<<(const list<T> value)
{
	write(value);
	return *this;
}

template<typename K, typename V>
inline DataStream& DataStream::operator<<(const map<K, V> value)
{
	write(value);
	return *this;
}

template<typename T>
inline DataStream& DataStream::operator<<(const set<T> value)
{
	write(value);
	return *this;
}

template<typename T>
inline DataStream& DataStream::operator>>(vector<T>& value)
{
	read(value);
	return*this;
}

template<typename T>
inline DataStream& DataStream::operator>>(list<T>& value)
{
	read(value);
	return*this;
}

template<typename K, typename V>
inline DataStream& DataStream::operator>>(map<K, V>& value)
{
	read(value);
	return*this;
}

template<typename T>
inline DataStream& DataStream::operator>>(set<T>& value)
{
	read(value);
	return*this;
}