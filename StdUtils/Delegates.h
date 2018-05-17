#ifndef STDUTILS_DELEGATES_H
#define STDUTILS_DELEGATES_H

#include <StdUtils/Base.h>

#include <StdUtils/Exceptions.h>

namespace StdUtils
{
	template<typename R, typename P>
	class STDUTILS_DLLCONTENT IDelegate
	{
	public:
		virtual ~IDelegate() {}

	public:
		virtual R operator()(P) const = 0;
		virtual bool operator==(IDelegate<R, P> const&) const = 0;
		virtual bool operator!=(IDelegate<R, P> const&) const = 0;

	public:
		virtual IDelegate* copy() const = 0;

		virtual bool isNull() const = 0;
	};
	template<typename P>
	class IDelegate<void, P>
	{
	public:
		virtual ~IDelegate() {}

	public:
		virtual void operator()(P) const = 0;
		virtual bool operator==(IDelegate<void, P> const&) const = 0;
		virtual bool operator!=(IDelegate<void, P> const&) const = 0;

	public:
		virtual IDelegate* copy() const = 0;

		virtual bool isNull() const = 0;
	};

	template<typename R>
	class IDelegate<R, void>
	{
	public:
		virtual ~IDelegate() {}

	public:
		virtual R operator()() const = 0;
		virtual bool operator==(IDelegate<R, void> const&) const = 0;
		virtual bool operator!=(IDelegate<R, void> const&) const = 0;

	public:
		virtual IDelegate* copy() const = 0;

		virtual bool isNull() const = 0;
	};

	template<>
	class IDelegate<void, void>
	{
	public:
		virtual ~IDelegate() {}

	public:
		virtual void operator()() const = 0;
		virtual bool operator==(IDelegate<void, void> const&) const = 0;
		virtual bool operator!=(IDelegate<void, void> const&) const = 0;

	public:
		virtual IDelegate* copy() const = 0;

		virtual bool isNull() const = 0;
	};

	template<class C, typename R, typename P>
	class STDUTILS_DLLCONTENT DelegateMethod : public IDelegate<R, P>
	{
	public:
		typedef R (C::* Method)(P);

	private:
		C* instance;
		Method method;

	public:
		DelegateMethod(C& instance, Method method)
			:instance(&instance),
			method(method)
		{
		}

		DelegateMethod()
			:instance(NULL),
			method(NULL)
		{
		}

		DelegateMethod(DelegateMethod const& original)
			:instance(original.instance),
			method(original.method)
		{
		}

		virtual ~DelegateMethod() {}

	public:
		DelegateMethod& operator=(DelegateMethod const& original)
		{
			this->instance = original.instance;
			this->method = original.method;
			return *this;
		}

		R operator()(P parameter) const STDUTILS_OVERRIDE
		{
			if (this->method)
				return (instance->*method)(parameter);
			else
				throw Exception("DelegateMethod is null");
		}
		virtual bool operator==(IDelegate<R, P> const& other) const STDUTILS_OVERRIDE
		{
			DelegateMethod<C, R, P> const* dm = dynamic_cast<DelegateMethod<C, R, P> const*>(&other);
			return dm && dm->instance == this->instance && dm->method == this->method;
		}
		virtual bool operator!=(IDelegate<R, P> const& other) const STDUTILS_OVERRIDE
		{
			return !this->operator==(other);
		}

	public:
		IDelegate<R, P>* copy() const STDUTILS_OVERRIDE
		{
			return new DelegateMethod(*this);
		}

		bool isNull() const STDUTILS_OVERRIDE
		{
			return this->method == NULL;
		}
	};
	template<typename C, typename P>
	class DelegateMethod<C, void, P> : public IDelegate<void, P>
	{
	public:
		typedef void (C::* Method)(P);

	private:
		C* instance;
		Method method;

	public:
		DelegateMethod(C& instance, Method method)
			:instance(&instance),
			method(method)
		{
		}

		DelegateMethod()
			:instance(NULL),
			method(NULL)
		{
		}

		DelegateMethod(DelegateMethod const& original)
			:instance(original.instance),
			method(original.method)
		{
		}

		virtual ~DelegateMethod() {}

	public:
		DelegateMethod& operator=(DelegateMethod const& original)
		{
			this->instance = original.instance;
			this->method = original.method;

			return *this;
		}

