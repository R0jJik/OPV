#include <iostream>
#include <omp.h>
#include <ctime>
using namespace std;
using namespace chrono;
const int N = 100000000;  

int main() {
    cout << "Число операций: " << N << endl;
    cout << "Максимум потоков: " << omp_get_max_threads() << endl;
    cout << "Количество ядер: " << omp_get_num_procs() << endl;
    cout << "Введите число потоков: ";
    int n;
    cin >> n;
    omp_set_num_threads(n);
    cout << "Используется потоков: " << n << endl;
    double* arr = new double[N];
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        arr[i] = rand();
    }
    clock_t start = clock();
    double sum1 = 0;
    for (int i = 0; i < N; i++) {
        sum1 += arr[i];
    }
    clock_t end = clock();
    double time_seq = (double)(end - start) / CLOCKS_PER_SEC;
    
    start = clock();
    double sum2 = 0;
#pragma omp parallel for reduction(+:sum2)
    for (int i = 0; i < N; i++) {
        sum2 += arr[i];
    }
    end = clock();
    double time_par = (double)(end - start) / CLOCKS_PER_SEC;

    cout << "Последовательно: " << time_seq << " сек" << endl;
    cout << "Параллельно: " << time_par << " сек" << endl;
    cout << "Ускорение: " << time_seq / time_par << "x" << endl;
    cout << "Суммы: " << sum1 << " = " << sum2 << " (" << (sum1 == sum2 ? "ok" : "error") << ")" << endl;

    delete[] arr;
    return 0;
}