#include<thread>
#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<math.h>
#include <queue>
#include "Container.cpp"
#include <sstream>
using namespace std;

bool               evaluation_done;
bool               printing_done;
fstream logFile, valuesFile;

class Point {
public:
	double x, y;

	Point(double x, double y) {
		Point::x = x;
		Point::y = y;
	}
};

container<Point*> *pointsContainter;
container<unsigned int> *evalTimesContainter;
container<unsigned int> *timeForFileOutputContainter;
container<string> *logMessagesContainer;

void PrintEvaluationToFile(double x, double y)
{
	valuesFile << "x: " << x << "; y: " << y << endl;
}

void addStringToLogBuffer(string str)
{
	ostringstream ss;
	ss << str;
	logMessagesContainer->push(ss.str());
}

void addStringToLogBuffer(string str1, string str2, unsigned int num1, unsigned int num2)
{
	ostringstream ss;
	ss << str1 << num1 << str2 << num2 << endl;
	logMessagesContainer->push(ss.str());
}

void addStringToLogBuffer(string str1, string str2, double num1, double num2)
{
	ostringstream ss;
	ss << str1 << num1 << str2 << num2 << endl;
	logMessagesContainer->push(ss.str());
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
	while (!logMessagesContainer->empty())
	{
		logFile << logMessagesContainer->front();
		logMessagesContainer->pop();
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

		addStringToLogBuffer("[thread1] evaluated values x: ", "; y: ", x, y);

		pointsContainter->push(point);
		evalTimesContainter->push(lastEvaluatedTimeInMS);
	}
}

void printInfoAboutEvaluation()
{
	addStringToLogBuffer("[thread2] thread started...\n");

	while (!evaluation_done)
	{
		while (!pointsContainter->empty())
		{
			unsigned int timeNowMS = clock();
			Point* tmpPoint = pointsContainter->front();

			PrintEvaluationToFile(tmpPoint->x, tmpPoint->y);
			addStringToLogBuffer("[thread2] printed values to file x: ", "; y: ", tmpPoint->x, tmpPoint->y);

			pointsContainter->pop();

			unsigned int lastPrintedTimeInMS = clock() - timeNowMS;
			timeForFileOutputContainter->push(lastPrintedTimeInMS);
		}
	}
}

void printTiming()
{
	addStringToLogBuffer("[thread3] thread started...\n");

	while (!printing_done)
	{
		while (!timeForFileOutputContainter->empty() && !evalTimesContainter->empty())
		{
			addStringToLogBuffer("[thread3] eval time(ms): ", "; file output time(ms): ", evalTimesContainter->front(), timeForFileOutputContainter->front());

			evalTimesContainter->pop();
			timeForFileOutputContainter->pop();
		}
	}
}

int main()
{
	pointsContainter = new container<Point*>();
	evalTimesContainter = new container<unsigned int>();
	timeForFileOutputContainter = new container<unsigned int>();
	logMessagesContainer = new container<string>();

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