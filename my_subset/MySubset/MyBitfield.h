#pragma once

#include <utility>
#include <type_traits>
#include <cinttypes>
#include <initializer_list>
#include <stdexcept>
#include <iterator>


template <typename T, std::enable_if_t<std::is_unsigned_v<T>, int> = 0>
struct _bitfield_container
{
	_bitfield_container() = default;
	_bitfield_container(std::size_t sz)
		: _size(sz)
		, _data(new T[_bit_to_size(sz)]{})
	{}
	_bitfield_container(const _bitfield_container& other)
		: _size(other._size)
		, _data(new T[_bit_to_size(other._size)]{})
	{
		for (std::size_t i = 0; i < _bit_to_size(this->_size); i++)
		{
			this->_data[i] = other._data[i];
		}
	}
	_bitfield_container(_bitfield_container&& other) noexcept
		: _size(other._size)
		, _data(other._data)
	{
		other._data = nullptr;
		other._size = 0;
	}

	_bitfield_container& operator=(const _bitfield_container& right)
	{
		if (*this == &right) return *this;

		if (_bit_to_size(this->_size) != _bit_to_size(right._size)) {
			this->clear();
			this->_data = new T[_bit_to_size(right._size)];
		}

		this->_size = right._size;

		for (std::size_t i = 0; i < _bit_to_size(this->_size); i++)
		{
			this->_data[i] = right._data[i];
		}

		return *this;
	}
	_bitfield_container& operator=(_bitfield_container&& right) noexcept
	{
		if (*this == &right) return *this;

		this->clear();

		this->_data = right._data;
		this->_size = right._size;
		right._data = nullptr;
		right._size = 0;

		return *this;
	}

	~_bitfield_container()
	{
		this->clear();
	}

	void clear() noexcept
	{
		delete[] this->_data;
		this->_data = nullptr;
		this->_size = 0;
	}

	size_t size() const noexcept
	{
		return this->_size;
	}
	size_t capacity() const noexcept
	{
		return this->_bit_to_size(this->_size);
	}

	void resize(size_t nsize)
	{
		if (_bit_to_size(this->_size) != _bit_to_size(nsize)) {
			auto temp = new T[_bit_to_size(nsize)]{};
			for (size_t i = 0; i < _bit_to_size(nsize) && i < _bit_to_size(this->_size); i++)
			{
				temp[i] = this->_data[i];
			}

			this->clear();
			this->_data = temp;
		}

		this->_size = nsize;
		
		if (nsize % elem_size != 0) {
			_use_mask_to_last_elem(this->_data[_bit_to_size(nsize) - 1], nsize);
		}
	}

	constexpr static size_t _bit_to_size(size_t count_bit) noexcept
	{
		return (count_bit + elem_size - 1) / elem_size;
	}
	
	static void _use_mask_to_last_elem(T& val, size_t sz) noexcept
	{
		val &= T(-1) >> (elem_size - sz % elem_size) % elem_size;
	}

	std::size_t _size = 0;
	T* _data = nullptr;

	static const std::size_t elem_size = sizeof(T) * 8;
};

template <typename T>
struct _bitfield_proxy;

template <typename T = uint8_t, std::enable_if_t<std::is_unsigned_v<T>, int> = 0>
class bitfield
{
public:
	using reference = _bitfield_proxy<T>;

	bitfield() = default;
	bitfield(size_t size) : _data(size) {}
	bitfield(size_t size, bool value) : _data(size)
	{
		if (value && size > 0) {
			for (size_t i = 0; i < _bitfield_container<T>::_bit_to_size(this->_data._size); i++)
			{
				this->_data._data[i] = -T(value);
			}

			this->_data.resize(size);
		}
	}
	template <typename U, std::enable_if_t<std::is_convertible_v<U, bool>, int> = 0>
	bitfield(std::initializer_list<U> ilist)
		: _data(ilist.size())
	{
		size_t index = 0;
		for (const auto& elem : ilist)
		{
			if (bool(elem)) {
				this->set_bit(index);
			}

			index++;
		}
	}

	template <typename U>
	explicit bitfield(const bitfield<U>& other)
		: _data(other.size())
	{
		for (size_t i = 0; i < this->size(); i++)
		{
			if (other.read_bit(i)) {
				this->set_bit(i);
			}
		}
	}
	
	bitfield(const bitfield& other) = default;
	bitfield(bitfield&& other) = default;

	~bitfield() = default;

	bitfield& operator=(const bitfield& right) = default;
	bitfield& operator=(bitfield&& right) = default;


	void set_bit(size_t index) noexcept
	{
		this->_data._data[_get_elem_pos(index)] |= T(1) << _get_bit_pos(index);
	}
	void clear_bit(size_t index) noexcept
	{
		this->_data._data[_get_elem_pos(index)] &= ~(T(1) << _get_bit_pos(index));
	}
	void write_bit(size_t index, bool value) noexcept
	{
		if (value) {
			this->set_bit(index);
		}
		else {
			this->clear_bit(index);
		}
	}
	bool read_bit(size_t index) const noexcept
	{
		return this->_data._data[_get_elem_pos(index)] & (T(1) << _get_bit_pos(index));
	}


