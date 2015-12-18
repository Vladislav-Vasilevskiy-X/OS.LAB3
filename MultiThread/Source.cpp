#include<thread>
#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<io.h>
#include<math.h>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <random>
#include<iomanip>

using namespace std;

mutex              lock_print;
mutex              lock_buffer;
mutex              lock_ev_buffer;
mutex              lock_pr_buffer;
condition_variable check_buffer;
condition_variable check_ev_buffer;
condition_variable check_pr_buffer;
bool               evaluation_done;
bool               notified_about_evaluation;
bool               printing_done;
bool               notified_about_printing;

fstream logFile, valuesFile;

class Point {
public:
	int x, y;

	Point(double x, double y) {
		Point::x = x;
		Point::y = y;
	}
};

queue<Point*> buffer;
queue<unsigned int> evaluatedTimesBuffer;
queue<unsigned int> printedTimesBuffer;


void PrintEvaluationToFile(double x, double y)
{
	valuesFile << fixed << setprecision(2) << "x: " << x << "; y: " << y << endl;
}

void LogToFile(unsigned int evaluationTime, unsigned int outputTime)
{
	logFile << "Evaluation time: " << evaluationTime << "milliseconds; file output time: " << outputTime << "milliseconds. " << endl;
}

void InitiateFiles()
{
	logFile.open("log.txt", ios::ate | ios::out);
	valuesFile.open("values.txt", ios::ate | ios::out);
}

void CloseFiles()
{
	logFile.close();
	valuesFile.close();
}

double EvaluateY(double x)
{
	return pow(x, 3);
}

void EvaluateMathFunc()
{
	{
		unique_lock<mutex> locker(lock_print);
		cout << "[Counter]\trunning..." << endl;
	}

	for (int i = 0; i < 10; i++)
	{
		double x = i - 5;

		unsigned int timeNowMS = clock();
		double y = EvaluateY(x);
		unsigned int lastEvaluatedTimeInMS = clock() - timeNowMS;

		Point* point = new Point(x, y);

		{
			unique_lock<mutex> locker(lock_print);
			cout << "[Counted] x: " << x << "; y: " << point->y << endl;
		}

		{
			unique_lock<mutex> locker(lock_buffer);
			buffer.push(point);
			check_buffer.notify_all();

			unique_lock<mutex> ev_locker(lock_ev_buffer);
			evaluatedTimesBuffer.push(lastEvaluatedTimeInMS);
			check_ev_buffer.notify_all();
		}
	}
}

void PrintInfoAboutEvaluation()
{
	{
		unique_lock<mutex> locker(lock_print);
		cout << "[printer]\trunning..." << endl;
	}
	while (!evaluation_done)
	{
		unique_lock<mutex> locker(lock_buffer);
		check_buffer.wait(locker, [&]() {return !buffer.empty(); });
		while (!buffer.empty())
		{
			unique_lock<mutex> locker(lock_print);
			unsigned int timeNowMS = clock();
			Point* tmpPoint = buffer.front();
			buffer.pop();
			check_buffer.notify_all();

			PrintEvaluationToFile(tmpPoint->x, tmpPoint->y);
			cout << "[printer] x: " << tmpPoint->x << "; y: " << tmpPoint->y << endl;

			unique_lock<mutex> pr_buffer_locker(lock_pr_buffer);
			unsigned int lastPrintedTimeInMS = clock() - timeNowMS;
			printedTimesBuffer.push(lastPrintedTimeInMS);
			check_pr_buffer.notify_all();
		}
	}
}

void PrintTiming()
{
	{
		unique_lock<mutex> locker(lock_print);
		cout << "[time printer]\trunning..." << endl;
	}
	while (!evaluation_done && !printing_done)
	{
		unique_lock<mutex> ev_buffer_locker(lock_ev_buffer);
		check_ev_buffer.wait(ev_buffer_locker, [&]() {return !evaluatedTimesBuffer.empty(); });

		unique_lock<mutex> pr_buffer_locker(lock_pr_buffer);
		check_pr_buffer.wait(pr_buffer_locker, [&]() {return !printedTimesBuffer.empty(); });
		while (!evaluatedTimesBuffer.empty() && !printedTimesBuffer.empty())
		{
			unique_lock<mutex> print_locker(lock_print);
			LogToFile(evaluatedTimesBuffer.back(), printedTimesBuffer.back());
			cout << "[time printer] eval time: " << evaluatedTimesBuffer.back() << "ms; file output time: " << printedTimesBuffer.back() << "ms. " << endl;
			evaluatedTimesBuffer.pop();
			check_ev_buffer.notify_all();

			printedTimesBuffer.pop();
			check_pr_buffer.notify_all();
		}
	}
}

int main()
{
	InitiateFiles();
	thread timePrinterThread(PrintTiming);
	thread loggerThread(PrintInfoAboutEvaluation);
	thread evaluatingThread(EvaluateMathFunc);
	evaluatingThread.join();
	evaluation_done = true;
	loggerThread.join();
	printing_done = true;
	timePrinterThread.join();
	CloseFiles();
	return 0;
}