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
		_lock.lock();
		_elements.push(element);
		_lock.unlock();
	}

	void pop()
	{
		_lock.lock();
		_elements.pop();
		_lock.unlock();
	}

	T front()
	{
		_lock.lock();
		T t = _elements.front();
		_lock.unlock();
		return t;
	}

	T back()
	{
		_lock.lock();
		T t = _elements.back();
		_lock.unlock();
		return t;
	}

	bool empty()
	{
		return _elements.empty();
	}

	mutex getMutex()
	{
		return _lock;
	}
};