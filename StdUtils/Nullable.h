#ifndef STDUTILS_NULLABLE_H
#define STDUTILS_NULLABLE_H

#include <StdUtils/Exceptions.h>

namespace StdUtils
{
	class NullableBase
	{
	protected:
		NullableBase()
		{
		}
	public:
		virtual ~NullableBase()
		{
		}

	public:
		virtual bool isNull() const = 0;
		virtual bool hasValue() const = 0;
		virtual void setNull() = 0;
	};

	template<typename T>
	class Nullable: public NullableBase
	{
	private:
		bool null;
		char value[sizeof(T)];

	public:
		Nullable()
			: null(true)
		{
		}
		Nullable(T const& value)
			: null(true) //Damit setValue sich richtig verhaelt
		{
			this->setValue(value);
		}
		Nullable(Nullable<T> const& original)
			: null(true) //Damit setValue sich richtig verhaelt
		{
			if (original.hasValue())
				this->setValue(original.getValue());
			else
				this->setNull();
		}
		virtual ~Nullable()
		{
			this->setNull();
		}

	public:
		Nullable<T>& operator=(Nullable<T> const& nb)
		{
			if (nb.hasValue())
				this->setValue(nb.getValue());
			else
				this->setNull();

			return *this;
		}
		Nullable<T>& operator=(T const& value)
		{
			this->setValue(value);
			return *this;
		}

		T& operator*()
		{
			return this->getValue();
		}
		T const& operator*() const
		{
			return this->getValue();
		}

		T* operator->()
		{
			return &this->getValue();
		}
		T const* operator->() const
		{
			return &this->getValue();
		}

		bool operator==(T const& value) const
		{
			return this->hasValue() && this->getValue() == value;
		}
		bool operator==(Nullable<T> const& value) const
		{
			if (this->hasValue() && value.hasValue())
				return this->getValue() == value.getValue();
			else if (this->hasValue() || value.hasValue())
				return false;

			return true;
		}
		bool operator!=(T const& value) const
		{
			return !this->operator==(value);
		}
		bool operator!=(Nullable<T> const& value) const
		{
			return !this->operator ==(value);
		}

	private:
		void assertNotNull() const
		{
			if (this->null)
				throw Exception("Nullable has no value");
		}
		void destroyValue()
		{
			reinterpret_cast<T*>(this->value)->~T();
		}

	public:
		virtual bool isNull() const STDUTILS_OVERRIDE
		{
			return this->null;
		}
		bool hasValue() const STDUTILS_OVERRIDE
		{
			return !this->null;
		}
		T & getValue()
		{
			this->assertNotNull();
			return *reinterpret_cast<T*>(this->value);
		}
		T const& getValue() const
		{
			this->assertNotNull();
			return *(T*) (this->value);
		}
		void setValue(T const& value)
		{
			if (this->hasValue())
			{
				T& fld(this->getValue());
				fld = value;
			}
			else
			{
				this->null = false;

				new ((T*) (this->value)) T(value);
			}
		}
		void setNull() STDUTILS_OVERRIDE
		{
			if (this->hasValue())
				this->destroyValue();
			this->null = true;
		}
	}
	;
}

#endif
