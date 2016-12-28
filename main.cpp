// parallel_chechetkin_lab2.cpp: определяет точку входа для консольного приложения.
//
#include <iostream>
#include <math.h>
#include <omp.h>
#include <ctime>
#include<windows.h>
using namespace std;


double f(double x) {
	return (x)*(cos(1 / x)*cos(1 / x));
}

double integralByLocker(double a, double b, long n) {
	omp_lock_t lock;
	double S = 0;
	double x = 0;
	double fSum = 0;
	double h = (b - a) / n;
	omp_init_lock(&lock);
#pragma omp parallel for private(x)
	for (int i = 0; i < n; i++)
	{
		x = a + i*h;
		omp_set_lock(&lock);
		fSum += f(x);
		omp_unset_lock(&lock);
	}
	S = h*fSum;
	omp_destroy_lock(&lock);
	return S;
}

double integralByCriticalSection(double a, double b, long n) {
	double S = 0;
	double x = 0;
	double fSum = 0;
	double h = (b - a) / n;
#pragma omp parallel num_threads(3)
	{
#pragma parallel for
		for (int i = 0; i < n; i++)
		{
			x = a + i*h;
#pragma omp critical
			fSum += f(x);
		}
	}
	
	S = h*fSum;
	return S;
}

double integral(double a, double b, long n) {
	double S = 0;
	double x = 0;
	double fSum = 0;
	double h = (b - a) / n;
	for (int i = 0; i < n; i++)
	{
		x = a + i*h;
		fSum += f(x);
	}
	S = h*fSum;
	return S;
}


double integralByAtomic(double a, double b, long n) {
	double S = 0;
	double x = 0;
	double fSum = 0;
	double h = (b - a) / n;
#pragma omp parallel for private(x)
	for (int i = 0; i < n; i++)
	{
		x = a + i*h;
#pragma omp atomic
		fSum += f(x);
	}
	S = h*fSum;
	return S;
}

void makeExperiement(double a, double b, long n) {
	int start = 0;
	int end = 0;
	int time = 0;
	double res = 0;

	cout << "Experiement with n = " << n << endl << endl;

	start = GetTickCount();
	res = integralByLocker(a, b, n);
	end = GetTickCount();
	time = end - start;
	cout << "Locker" << endl;
	//cout << "n = " << n;
	cout << "result: " << res << endl;
	cout << "time: " << time << endl << endl;


	start = GetTickCount();
	res = integralByAtomic(a, b, n);
	end = GetTickCount();
	time = end - start;
	cout << "Atomic" << endl;
	//cout << "n = " << n;
	cout << "result: " << res << endl;
	cout << "time: " << time << endl << endl;

	start = GetTickCount();
	res = integralByCriticalSection(a, b, n);
	end = GetTickCount();
	time = end - start;
	cout << "Critical" << endl;
	//cout << "n = " << n;
	cout << "result: " << res << endl;
	cout << "time: " << time << endl << endl;

	start = GetTickCount();
	res = integral(a, b, n);
	end = GetTickCount();
	time = end - start;
	cout << "Simple" << endl;
	//cout << "n = " << n;
	cout << "result: " << res << endl;
	cout << "time: " << time << endl << endl;

	cout << "--------------------------------" << endl << endl;
}


int main()
{
	long n = 100000000;
	double a = 0.0001;
	double b = 1;

	makeExperiement(a, b, n / 1000);
	makeExperiement(a, b, n / 100);
	makeExperiement(a, b, n / 10);
	makeExperiement(a, b, n);

	return 0;
}

