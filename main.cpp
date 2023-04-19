#include<iostream>
#include<string>
#include "Datastream.h"
#include"Serializable.h"

using namespace std;

class A :public Serializable {
public:
	A() {};
	A(const string& name, int age) :m_name(name), m_age(age) {	}
	~A() {};
	SERIALIZE(m_name, m_age);
private:
	string m_name;
	int m_age;
};
int main()
{
	DataStream ds;
	A a("hello", 18);
	ds << a;
	A b;
	ds >> b;
	//set<int> s;
	//s.insert(123);
	////ds.write(s);
	//ds<<s;
	//set<int> s1;
	////ds.read(s1);
	//ds >> s1;
	//cout << *s1.begin() << endl;

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
	// 
	//ds << 123 << 1.23 << true << "hello";
	//int a;
	//double b;
	//bool c;
	//string d;
	//ds >> a >> b >> c >> d;
	//cout << a << " " << b << "  " << c << "  " << d << endl;
	return 0;

}