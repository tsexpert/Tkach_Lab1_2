// Tkach_Lab1_C.cpp : Defines the entry point for the console application.
//
/*
Semaphore

Данный объект синхронизации позволяет ограничить доступ потоков к объекту синхронизации на основании 
их количества. Например, мы хотим, чтобы к какому-нибудь объекту могли обратиться максимум 3 потока. 
Не больше. Тогда нам нужен семафор. Сначала семафор инициализируется и ему передается количество потоков, 
которые к нему могут обратиться. Дальше при каждом обращении к ресурсу его счетчик уменьшается. 
Когда счетчик уменьшиться до 0 к ресурсу обратиться больше нельзя. При отсоединении потока от семафора 
его счетчик увеличивается, что позволяет другим потокам обратиться к нему. Сигнальному состоянию 
соответствует значение счетчика больше нуля. Когда счетчик равен нулю, семафор считается не установленным 
(сброшенным).
Сначала нам нужно создать семафор, а для этого есть функция CreateSemaphore(). 
Эта функция создает семафор с заданным начальным значением счетчика и максимальным значением. 
Это значение ограничивает доступ. Функция OpenSemaphore() осуществляет доступ к семафору. 
Функция ReleaseSemaphore() увеличивает значение счетчика. Счетчик может меняться от 0 до максимального 
значения. После завершения работы достаточно вызвать CloseHandle(). Как происходит работа, 
то есть обращение? На основе двух функций:
WaitForSingleObject
WaitForMultipleObject
*/

#include "stdafx.h"
#include <iostream>
#include "windows.h"
#include "process.h" 

using namespace std;
HANDLE hSemaphore, hSemaphore2;	//handle - это обработчик событий, тут это семафоры. указатели на объекты
LONG cMax = 2;					//long это длинное целое число. максимальное количество одновременных	
								//потоков семафора. Я выбираю, что их максимум может быть 2

void Test1(void *);				// обьявление функции (процесса)
void Test2(void *);
void Test3(void *);


int main()						// функция main. Она запускается первой
{
	//создаём семафор
	hSemaphore = CreateSemaphore(
		NULL,			// нет атрибута
		cMax,			// начальное состояние
		cMax,			// максимальное состояние
		TEXT("SEMA")	// имя семафора
	);

	//создаём другой семафор с тем же именем
	hSemaphore2 = CreateSemaphore(
		NULL,			// нет атрибута
		cMax,			// начальное состояние
		cMax,			// максимальное состояние
		TEXT("SEMA")	// без имени
	);

	//проверяем состояние созданного семафора
	if (hSemaphore2 == NULL) cout << "Cannot create semaphore with the same name\n";
		else
			if (GetLastError() == ERROR_ALREADY_EXISTS)
				printf("CreateSemaphore opened existing semaphore\n");
			else 
				printf("CreateSemaphore created a new semaphore\n");

	//запускаем потоки
	cout << "Program is starting\n";
	if (!hSemaphore == NULL)
	{
		if (_beginthread(Test1, 1024, NULL) == -1) 
			cout << "Cannot create thread\n";
		if (_beginthread(Test2, 1024, NULL) == -1)
			cout << "Cannot create thread\n";
		if (_beginthread(Test3, 1024, NULL) == -1)
			cout << "Cannot create thread\n";
		Sleep(5000);				//даём всем потокам завершиться
		CloseHandle(hSemaphore);	//закрываем указатели на семафор
		CloseHandle(hSemaphore2);
	}
	else
		cout << "Cannot create semaphore\n";
	
	cout << endl << "Press Enter to exit";	//выход из программы
	cin.get();
	return 0;
}

void Test1(void *)
{
	cout << "Test1 Started\n";
	DWORD dwWaitResult = WAIT_TIMEOUT;
	HANDLE hSemaphoreL;

	//открываем существующий семафор по имени
	hSemaphoreL = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, TEXT("SEMA"));
	if (hSemaphoreL == NULL)
	{
		cout << "Can't open Semaphore";
		_endthread();
	}

	//ожидание доступа к объекту синхронизации
	while (dwWaitResult != WAIT_OBJECT_0)
	{
		dwWaitResult = WaitForSingleObject(
			hSemaphoreL,	// указатель на семафор
			1				// интерфал ожидания
		);
		cout << "Test 1 TIMEOUT\n";
	}
	//выполнение задачи потока
	Sleep(70);
	//завершение задачи потока, освобождение семафора
	if (ReleaseSemaphore(
		hSemaphoreL,	// указатель на семафор
		1,				// изменяет счетчик на 1
		NULL)
		)
		cout << "Test1 Finished\n";
	CloseHandle(hSemaphoreL);
	_endthread();
}

void Test2(void *)
{
	cout << "Test2 Started\n";
	DWORD dwWaitResult = WAIT_TIMEOUT;

	//используем глобальный указатель hSemaphore на семафор SEMA
	while (dwWaitResult != WAIT_OBJECT_0)
	{
		dwWaitResult = WaitForSingleObject(hSemaphore, 1);
		cout << "Test 2 TIMEOUT\n";
	}
	//выполнение задачи потока
	Sleep(100);
	//завершение задачи потока, освобождение семафора
	if (ReleaseSemaphore(hSemaphore, 1, NULL))
		cout << "Test2 Finished\n";
	_endthread();
}

void Test3(void *)
{
	cout << "Test3 Started\n";
	DWORD dwWaitResult = WAIT_TIMEOUT;

	//используем глобальный указатель на семафор hSemaphore2, ссылающийся на тот же семафор SEMA
	while (dwWaitResult != WAIT_OBJECT_0)
	{
		dwWaitResult = WaitForSingleObject(hSemaphore2, 1);
		cout << "Test 3 TIMEOUT\n";
	}
	//выполнение задачи потока
	Sleep(50);
	//завершение задачи потока, освобождение семафора
	if (ReleaseSemaphore(hSemaphore2, 1, NULL))
		cout << "Test3 Finished\n";
	_endthread();
}