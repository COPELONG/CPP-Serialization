#include "Datastream.h"
#include<iostream>
#include<string>
#include<vector>
#include<list>
#include<map>
#include<set>
#include<algorithm>
DataStream::ByteOrder DataStream::byteorder()
{
	int n = 0x12345678;
	char str[4];
	memcpy(str, &n, sizeof(int));
	if (str[0] == 0x12) {
		return BigEndian;
	}
	return LittleEndian;
}
void DataStream::reserve(int len)
{
	int size = m_buf.size();
	int cap = m_buf.capacity();
	if (size + len > cap) {
		while (size + len > cap) {
			if (cap == 0) {
				cap = 1;
			}
			else {
				cap *= 2;
			}
		}
		m_buf.reserve(cap);
	}
}

void DataStream::write(const char* data, int len)
{
	reserve(len);
	int size = m_buf.size();
	m_buf.resize(size + len);//手动改变size
	memcpy(&m_buf[size], data, len);//简单的拷贝，不会改变size大小。
}

void DataStream::write(bool value)
{
	char type = DataType::BOOL;
	write((char*)&type, sizeof(char));
	write((char*)&value, sizeof(char));
}

void DataStream::write(char value)
{
	char type = DataType::CHAR;
	write((char*)&type, sizeof(char));
	write((char*)&value, sizeof(char));
}

void DataStream::write(int32_t value)
{
	char type = DataType::INT32;
	write((char*)&type, sizeof(char));
	if (m_byteorder == ByteOrder::BigEndian) {
		char* first = (char*)&value;
		char* second = first+sizeof(int32_t);
		reverse(first, second);
	}
	write((char*)&value, sizeof(int32_t));
}

void DataStream::write(int64_t value)
{
	char type = DataType::INT64;
	write((char*)&type, sizeof(char));
	if (m_byteorder == ByteOrder::BigEndian) {
		char* first = (char*)&value;
		char* second = first + sizeof(int64_t);
		reverse(first, second);
	}
	write((char*)&value, sizeof(int64_t));
}

void DataStream::write(float value)
{
	char type = DataType::FLOAT;
	write((char*)&type, sizeof(char));
	if (m_byteorder == ByteOrder::BigEndian) {
		char* first = (char*)&value;
		char* second = first + sizeof(float);
		reverse(first, second);
	}
	write((char*)&value, sizeof(float));
}

void DataStream::write(double value)
{
	char type = DataType::DOUBLE;
	write((char*)&type, sizeof(char));
	if (m_byteorder == ByteOrder::BigEndian) {
		char* first = (char*)&value;
		char* second = first + sizeof(double);
		reverse(first, second);
	}
	write((char*)&value, sizeof(double));
}

void DataStream::write(const char* value)
{
	char type = DataType::STRING;
	write((char*)&type, sizeof(char));
	int32_t len = strlen(value);
	write(len);
	write(value, len);
}

void DataStream::write(string value)
{
	write(value.c_str());
}

void DataStream::write(const Serializable& value)
{
	value.serialize(*this);
}

bool DataStream::read(Serializable& value)
{
	return value.unserialize(*this);
}

DataStream& DataStream::operator<<(const Serializable& value)
{
	write(value);
	return*this;
}

DataStream& DataStream::operator>>(Serializable& value)
{
	read(value);
	return*this;
}

void DataStream::write_args()
{
}

void DataStream::show() const
{
	int size = m_buf.size();
	cout << "data size==" << size << endl;
	int i = 0;
	while (i < size) {
		switch ((DataType)m_buf[i])
		{
		case BOOL:
			if (m_buf[++i] == 0) {
				cout << "flase" << endl;
			}
			else {
				cout << "true" << endl;
			}
			++i;
			break;
		case CHAR:
			cout << m_buf[++i] << endl;
			++i;
			break;
		case INT32:
			cout << *(int32_t*)&m_buf[++i]<< endl;
			i += 4;
			break;
		case INT64:
			cout << *(int64_t*)&m_buf[++i] << endl;
			i += 8;
			break;
		case FLOAT:
			cout << *(float*)&m_buf[++i] << endl;
			i += 4;
			break;
		case DOUBLE:
			cout << *(double*)&m_buf[++i] << endl;
			i += 8;
			break;
		case STRING:
			if ((DataType)m_buf[++i] == INT32) {
				int len = *(int*)&m_buf[++i];
			i = 4 + i;
			cout << string(&m_buf[i],len) << endl;
			
			i = i + len;
			}
			else {
				throw new logic_error("string paser error");
			}
			break;
		default:
			break;
		}
	}
}

