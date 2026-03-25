#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t threads);
    ~ThreadPool();

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> future<typename invoke_result<F, Args...>::type>;

private:
    vector<thread> workers;
    queue<function<void()>> tasks;
    mutex queue_mutex;
    condition_variable condition;
    atomic<bool> stop;
};

ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this]() {
            while (true) {
                function<void()> task;
                {
                    unique_lock<mutex> lock(queue_mutex);
                    condition.wait(lock, [this]() {
                        return stop || !tasks.empty();
                        });

                    if (stop && tasks.empty())
                        return;

                    task = move(tasks.front());
                    tasks.pop();
                }
                task();
            }
            });
    }
}

ThreadPool::~ThreadPool() {
    {
        unique_lock<mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (thread& worker : workers) {
        worker.join();
    }
}

template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> future<typename invoke_result<F, Args...>::type> {
    using return_type = typename invoke_result<F, Args...>::type;

    auto task = make_shared<packaged_task<return_type()>>(
        bind(forward<F>(f), forward<Args>(args)...)
    );
    future<return_type> result = task->get_future();
    {
        unique_lock<mutex> lock(queue_mutex);
        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return result;
}

pair<thread::id, unsigned long long> factorial(int n) {
    unsigned long long result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
        this_thread::sleep_for(milliseconds(10));
    }
    return { this_thread::get_id(), result };
}


int main() {
    vector<int> numbers = {
        5, 7, 10, 12, 15, 8, 6, 9, 11, 20,
        4, 13, 14, 16, 17, 18, 19, 21, 22, 23
    };
    ThreadPool pool(4);
    vector<future<pair<thread::id, unsigned long long>>> futures;
    cout << "Добавляем " << numbers.size() << " задач...\n\n";

    for (int n : numbers) {
        futures.push_back(pool.enqueue(factorial, n));
    }
    cout << "РЕЗУЛЬТАТЫ:\n";
    for (size_t i = 0; i < numbers.size(); ++i) {
        auto [thread_id, result] = futures[i].get();
        cout << "Поток " << thread_id
            << " посчитал " << numbers[i] << "! = "
            << result << endl;
    }
    cout << "\nВсе задачи выполнены!\n";
    return 0;
}