		void operator()(P parameter) const STDUTILS_OVERRIDE
		{
			if (this->method)
				(instance->*method)(parameter);
			else
				throw Exception("DelegateMethod is null");
		}
		virtual bool operator==(IDelegate<void, P> const& other) const STDUTILS_OVERRIDE
		{
			DelegateMethod<C, void, P> const* dm = dynamic_cast<DelegateMethod<C, void, P> const*>(&other);
			return dm && dm->instance == this->instance && dm->method == this->method;
		}
		virtual bool operator!=(IDelegate<void, P> const& other) const STDUTILS_OVERRIDE
		{
			return !this->operator==(other);
		}

	public:
		IDelegate<void, P>* copy() const STDUTILS_OVERRIDE
		{
			return new DelegateMethod(*this);
		}

		bool isNull() const STDUTILS_OVERRIDE
		{
			return this->method == NULL;
		}
	};

	template<typename C, typename R>
	class DelegateMethod<C, R, void> : public IDelegate<R, void>
	{
	public:
		typedef R (C::* Method)(void);

	private:
		C* instance;
		Method method;

	public:
		DelegateMethod(C& instance, Method method)
			:instance(&instance),
			method(method)
		{
		}

		DelegateMethod()
			:instance(NULL),
			method(NULL)
		{
		}

		DelegateMethod(DelegateMethod const& original)
			:instance(original.instance),
			method(original.method)
		{
		}

		virtual ~DelegateMethod() {}

	public:
		DelegateMethod& operator=(DelegateMethod const& original)
		{
			this->instance = original.instance;
			this->method = original.method;

			return *this;
		}

		R operator()() const STDUTILS_OVERRIDE
		{
			if (this->method)
				return (instance->*method)();
			else
				throw Exception("DelegateMethod is null");
		}
		virtual bool operator==(IDelegate<R, void> const& other) const STDUTILS_OVERRIDE
		{
			DelegateMethod<C, R, void> const* dm = dynamic_cast<DelegateMethod<C, R, void> const*>(&other);
			return dm && dm->instance == this->instance && dm->method == this->method;
		}
		virtual bool operator!=(IDelegate<R, void> const& other) const STDUTILS_OVERRIDE
		{
			return !this->operator==(other);
		}

	public:
		IDelegate<R, void>* copy() const STDUTILS_OVERRIDE
		{
			return new DelegateMethod(*this);
		}

		bool isNull() const STDUTILS_OVERRIDE
		{
			return this->method == NULL;
		}
	};

	template<typename C>
	class DelegateMethod<C, void, void> : public IDelegate<void, void>
	{
	public:
		typedef void (C::* Method)(void);

	private:
		C* instance;
		Method method;

	public:
		DelegateMethod(C& instance, Method method)
			:instance(&instance),
			method(method)
		{
		}

		DelegateMethod()
			:instance(NULL),
			method(NULL)
		{
		}

		DelegateMethod(DelegateMethod const& original)
			:instance(original.instance),
			method(original.method)
		{
		}

		virtual ~DelegateMethod() {}

	public:
		DelegateMethod& operator=(DelegateMethod const& original)
		{
			this->instance = original.instance;
			this->method = original.method;

			return *this;
		}

		void operator()() const STDUTILS_OVERRIDE
		{
			if (this->method)
				(instance->*method)();
			else
				throw Exception("DelegateMethod is null");
		}
		virtual bool operator==(IDelegate<void, void> const& other) const STDUTILS_OVERRIDE
		{
			DelegateMethod<C, void, void> const* dm = dynamic_cast<DelegateMethod<C, void, void> const*>(&other);
			return dm && dm->instance == this->instance && dm->method == this->method;
		}
		virtual bool operator!=(IDelegate<void, void> const& other) const STDUTILS_OVERRIDE
		{
			return !this->operator==(other);
		}

	public:
		IDelegate<void, void>* copy() const STDUTILS_OVERRIDE
		{
			return new DelegateMethod<C, void, void>(*this);
		}

		bool isNull() const STDUTILS_OVERRIDE
		{
			return this->method == NULL;
		}
	};

	template<typename R, typename P>
	class DelegateFunction : public IDelegate<R, P>
	{
	public:
		typedef R (* Function)(P);
	private:
		Function function;

	public:
		DelegateFunction(Function function)
			:function(function)
		{
		}

		DelegateFunction()
			:function(NULL)
		{
		}

		DelegateFunction(DelegateFunction const& original)
			:function(original.function)
		{
		}

		virtual ~DelegateFunction() {}

