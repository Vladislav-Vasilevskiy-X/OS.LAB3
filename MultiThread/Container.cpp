#include<mutex>
#include<queue>
using namespace std;

template <typename T>
class ThreadSafeContainer
{
private:
	mutex _lock;
	queue<T> _elements;
public:
	void push(T element)
	{
		unique_lock<mutex> lock(_lock);
		_elements.push(element);
	}

	void pop()
	{
		unique_lock<mutex> lock(_lock);
		_elements.pop();
	}

	T front()
	{
		unique_lock<mutex> lock(_lock);
		T t = _elements.front();
		return t;
	}

	T back()
	{
		unique_lock<mutex> lock(_lock);
		T t = _elements.back();
		return t;
	}

	bool empty()
	{
		return _elements.empty();
	}
};