template <typename T>
class SmartPointer
{
	T *ptr;
	int copies_number;
public:
	SmartPointer(T *p)
	{
		ptr = p;
		copies_number = 0;
	}

	SmartPointer(const T &obj)
	{
		ptr = obj.ptr;
		copies_number = 1;
	}

	SmartPointer operator=(const T &obj)
	{
		ptr = obj.ptr;
		copies_number = 1;
	}

	~SmartPointer()
	{
		if (copies_number == 0 && ptr != NULL)
		{
			delete ptr;
		}
	}

	T* operator ->()
	{
		return ptr;
	}

	T& operator*()
	{
		return *ptr;
	}
};