	public:
		DelegateFunction& operator=(DelegateFunction const& original)
		{
			this->function = original.function;
			return *this;
		}
		R operator()(P parameter) const STDUTILS_OVERRIDE
		{
			if (this->function)
				return function(parameter);
			else
				throw Exception("DelegateFunction is null");
		}
		virtual bool operator==(IDelegate<R, P> const& other) const STDUTILS_OVERRIDE
		{
			DelegateFunction<R, P> const* dm = dynamic_cast<DelegateFunction<R, P> const*>(&other);
			return dm && dm->function == this->function;
		}
		virtual bool operator!=(IDelegate<R, P> const& other) const STDUTILS_OVERRIDE
		{
			return !this->operator==(other);
		}

	public:
		IDelegate<R, P>* copy() const STDUTILS_OVERRIDE
		{
			return new DelegateFunction<R, P>(*this);
		}

		bool isNull() const STDUTILS_OVERRIDE
		{
			return this->function == NULL;
		}
	};
	template<typename P>
	class DelegateFunction<void, P> : public IDelegate<void, P>
	{
	public:
		typedef void (* Function)(P);
	private:
		Function function;

	public:
		DelegateFunction(Function function)
			:function(function)
		{
		}

		DelegateFunction()
			:function(NULL)
		{
		}

		DelegateFunction(DelegateFunction const& original)
			:function(original.function)
		{
		}

		virtual ~DelegateFunction() {}

	public:
		DelegateFunction& operator=(DelegateFunction const& original)
		{
			this->function = original.function;
			return *this;
		}
		void operator()(P parameter) const STDUTILS_OVERRIDE
		{
			if (this->function)
				function(parameter);
			else
				throw Exception("DelegateFunction is null");
		}
		virtual bool operator==(IDelegate<void, P> const& other) const STDUTILS_OVERRIDE
		{
			DelegateFunction<void, P> const* dm = dynamic_cast<DelegateFunction<void, P> const*>(&other);
			return dm && dm->function == this->function;
		}
		virtual bool operator!=(IDelegate<void, P> const& other) const STDUTILS_OVERRIDE
		{
			return !this->operator==(other);
		}

	public:
		IDelegate<void, P>* copy() const STDUTILS_OVERRIDE
		{
			return new DelegateFunction(*this);
		}

		bool isNull() const STDUTILS_OVERRIDE
		{
			return this->function == NULL;
		}
	};

	template<typename R>
	class DelegateFunction<R, void> : public IDelegate<R, void>
	{
	public:
		typedef R (* Function)(void);
	private:
		Function function;

	public:
		DelegateFunction(Function function)
			:function(function)
		{
		}

		DelegateFunction()
			:function(NULL)
		{
		}

		DelegateFunction(DelegateFunction const& original)
			:function(original.function)
		{
		}

		virtual ~DelegateFunction() {}

	public:
		DelegateFunction& operator=(DelegateFunction const& original)
		{
			this->function = original.function;

			return *this;
		}
		R operator()() const STDUTILS_OVERRIDE
		{
			if (this->function)
				return function();
			else
				throw Exception("DelegateFunction is null");
		}
		virtual bool operator==(IDelegate<R, void> const& other) const STDUTILS_OVERRIDE
		{
			DelegateFunction<R, void> const* dm = dynamic_cast<DelegateFunction<R,void> const*>(&other);
			return dm && dm->function == this->function;
		}
		virtual bool operator!=(IDelegate<R, void> const& other) const STDUTILS_OVERRIDE
		{
			return !this->operator==(other);
		}

	public:
		IDelegate<R, void>* copy() const STDUTILS_OVERRIDE
		{
			return new DelegateFunction(*this);
		}

		bool isNull() const STDUTILS_OVERRIDE
		{
			return this->function == NULL;
		}
	};

	template<>
	class DelegateFunction<void, void> : public IDelegate<void, void>
	{
	public:
		typedef void (* Function)(void);
	private:
		Function function;

	public:
		DelegateFunction(Function function)
			:function(function)
		{
		}

		DelegateFunction()
			:function(NULL)
		{
		}

		DelegateFunction(DelegateFunction const& original)
			:function(original.function)
		{
		}

		virtual ~DelegateFunction() {}

	public:
		DelegateFunction& operator=(DelegateFunction const& original)
		{
			this->function = original.function;
			return *this;
		}
		void operator()() const STDUTILS_OVERRIDE
		{
			if (this->function)
				function();
			else
				throw Exception("DelegateFunction is null");
		}
		virtual bool operator==(IDelegate<void, void> const& other) const STDUTILS_OVERRIDE
		{
			DelegateFunction<void, void> const* dm = dynamic_cast<DelegateFunction<void, void> const*>(&other);
			return dm && dm->function == this->function;
		}
		virtual bool operator!=(IDelegate<void, void> const& other) const STDUTILS_OVERRIDE
		{
			return !this->operator==(other);
		}

