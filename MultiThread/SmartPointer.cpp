#include <mutex>
using namespace std;

template <typename T>
class SmartPointer
{
	T *ptr;
	int copies_number;
	mutex _lock;
public:
	SmartPointer(T *p)
	{
		ptr = p;
		copies_number = 0;
	}

	SmartPointer(const T &obj)
	{
		unique_lock<mutex> lock(_lock);
		ptr = obj.ptr;
		copies_number = 1;
	}

	SmartPointer operator=(const T &obj)
	{
		unique_lock<mutex> lock(_lock);
		ptr = obj.ptr;
		copies_number = 1;
	}

	~SmartPointer()
	{
		unique_lock<mutex> lock(_lock);
		if (copies_number == 0 && ptr != NULL)
		{
			delete ptr;
		}
	}

	T* operator ->()
	{
		unique_lock<mutex> lock(_lock);
		return ptr;
	}

	T& operator*()
	{
		unique_lock<mutex> lock(_lock);
		return *ptr;
	}
};