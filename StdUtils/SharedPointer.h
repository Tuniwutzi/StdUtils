#ifndef STDUTILS_SHAREDPOINTER
#define STDUTILS_SHAREDPOINTER

#include <StdUtils/Threading/Mutex.h>
#include <StdUtils/Exceptions.h>

namespace StdUtils
{
	template<typename T>
	class STDUTILS_DLLCONTENT SharedPointer
	{
		template<typename> friend class SharedPointer;

	private:
		T* pointer;

		unsigned int* references;
		StdUtils::Threading::Mutex* lock;

	private:
		explicit SharedPointer(T* data, unsigned int* references, StdUtils::Threading::Mutex* lock)
		{
			this->pointer = data;
			this->references = references;
			this->lock = lock;

			(*this->references)++;
		}

	public:
		explicit SharedPointer(T* data = NULL)
		{
			this->init();

			this->pointer = data;
		}
		SharedPointer(SharedPointer<T> const& original)
		{
			copy(original);
		}
		~SharedPointer()
		{
			destruct();
		}

	public:
		SharedPointer<T>& operator=(SharedPointer<T> const& original)
		{
			destruct();
			copy(original);

			return *this;
		}

		T* operator->() const
		{
			return this->pointer;
		}

		T& operator*() const
		{
			return *this->pointer;
		}

	private:
		void init()
		{
			this->references = new unsigned int(1);

			this->lock = new StdUtils::Threading::Mutex();

			this->pointer = NULL;
		}
		void destruct()
		{
			this->lock->lock();

			(*this->references)--;
			if (*this->references <= 0)
			{
				delete this->pointer;
				delete this->references;
				delete this->lock;
			}
			else
			{
				this->lock->release();
			}
		}
		void copy(SharedPointer<T> const& original)
		{
			original.lock->lock();

			this->pointer = original.pointer;
			this->lock = original.lock;
			this->references = original.references;

			(*this->references)++;

			original.lock->release();
		}

	public:
		T* getRawPointer() const
		{
			return this->pointer;
		}
		bool isNull() const
		{
			return this->pointer == NULL;
		}

		void setNull()
		{
			this->destruct();
			this->init();
		}

		template<typename TO>
		SharedPointer<TO> dynamicCast() const
		{
			TO* t = dynamic_cast<TO*>(this->pointer);
			if (t != NULL)
				return SharedPointer<TO>(t, this->references, this->lock);
			else
				throw Exception("Invalid cast of shared pointer");
		}
	};
}

#endif