#include<thread>
#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<math.h>
#include <queue>
#include "SmartPointer.cpp"
#include <sstream>
using namespace std;

class Point {
public:
	double x, y;

	Point(double x, double y) {
		Point::x = x;
		Point::y = y;
	}
};

mutex _print_lock;
bool evaluation_done;
bool printing_done;
fstream logFile, valuesFile;

//ver 1
//SmartPointer<ThreadSafeContainer<Point*>> smart_ptr_points_queue(new ThreadSafeContainer<Point*>);
//SmartPointer<ThreadSafeContainer<unsigned int>> smart_ptr_eval_time_queue(new ThreadSafeContainer<unsigned int>);
//SmartPointer<ThreadSafeContainer<unsigned int>> smart_ptr_file_out_time_queue(new ThreadSafeContainer<unsigned int>);
//SmartPointer<ThreadSafeContainer<string>> smart_ptr_log_messages_queue(new ThreadSafeContainer<string>);

//ver2
//SmartPointer<queue<Point*>> smart_ptr_points_queue(new queue<Point*>);

//ver3
queue<SmartPointer<Point>*> smart_pointers_queue;
SmartPointer<queue<unsigned int>> smart_ptr_eval_time_queue(new queue<unsigned int>);
SmartPointer<queue<unsigned int>> smart_ptr_file_out_time_queue(new queue<unsigned int>);
SmartPointer<queue<string>> smart_ptr_log_messages_queue(new queue<string>);

void printEvaluationToFile(double x, double y)
{
	valuesFile << "x: " << x << "; y: " << y << endl;
}

void addStringToLogBuffer(string str)
{
	unique_lock<mutex> lock(_print_lock);
	ostringstream ss;
	ss << str;
	smart_ptr_log_messages_queue->push(ss.str());
}

void addStringToLogBuffer(string str1, string str2, unsigned int num1, unsigned int num2)
{
	unique_lock<mutex> lock(_print_lock);
	ostringstream ss;
	ss << str1 << num1 << str2 << num2 << endl;
	smart_ptr_log_messages_queue->push(ss.str());
}

void addStringToLogBuffer(string str1, string str2, double num1, double num2)
{
	unique_lock<mutex> lock(_print_lock);
	ostringstream ss;
	ss << str1 << num1 << str2 << num2 << endl;
	smart_ptr_log_messages_queue->push(ss.str());
}

void initiateFiles()
{
	logFile.open("log.txt", ios::ate | ios::out);
	valuesFile.open("values.txt", ios::ate | ios::out);
}

void closeFiles()
{
	logFile.close();
	valuesFile.close();
}

double evaluateY(double x)
{
	return pow(x, 3);
}

void writeLogFile()
{
	while (!smart_ptr_log_messages_queue->empty())
	{
		logFile << smart_ptr_log_messages_queue->front();
		smart_ptr_log_messages_queue->pop();
	}
}

void evaluateMathFunc()
{
	addStringToLogBuffer("[thread1] thread started...\n");

	for (int i = 0; i < 10; i++)
	{
		double x = i - 5;

		unsigned int timeNowMS = clock();
		double y = evaluateY(x);
		unsigned int lastEvaluatedTimeInMS = clock() - timeNowMS;

		Point* point = new Point(x, y);
		SmartPointer<Point> *ptr = new SmartPointer<Point>(point);

		addStringToLogBuffer("[thread1] evaluated values x: ", "; y: ", x, y);

		smart_pointers_queue.push(ptr);

		smart_ptr_eval_time_queue->push(lastEvaluatedTimeInMS);
	}
}

void printInfoAboutEvaluation()
{
	addStringToLogBuffer("[thread2] thread started...\n");

	while (!evaluation_done)
	{
		while (!smart_pointers_queue.empty())
		{
			unsigned int timeNowMS = clock();
			Point tmpPoint = **smart_pointers_queue.front();

			printEvaluationToFile(tmpPoint.x, tmpPoint.y);
			addStringToLogBuffer("[thread2] printed values to file x: ", "; y: ", tmpPoint.x, tmpPoint.y);

			delete smart_pointers_queue.front();
			smart_pointers_queue.pop();

			unsigned int lastPrintedTimeInMS = clock() - timeNowMS;
			smart_ptr_file_out_time_queue->push(lastPrintedTimeInMS);
		}
	}
}

void printTiming()
{
	addStringToLogBuffer("[thread3] thread started...\n");

	while (!printing_done)
	{
		while (!smart_ptr_file_out_time_queue->empty() && !smart_ptr_eval_time_queue->empty())
		{
			addStringToLogBuffer("[thread3] eval time(ms): ", "; file output time(ms): ", smart_ptr_eval_time_queue->front(), smart_ptr_file_out_time_queue->front());

			smart_ptr_eval_time_queue->pop();
			smart_ptr_file_out_time_queue->pop();
		}
	}
}

int main()
{
	initiateFiles();
	thread timePrinterThread(printTiming);
	thread loggerThread(printInfoAboutEvaluation);
	thread evaluatingThread(evaluateMathFunc);

	evaluatingThread.join();
	addStringToLogBuffer("[thread1] joined to main thread.\n");
	evaluation_done = true;

	loggerThread.join();
	addStringToLogBuffer("[thread2] joined to main thread.\n");

	printing_done = true;

	timePrinterThread.join();
	addStringToLogBuffer("[thread3] joined to main thread.\n");

	writeLogFile();
	closeFiles();

	return 0;
}