	_bitfield_proxy<T> operator[](size_t index) noexcept
	{
		return _bitfield_proxy<T>(*this, index);
	}
	bool operator[](size_t index) const noexcept
	{
		return this->_data.read_bit(index);
	}

	void clear() noexcept
	{
		this->_data.clear();
	}
	void resize(size_t nsize)
	{
		this->_data.resize(nsize);
	}
	size_t size() const noexcept
	{
		return this->_data.size();
	}
	size_t capacity() const noexcept
	{
		return this->_data.capacity();
	}

	friend bool operator==(const bitfield& left, const bitfield& right)
	{
		if (&left == &right) return true;
		if (left._data._size != right._data._size) return false;

		for (size_t i = 0; i < _bitfield_container<T>::_bit_to_size(left._data._size); i++)
		{
			if (left._data._data[i] != right._data._data[i]) return false;
		}

		return true;
	}
	friend bool operator!=(const bitfield& left, const bitfield& right)
	{
		return !(left == right);
	}

protected:
	_bitfield_container<T> _data;

	static size_t _get_elem_pos(size_t index)
	{
		return index / _bitfield_container<T>::elem_size;
	}
	static size_t _get_bit_pos(size_t index)
	{
		return index % _bitfield_container<T>::elem_size;
	}
};

template <typename T>
struct _bitfield_proxy
{
public:
	_bitfield_proxy(bitfield<T>& field, size_t bit) noexcept
		: bit_pos(bit)
		, field(field)
	{}
	_bitfield_proxy(const _bitfield_proxy&) = default;

	_bitfield_proxy& operator=(bool value) noexcept
	{
		this->field.write_bit(this->bit_pos, value);
		return *this;
	}
	_bitfield_proxy& operator=(const _bitfield_proxy& right)
	{
		*this = static_cast<bool>(right);
		return *this;
	}

	operator bool() const noexcept
	{
		return this->field.read_bit(this->bit_pos);
	}

	friend void swap(_bitfield_proxy<T> left, _bitfield_proxy<T> right) noexcept
	{
		bool temp = left;
		left = right;
		right = temp;
	}

	size_t bit_pos;
	bitfield<T>& field;
};

template <typename T = uint8_t, std::enable_if_t<std::is_unsigned_v<T>, int> = 0>
class bitfield_arithmetic : public bitfield<T>
{
public:
	using bitfield<T>::bitfield;

	bitfield_arithmetic(const bitfield_arithmetic&) = default;
	bitfield_arithmetic(bitfield_arithmetic&&) = default;

	~bitfield_arithmetic() = default;

	bitfield_arithmetic& operator=(const bitfield_arithmetic&) = default;
	bitfield_arithmetic& operator=(bitfield_arithmetic&&) = default;

	// set operations
	bitfield_arithmetic& operator&=(const bitfield_arithmetic& right)
	{
		if (this->_data._size != right._data._size) {
			throw std::logic_error("different lengths of arguments");
		}

		for (size_t i = 0; i < _bitfield_container<T>::_bit_to_size(this->_data._size); i++)
		{
			this->_data._data[i] &= right._data._data[i];
		}

		return *this;
	}
	bitfield_arithmetic& operator|=(const bitfield_arithmetic& right)
	{
		if (this->_data._size != right._data._size) {
			throw std::logic_error("different lengths of arguments");
		}

		for (size_t i = 0; i < _bitfield_container<T>::_bit_to_size(this->_data._size); i++)
		{
			this->_data._data[i] |= right._data._data[i];
		}

		return *this;
	}
	bitfield_arithmetic& operator^=(const bitfield_arithmetic& right)
	{
		if (this->_data._size != right._data._size) {
			throw std::logic_error("different lengths of arguments");
		}

		for (size_t i = 0; i < _bitfield_container<T>::_bit_to_size(this->_data._size); i++)
		{
			this->_data._data[i] ^= right._data._data[i];
		}

		return *this;
	}
	bitfield_arithmetic& operator-=(const bitfield_arithmetic& right)
	{
		if (this->_data._size != right._data._size) {
			throw std::logic_error("different lengths of arguments");
		}

		for (size_t i = 0; i < _bitfield_container<T>::_bit_to_size(this->_data._size); i++)
		{
			this->_data._data[i] &= ~right._data._data[i];
		}

		return *this;
	}

	friend bitfield_arithmetic operator&(bitfield_arithmetic left, const bitfield_arithmetic& right)
	{
		left &= right;
		return left;
	}
	friend bitfield_arithmetic operator|(bitfield_arithmetic left, const bitfield_arithmetic& right)
	{
		left |= right;
		return left;
	}
	friend bitfield_arithmetic operator^(bitfield_arithmetic left, const bitfield_arithmetic& right)
	{
		left ^= right;
		return left;
	}
	friend bitfield_arithmetic operator-(bitfield_arithmetic left, const bitfield_arithmetic& right)
	{
		left -= right;
		return left;
	}
	friend bitfield_arithmetic operator~(bitfield_arithmetic val)
	{
		if (val.capacity() > 0) {
			for (size_t i = 0; i < val.capacity(); i++)
			{
				val._data._data[i] = ~val._data._data[i];
			}
			val.resize(val.size());
		}

		return val;
	}
};


