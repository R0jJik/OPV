#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <string>
#include <algorithm>

unsigned long long factorial(int i)
{
	unsigned long long x = 1;
	int t = i;
	for ( i; i>1 ; i--)
	{
		x = x * i;
	}
	this_thread::sleep_for(chrono::milliseconds(300*t));
	return x;
}
vector<int> ret_N()
{
	int n = 0;
	int i = 1;
	vector<int> N;
	cout << "Ввведите значения для подсчета факториала:\n";
	while (true)
	{
		cout << i << ") ";
		cin >> n;
		if (n != -1)
		{
			N.push_back(n);
			i++;
		}
		else { break; }	
	}
	sort(N.begin(), N.end());
	return N;
}


int main()
{
	vector<int> N = ret_N();
	vector<future<unsigned long long>> Future;
	vector<thread> Treads;

	promise<void> flag;
	future<void> chek = flag.get_future();

	for (int n : N)
	{
		packaged_task<unsigned long long(int)> fact(factorial);
		Future.push_back(fact.get_future());
		thread t(move(fact), n);
		Treads.push_back(move(t));
	}

	thread monitor([&Future, &flag, global = N.size()]()
		{
			int size = global;
			cout << "Прогресс: [ " << string(global - size, '=') << string(size, ' ') << " ]" << global - size << " / " << global;
			for (future<unsigned long long>& f : Future)
			{
				f.wait();
				size--;
				cout << "\rПрогресс: [ " << string(global - size,'=') << string(size, ' ') << " ]" << global - size << " / " << global;
				cout.flush();
			}
			cout << "\rВсе вычесления завершены!" << string(5,' ')<<endl;
			flag.set_value();
		});
	chek.wait();
	cout << "Полученные результаты:\n";
	for (int i=0; i < N.size(); i++)
	{
		cout << "Факториал " << N[i] << " : " << Future[i].get()<<endl;
	}
	for (thread& t : Treads)
	{
		t.join();
	}
	monitor.join();
	return 0;
}


