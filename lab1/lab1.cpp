#include <chrono>
#include <thread>
#include <iostream>


using namespace std;

void task()
{
	for (size_t i = 0; i < 5; i++)
	{
		this_thread::sleep_for(chrono::milliseconds(300));
		cout << i + 1<<endl;
	}
	
}
void task_1()
{
	for (size_t i = 0; i < 5; i++)
	{
		this_thread::sleep_for(chrono::milliseconds(300));
		cout << "Поток 1: " << i + 1 << endl;
	}
}
void task_2()
{
	for (size_t i = 0; i < 5; i++)
	{
		this_thread::sleep_for(chrono::milliseconds(100));
		cout << "Поток 2: " << i + 1 << endl;
	}
}

void test1()
{
	thread t1(task);
	thread t2(task);
	for (size_t i = 0; i < 5; i++)
	{
		this_thread::sleep_for(chrono::milliseconds(200));
		cout << "Это главный поток!" << endl;
	}
	t1.join();
	t2.join();
}
void test2()
{
	thread t1(task_1);
	thread t2(task_2);
	t1.detach();
	for (size_t i = 0; i < 5; i++)
	{
		this_thread::sleep_for(chrono::milliseconds(200));
		cout << "Это главный поток!" << endl;
	}
	t2.join();
	return 0;
}

int main{
	setlocale(LC_ALL, "ru");
	test1();
	test2();
	return 0;
}
