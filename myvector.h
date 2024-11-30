#pragma once
#include <memory> 
#include <iostream>
#include <initializer_list>
#include <stdexcept>

constexpr int START_CAPACITY = 8;

template <typename T, typename Allocator = std::allocator<T>>
class MyVector {
public:
	MyVector() noexcept;
	MyVector(std::initializer_list<T> init);
	~MyVector() noexcept;

	MyVector(const MyVector& other);
	MyVector(MyVector&& other) noexcept;

	MyVector& operator=(const MyVector& other);
	MyVector& operator=(MyVector&& other) noexcept;

	T& operator[](size_t index);        
	const T& operator[](size_t index) const;  

	size_t size() const noexcept;       
	bool empty() const noexcept; 
	size_t capacity() const noexcept;

	void push_back(const T& value);
	void push_back(T&& value);
	void reserve(size_t new_cap);
	void shrink_to_fit();
	void clear();

	template <typename... Args>
	void emplace_back(Args&&... args);

private:
	T* m_data;			// указатель на массив данных
	size_t m_size;		// текущий размер вектора
	size_t m_capacity;	// емкость вектора
	Allocator m_allocator;// аллокатор для управления памятью

	using Traits = std::allocator_traits<Allocator>; // Определение типа Traits

	void reallocate(size_t new_capacity); // Вспомогательный метод для перевыделения памяти
	
};

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline MyVector<T, Allocator>::MyVector() noexcept 
	: m_data(Traits::allocate(m_allocator, START_CAPACITY)), m_size(0), m_capacity(START_CAPACITY)
{

}

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline MyVector<T, Allocator>::MyVector(std::initializer_list<T> init) :
	m_data(Traits::allocate(m_allocator, init.size())), m_size(init.size()), m_capacity(init.size())
{
	size_t i = 0;
	try 
	{
		for (const T& value : init) 
		{
			Traits::construct(m_allocator, &m_data[i++], value);
		}
	}
	catch (...) 
	{
		for (size_t j = 0; j < i; ++j) 
		{
			Traits::destroy(m_allocator, &m_data[j]);
		}
		Traits::deallocate(m_allocator, m_data, m_capacity);
		throw;
	}
}

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline MyVector<T, Allocator>::~MyVector() noexcept
{
	this->clear();
}

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline MyVector<T, Allocator>::MyVector(const MyVector& other) :
	m_data(Traits::allocate(m_allocator, other.m_capacity)),
	m_capacity(other.m_capacity),
	m_size(0)
{
	try 
	{
		for (size_t i = 0; i < other.m_size; ++i) 
		{
			Traits::construct(m_allocator, &m_data[i], other.m_data[i]);
			++m_size;
		}
	}
	catch (...) 
	{
		this->clear();
		throw;
	}
}

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline MyVector<T, Allocator>::MyVector(MyVector&& other) noexcept :
	m_data(other.m_data),
	m_size(other.m_size),
	m_capacity(other.m_capacity),
	m_allocator(std::move(other.m_allocator))
{
	other.m_data = nullptr;
	other.m_size = 0;
	other.m_capacity = 0;
}

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline MyVector<T, Allocator>& MyVector<T, Allocator>::operator=(const MyVector& other)
{
	if (this != &other) 
	{
		this->clear();

		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_allocator = Traits::select_on_container_copy_construction(other.m_allocator);

		if (m_size > 0) 
		{
			m_data = Traits::allocate(m_allocator, m_capacity);

			try 
			{
				for (size_t i = 0; i < m_size; ++i) 
				{
					Traits::construct(m_allocator, &m_data[i], other.m_data[i]);
				}
			}
			catch (...) 
			{
				this->clear();
				throw;
			}
		}
		else 
		{
			m_data = nullptr;
		}
	}
	return *this;
}

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline MyVector<T, Allocator>& MyVector<T, Allocator>::operator=(MyVector&& other) noexcept
{
	if (this != &other) 
	{
		this->clear();

		m_data = other.m_data;
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		m_allocator = std::move(other.m_allocator);

		other.m_data = nullptr;
		other.m_size = 0;
		other.m_capacity = 0;
	}
	return *this;
}

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline T& MyVector<T, Allocator>::operator[](size_t index)
{
	if (index > m_size)
	{
		throw std::out_of_range("Index out of range");
	}

	return m_data[index];
}

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline const T& MyVector<T, Allocator>::operator[](size_t index) const
{
	if (index > m_size)
	{
		throw std::out_of_range("Index out of range");
	}

	return m_data[index];
}

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline size_t MyVector<T, Allocator>::size() const noexcept
{
	return m_size;
}

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline bool MyVector<T, Allocator>::empty() const noexcept
{
	return m_size == 0;
}

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline size_t MyVector<T, Allocator>::capacity() const noexcept
{
	return m_capacity;
}

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline void MyVector<T, Allocator>::push_back(const T& value)
{
	if (m_size == m_capacity)
	{
		this->reallocate(m_capacity == 0 ? 1 : m_capacity * 2);
	}

	Traits::construct(m_allocator, &m_data[m_size], value);
	++m_size;
}

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline void MyVector<T, Allocator>::push_back(T&& value)
{
	if (m_size == m_capacity)
	{
		this->reallocate(m_capacity == 0 ? 1 : m_capacity * 2);
	}

	Traits::construct(m_allocator, &m_data[m_size], std::move(value));
	++m_size;
}

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline void MyVector<T, Allocator>::reserve(size_t new_cap)
{
	if (new_cap > m_capacity)
	{
		this->reallocate(new_cap);
	}
}

//------------------------------------------------------------------------------

template<typename T, typename Allocator>
inline void MyVector<T, Allocator>::shrink_to_fit()
{
	if (m_capacity > m_size)
	{
		this->reallocate(m_size);
	}
}

//------------------------------------------------------------------------------

// Вспомогательный метод для перевыделения памяти
template<typename T, typename Allocator>
inline void MyVector<T, Allocator>::reallocate(size_t new_capacity)
{
	T* new_data = Traits::allocate(m_allocator, new_capacity);

	for (size_t i = 0; i < m_size; ++i) 
	{
		Traits::construct(m_allocator, &new_data[i], std::move(m_data[i])); // Перемещаем элементы
		Traits::destroy(m_allocator, &m_data[i]);
	}

	Traits::deallocate(m_allocator, m_data, m_capacity);
	m_data = new_data;
	m_capacity = new_capacity;
}

//------------------------------------------------------------------------------

// Вспомогательный метод для очистки вектора
template<typename T, typename Allocator>
inline void MyVector<T, Allocator>::clear()
{
	for (size_t i = 0; i < m_size; ++i) 
	{
		Traits::destroy(m_allocator, &m_data[i]);
	}

	if (m_data) 
	{
		Traits::deallocate(m_allocator, m_data, m_capacity);
		m_data = nullptr;
	}
	m_size = 0;
}

//------------------------------------------------------------------------------


template<typename T, typename Allocator>
template<typename ...Args>
inline void MyVector<T, Allocator>::emplace_back(Args && ...args)
{
	if (m_size == m_capacity)
	{
		this->reallocate(m_capacity == 0 ? 1 : m_capacity * 2);
	}

	// Конструируем объект на месте с использованием переданных аргументов
	Traits::construct(m_allocator, &m_data[m_size], std::forward<Args>(args)...);
	++m_size;
}