	public:
		IDelegate<void,void>* copy() const STDUTILS_OVERRIDE
		{
			return new DelegateFunction(*this);
		}

		bool isNull() const STDUTILS_OVERRIDE
		{
			return this->function == NULL;
		}
	};

	template<typename R, typename P>
	class STDUTILS_DLLCONTENT Delegate : public IDelegate<R, P>
	{
	private:
		IDelegate<R, P>* delegat;

	public:
		Delegate()
			:delegat(NULL)
		{
		}

		Delegate(IDelegate<R, P> const& original)
			:delegat(original.copy())
		{
		}

		template<class C>
		Delegate(DelegateMethod<C, R, P> const& original)
			:delegat(new DelegateMethod<C, R, P>(original))
		{
		}

		Delegate(DelegateFunction<R, P> const& original)
			:delegat(new DelegateFunction<R, P>(original))
		{
		}

		Delegate(Delegate const& original)
			:delegat(NULL)
		{
			if (original.delegat)
				delegat = original.delegat->copy();
		}
		virtual ~Delegate()
		{
			destroy();
		}


	public:
		Delegate& operator=(Delegate const& original)
		{
			destroy();

			if (original.delegat)
				delegat = original.delegat->copy();

			return *this;
		}

		template<class C>
		Delegate& operator=(DelegateMethod<C, R, P> const& original)
		{
			destroy();

			delegat = new DelegateMethod<C, R, P>(original);

			return *this;
		}

		Delegate& operator=(DelegateFunction<R, P> const& original)
		{
			destroy();

			delegat = new DelegateFunction<R, P>(original);

			return *this;
		}

		R operator()(P parameter) const STDUTILS_OVERRIDE
		{
			if (delegat)
				return delegat->operator()(parameter);
			else
				throw Exception("Delegate does not have value");
		}
		bool operator==(IDelegate<R, P> const& other) const STDUTILS_OVERRIDE
		{
			return this->delegat->operator==(other);
		}
		bool operator!=(IDelegate<R, P> const& other) const STDUTILS_OVERRIDE
		{
			return this->delegat->operator!=(other);
		}

	private:
		void destroy()
		{
			if (delegat)
			{
				delete delegat;
				delegat = NULL;
			}
		}

	public:
		IDelegate<R, P>* copy() const STDUTILS_OVERRIDE
		{
			return new Delegate(*this);
		}


		bool isNull() const STDUTILS_OVERRIDE
		{
			return this->delegat == NULL || this->delegat->isNull();
		}
	};
	template<typename P>
	class Delegate<void, P> : public IDelegate<void, P>
	{
	private:
		IDelegate<void, P>* delegat;

	public:
		Delegate()
			:delegat(NULL)
		{
		}

		Delegate(IDelegate<void, P> const& original)
			:delegat(original.copy())
		{
		}

		template<class C>
		Delegate(DelegateMethod<C, void, P> const& original)
			:delegat(new DelegateMethod<C, void, P>(original))
		{
		}

		Delegate(DelegateFunction<void, P> const& original)
			:delegat(new DelegateFunction<void, P>(original))
		{
		}

		Delegate(Delegate const& original)
			:delegat(NULL)
		{
			if (original.delegat)
				delegat = original.delegat->copy();
		}
		virtual ~Delegate()
		{
			destroy();
		}


	public:
		Delegate& operator=(Delegate const& original)
		{
			destroy();

			if (original.delegat)
				delegat = original.delegat->copy();

			return *this;
		}

		template<class C>
		Delegate& operator=(DelegateMethod<C, void, P> const& original)
		{
			destroy();

			delegat = new DelegateMethod<C, void, P>(original);

			return *this;
		}

		Delegate& operator=(DelegateFunction<void, P> const& original)
		{
			destroy();

			delegat = new DelegateFunction<void, P>(original);

			return *this;
		}

		void operator()(P parameter) const STDUTILS_OVERRIDE
		{
			if (delegat)
				delegat->operator()(parameter);
			else
				throw Exception("Delegate does not have value");
		}
		bool operator==(IDelegate<void, P> const& other) const STDUTILS_OVERRIDE
		{
			return this->delegat->operator==(other);
		}
		bool operator!=(IDelegate<void, P> const& other) const STDUTILS_OVERRIDE
		{
			return this->delegat->operator!=(other);
		}

	private:
		void destroy()
		{
			if (delegat)
			{
				delete delegat;
				delegat = NULL;
			}
		}

	public:
		IDelegate<void, P>* copy() const STDUTILS_OVERRIDE
		{
			return new Delegate(*this);
		}