DataStream& DataStream::operator<<(bool value)
{
	write(value);
	return *this;
}

DataStream& DataStream::operator<<(char value)
{
	write(value);
	return *this;
}

DataStream& DataStream::operator<<(int32_t value)
{
	write(value);
	return *this;
}

DataStream& DataStream::operator<<(int64_t value)
{
	write(value);
	return *this;
}

DataStream& DataStream::operator<<(float value)
{
	write(value);
	return *this;
}

DataStream& DataStream::operator<<(double value)
{
	write(value);
	return *this;
}

DataStream& DataStream::operator<<(const char* value)
{
	write(value);
	return *this;
}

DataStream& DataStream::operator<<(string value)
{
	write(value);
	return *this;
}

bool DataStream::read(bool& value)
{
	if (m_buf[m_pos] != DataStream::BOOL) {
		return false;
	}
	else {
		value = m_buf[++m_pos];
		++m_pos;
		return true;
	}
	
}

bool DataStream::read(char& value)
{
	if (m_buf[m_pos] != DataStream::CHAR) {
		return false;
	}
	value = m_buf[++m_pos];
	++m_pos;
	return true;
}

bool DataStream::read(int32_t& value)
{
	if (m_buf[m_pos] != DataStream::INT32) {
		return false;
	}

	value = *(int32_t*)&m_buf[++m_pos];
	if (m_byteorder == ByteOrder::BigEndian) {
		char* first = (char*)&value;
		char* second = first + sizeof(int32_t);
		reverse(first, second);
	}
	m_pos+=4;
	return true;
}

bool DataStream::read(int64_t& value)
{
	if (m_buf[m_pos] != DataStream::INT64) {
		return false;
	}
	value = *(int64_t*)&m_buf[++m_pos];
	if (m_byteorder == ByteOrder::BigEndian) {
		char* first = (char*)&value;
		char* second = first + sizeof(int64_t);
		reverse(first, second);
	}
	m_pos += 8;
	return true;
}

bool DataStream::read(float& value)
{
	if (m_buf[m_pos] != DataStream::FLOAT) {
		return false;
	}
	value = *(float*)&m_buf[++m_pos];
	if (m_byteorder == ByteOrder::BigEndian) {
		char* first = (char*)&value;
		char* second = first + sizeof(float);
		reverse(first, second);
	}
	m_pos += 4;
	return true;
}

bool DataStream::read(double& value)
{
	if (m_buf[m_pos] != DataStream::DOUBLE) {
		return false;
	}
	
	value = *(double*)&m_buf[++m_pos];
	if (m_byteorder == ByteOrder::BigEndian) {
		char* first = (char*)&value;
		char* second = first + sizeof(double);
		reverse(first, second);
	}
	m_pos += 8;
	return true;
}

bool DataStream::read(string& value)
{
	if (m_buf[m_pos] != DataStream::STRING) {
		return false;
	}
	++m_pos;
	int len = 0;
	read(len);
	value = string(&m_buf[m_pos],len);
	m_pos += len;
	return true;
}

bool DataStream::read( char* value, int len)
{
	memcpy(value, &m_buf[m_pos], len);
	m_pos += len;
	return true;
}

DataStream& DataStream::operator>>(bool& value)
{
	read(value);
	return *this;
}

DataStream& DataStream::operator>>(char& value)
{
	read(value);
	return *this;
}

DataStream& DataStream::operator>>(int32_t& value)
{
	read(value);
	return *this;
}

DataStream& DataStream::operator>>(int64_t& value)
{
	read(value);
	return *this;
}

DataStream& DataStream::operator>>(float& value)
{
	read(value);
	return *this;
}

DataStream& DataStream::operator>>(double& value)
{
	read(value);
	return *this;
}


DataStream& DataStream::operator>>(string& value)
{
	read(value);
	return *this;
}






