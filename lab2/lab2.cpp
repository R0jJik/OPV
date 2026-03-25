#include <iostream>
#include <thread>
#include <mutex>
#include<condition_variable>
#include<queue>
#include<chrono>
using namespace std;

queue<int> buffer;
mutex mtx;
recursive_mutex rmtx;
condition_variable cv;
const unsigned int limit = 5;
bool finished = false;
thread_local unsigned int counter = 0;

void recursev(int depth)
{
	if (!depth) return;
	unique_lock<recursive_mutex>lock(rmtx);
	cout << "Глубина: " << depth << endl;
	recursev(depth - 1);
	lock.unlock();
}
void recursev2(int depth)
{
	if (!depth) return;
	unique_lock<mutex>lock(mtx);
	cout << "Глубина: " << depth << endl;
	recursev2(depth - 1);
	
	lock.unlock();
}


void producer() {
	for (int i = 0; i < 20; ++i) {
		unique_lock<mutex>lock(mtx);
		cv.wait(lock, [] { return buffer.size() < limit; });
		int namber = rand() % 100;
		buffer.push(namber);
		cout << "Производитель добавил: " << namber << endl;
		lock.unlock();
		cv.notify_one();
		++counter;
		this_thread::sleep_for(chrono::milliseconds(200));
	}
	{
		lock_guard<mutex>lock(mtx);
		cout << "Производитель выполнил " <<  counter << " операций" << endl;
		finished = true;
	}
	cv.notify_all();
}
void consumer() {
	while (true) {
		unique_lock<mutex>lock(mtx);
		cv.wait(lock, [] { return !buffer.empty() || finished; });
		if (!buffer.empty()) {
			int value = buffer.front();
			buffer.pop();
			cout << "Потребитель " << this_thread::get_id() << " получил: " << value*value << endl;
			lock.unlock();
			cv.notify_one();
			++counter;
			this_thread::sleep_for(chrono::milliseconds(300));
		} else if(finished) {
			cout << "Потребитель " << this_thread::get_id() << " выполнил " << counter << " операций" << endl;
			break;
		}
	}
}
int main() {
	int n = 3;
	thread prod(producer);
	thread cons1(consumer);
	thread cons2(consumer);
	prod.join();
	cons1.join();
	cons2.join();
	thread rec(recursev,5);
	rec.join();
	return 0;
}