		bool isNull() const STDUTILS_OVERRIDE
		{
			return this->delegat == NULL || this->delegat->isNull();
		}
	};

	template<typename R>
	class Delegate<R, void> : public IDelegate<R, void>
	{
	private:
		IDelegate<R, void>* delegat;

	public:
		Delegate()
			:delegat(NULL)
		{
		}

		Delegate(IDelegate<R, void> const& original)
			:delegat(original.copy())
		{
		}

		template<class C>
		Delegate(DelegateMethod<C, R, void> const& original)
			:delegat(new DelegateMethod<C, R, void>(original))
		{
		}

		Delegate(DelegateFunction<R, void> const& original)
			:delegat(new DelegateFunction<R, void>(original))
		{
		}

		Delegate(Delegate const& original)
			:delegat(NULL)
		{
			if (original.delegat)
				delegat = original.delegat->copy();
		}
		virtual ~Delegate()
		{
			destroy();
		}


	public:
		Delegate& operator=(Delegate const& original)
		{
			destroy();

			if (original.delegat)
				delegat = original.delegat->copy();

			return *this;
		}

		template<class C>
		Delegate& operator=(DelegateMethod<C, R, void> const& original)
		{
			destroy();

			delegat = new DelegateMethod<C, R, void>(original);

			return *this;
		}

		Delegate& operator=(DelegateFunction<R, void> const& original)
		{
			destroy();

			delegat = new DelegateFunction<R, void>(original);

			return *this;
		}

		R operator()() const STDUTILS_OVERRIDE
		{
			if (delegat)
				return delegat->operator()();
			else
				throw Exception("Delegate does not have value");
		}
		bool operator==(IDelegate<R, void> const& other) const STDUTILS_OVERRIDE
		{
			return this->delegat->operator==(other);
		}
		bool operator!=(IDelegate<R, void> const& other) const STDUTILS_OVERRIDE
		{
			return this->delegat->operator!=(other);
		}

	private:
		void destroy()
		{
			if (delegat)
			{
				delete delegat;
				delegat = NULL;
			}
		}

	public:
		IDelegate<R, void>* copy() const STDUTILS_OVERRIDE
		{
			return new Delegate(*this);
		}


		bool isNull() const STDUTILS_OVERRIDE
		{
			return this->delegat == NULL || this->delegat->isNull();
		}
	};

	template<>
	class Delegate<void, void> : public IDelegate<void, void>
	{
	private:
		IDelegate<void, void>* delegat;

	public:
		Delegate()
			:delegat(NULL)
		{
		}

		Delegate(IDelegate<void, void> const& original)
			:delegat(original.copy())
		{
		}

		template<class C>
		Delegate(DelegateMethod<C, void, void> const& original)
			:delegat(new DelegateMethod<C, void, void>(original))
		{
		}

		Delegate(DelegateFunction<void, void> const& original)
			:delegat(new DelegateFunction<void, void>(original))
		{
		}

		Delegate(Delegate const& original)
			:delegat(NULL)
		{
			if (original.delegat)
				delegat = original.delegat->copy();
		}
		virtual ~Delegate()
		{
			destroy();
		}


	public:
		Delegate& operator=(Delegate const& original)
		{
			destroy();

			if (original.delegat)
				delegat = original.delegat->copy();

			return *this;
		}

		template<class C>
		Delegate& operator=(DelegateMethod<C, void, void> const& original)
		{
			destroy();

			delegat = new DelegateMethod<C, void, void>(original);

			return *this;
		}

		Delegate& operator=(DelegateFunction<void, void> const& original)
		{
			destroy();

			delegat = new DelegateFunction<void, void>(original);

			return *this;
		}

		void operator()() const STDUTILS_OVERRIDE
		{
			if (delegat)
				delegat->operator()();
			else
				throw Exception("Delegate does not have value");
		}
		bool operator==(IDelegate<void, void> const& other) const STDUTILS_OVERRIDE
		{
			return this->delegat->operator==(other);
		}
		bool operator!=(IDelegate<void, void> const& other) const STDUTILS_OVERRIDE
		{
			return this->delegat->operator!=(other);
		}

	private:
		void destroy()
		{
			if (delegat)
			{
				delete delegat;
				delegat = NULL;
			}
		}

	public:
		IDelegate<void, void>* copy() const STDUTILS_OVERRIDE
		{
			return new Delegate(*this);
		}


		bool isNull() const STDUTILS_OVERRIDE
		{
			return this->delegat == NULL || this->delegat->isNull();
		}
	};
}

#endif
