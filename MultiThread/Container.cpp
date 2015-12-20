#include<mutex>
#include<queue>
using namespace std;

template <typename T>
class container
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

	void addRange(int num, ...)
	{
		va_list arguments;
		__crt_va_start_a(arguments, num);
		for (int i = 0; i < num; i++)
		{
			Add(__crt_va_arg(arguments, T));
		}
		__crt_va_end(arguments);
	}
	
	void dump()
	{
		_lock.lock();
		for (auto e : _elements)
			cout << e << endl;
		_lock.unlock();
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