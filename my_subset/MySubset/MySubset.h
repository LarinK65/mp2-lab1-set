#pragma once

#include <vector>
#include <list>
#include <memory>
#include <initializer_list>
#include <utility>
#include <type_traits>
#include <set>
#include "MyBitfield.h"

template <typename T, typename BitmaskType>
struct _subset_const_iterator
{
	using value_type = const T;
	using reference = const value_type&;
	using pointer = const T*;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;

	_subset_const_iterator(typename std::list<T>::const_iterator it, const bitfield_arithmetic<BitmaskType>* mask, size_t index) noexcept
		: data_it(it)
		, mask_ptr(mask)
		, elem_index(index)
	{
		while (this->elem_index < this->mask_ptr->size() && !this->mask_ptr->read_bit(this->elem_index))
		{
			++this->data_it;
			++this->elem_index;
		}
	}

	reference operator*() const noexcept
	{
		return *this->data_it;
	}
	pointer operator->() const noexcept
	{
		return this->data_it;
	}

	_subset_const_iterator& operator++() noexcept
	{
		do
		{
			++this->data_it;
			++this->elem_index;

		} while (this->elem_index < this->mask_ptr->size() && !this->mask_ptr->read_bit(this->elem_index));

		return *this;
	}
	_subset_const_iterator operator++(int) noexcept
	{
		auto temp = *this;
		++(*this);
		return temp;
	}
	_subset_const_iterator& operator--() noexcept
	{
		do
		{
			--this->data_it;
			--this->elem_index;

		} while (this->elem_index + 1 > 0 && !this->mask_ptr->read_bit(this->elem_index));

		return *this;
	}
	_subset_const_iterator operator--(int) noexcept
	{
		auto temp = *this;
		--(*this);
		return temp;
	}


	friend bool operator==(const _subset_const_iterator& left, const _subset_const_iterator& right) noexcept
	{
		return 	left.mask_ptr == right.mask_ptr && 
			left.elem_index == right.elem_index;
	}
	friend bool operator!=(const _subset_const_iterator& left, const _subset_const_iterator& right) noexcept
	{
		return !(left == right);
	}


	typename std::list<T>::const_iterator data_it;
	const bitfield_arithmetic<BitmaskType>* mask_ptr = nullptr;
	size_t elem_index = 0;
};

template <typename T, typename BitmaskType>
struct _subset_iterator : _subset_const_iterator<T, BitmaskType>
{
	using Base = _subset_const_iterator<T, BitmaskType>;
	using value_type = T;
	using reference = value_type&;
	using pointer = T*;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;

	using Base::Base;

	reference operator*() const noexcept
	{
		return const_cast<reference>(Base::operator*());
	}
	pointer operator->() const noexcept
	{
		return &(**this);
	}

	_subset_iterator& operator++() noexcept
	{
		Base::operator++();
		return *this;
	}
	_subset_iterator operator++(int) noexcept
	{
		auto temp = *this;
		Base::operator++();
		return temp;
	}
	_subset_iterator& operator--() noexcept
	{
		Base::operator--();
		return *this;
	}
	_subset_iterator operator--(int) noexcept
	{
		auto temp = *this;
		Base::operator--();
		return temp;
	}
};



template <typename T, typename BitmaskType = uint8_t>
class subset
{
public:
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = value_type&;
	using const_reference = const value_type&;
	using iterator = _subset_iterator<T, BitmaskType>;
	using const_iterator = _subset_const_iterator<T, BitmaskType>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	subset()
		: _data_ptr(new std::list<value_type>)
		, _mask()
		, _size(0)
	{}

	template <typename InputIt>
	subset(InputIt first, InputIt last)
		: _data_ptr(new std::list<value_type>)
		, _mask()
		, _size(0)
	{
		while (first != last)
		{
			this->insert(*first);
			++first;
		}
	}

	subset(std::initializer_list<value_type> iList)
		: _data_ptr(new std::list<value_type>)
		, _mask()
		, _size(0)
	{
		for (const auto& x : iList)
		{
			this->insert(x);
		}
	}

	subset(std::shared_ptr<std::list<value_type>> universe_ptr)
		: _data_ptr(universe_ptr)
		, _mask(_data_ptr->size())
		, _size(0)
	{}

	subset(const subset&) = default;
	subset(subset&&) = default;

	subset& operator=(const subset&) = default;
	subset& operator=(subset&&) = default;

private:
	std::pair<typename std::list<T>::iterator, size_t> _find_elem(const T& val) const noexcept
	{
		size_t index = 0;
		auto list_it = this->_data_ptr->begin();

		while (list_it != this->_data_ptr->end())
		{
			if (val == *list_it) {
				break;
			}

			++index;
			++list_it;
		}

		return { list_it, index };
	}

public:
	void insert(const T& val)
	{
		auto el = this->_find_elem(val);

		if (el.first == this->_data_ptr->end()) {
			this->_data_ptr->emplace_back(val);
		}

		this->_sync_data_size();
		this->_size += 1ll - this->_mask.read_bit(el.second);
		this->_mask.set_bit(el.second);
	}
	void insert(T&& val)
	{
		auto el = this->_find_elem(val);

		if (el.first == this->_data_ptr->end()) {
			this->_data_ptr->emplace_back(std::move(val));
		}

		this->_sync_data_size();
		this->_size += 1ll - this->_mask.read_bit(el.second);
		this->_mask.set_bit(el.second);
	}

	template <typename... types>
	void emplace(types&&... args)
	{
		this->_data_ptr->emplace_back(std::forward<types>(args)...);

		auto el = this->_find_elem(*this->_data_ptr->rbegin());

		if (el.first != std::prev(this->_data_ptr->end())) {
			this->_data_ptr->erase(std::prev(this->_data_ptr->end()));
		}

		this->_sync_data_size();
		this->_size += 1 - this->_mask.read_bit(el.second);
		this->_mask.set_bit(el.second);
	}
	
	void erase(const T& value)
	{
		auto el = this->_find_elem(value);

		if (el.second < this->_mask.size()) {
			this->_size -= this->_mask.read_bit(el.second);
			this->_mask.clear_bit(el.second);
		}
	}

	void erase(const_iterator pos)
	{
		this->_size -= this->_mask.read_bit(pos.elem_index);
		this->_mask.clear_bit(pos.elem_index);
	}


	size_t size() const noexcept
	{
		return this->_size;
	}
	size_t universe_size() const noexcept
	{
		return this->_data_ptr->size();
	}
	bool empty() const noexcept
	{
		return this->_size == 0;
	}

	void clear()
	{
		this->_data_ptr.reset(new std::list<value_type>);
		this->_mask.clear();
		this->_size = 0;
	}

	template <typename... types>
	static std::enable_if_t<std::conjunction_v<std::is_same<types, subset<T, BitmaskType>>...>> 
		remove_from_univerce(const T& value, types... objects)
	{
		auto list_ptr = (objects._data_ptr, ...);

		if (((list_ptr != objects._data_ptr) || ...)) {
			throw std::logic_error("different universes of subsets");
		}

		size_t index = 0;
		auto it = list_ptr->begin();
		while (it != list_ptr->end())
		{
			if (*it == value) {
				(objects._sync_data_size(), ...);
				(objects._mask.write_bit(index, objects._mask.read_bit(objects._data_ptr.size() - 1)), ...);

				list_ptr->splice(it, list_ptr, std::prev(list_ptr->end()));
				list_ptr->erase(it);

				break;
			}
		}
	}

	friend bool operator==(const subset& left, const subset& right)
	{
		if (&left == &right) return true;
		if (left.size() != right.size()) return false;

		if (left._data_ptr == right._data_ptr) {
			for (auto lIt = left.begin(), rIt = right.begin(); lIt != left.end(); ++lIt, ++rIt)
			{
				if (*lIt != *rIt) {
					return false;
				}
			}
		}
		else {
			for (const auto& elem : left)
			{
				if (std::find(right.begin(), right.end(), elem) == right.end()) {
					return false;
				}
			}
		}

		return true;
	}
	friend bool operator!=(const subset& left, const subset& right)
	{
		return !(left == right);
	}

	iterator begin() noexcept
	{
		return iterator(this->_data_ptr->begin(), &this->_mask, 0);
	}

	const_iterator begin() const noexcept
	{
		return const_iterator(this->_data_ptr->begin(), &this->_mask, 0);
	}

	iterator end() noexcept
	{
		return iterator(this->_data_ptr->end(), &this->_mask, this->_mask.size());
	}
	
	const_iterator end() const noexcept
	{
		return const_iterator(this->_data_ptr->end(), &this->_mask, this->_mask.size());
	}

	reverse_iterator rbegin() noexcept
	{
		return reverse_iterator(this->end());
	}

	const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(this->end());
	}

	reverse_iterator rend() noexcept
	{
		return reverse_iterator(this->begin());
	}

	const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(this->begin());
	}

private:
	std::shared_ptr<std::list<T>> _data_ptr;
	bitfield_arithmetic<BitmaskType> _mask;
	size_t _size = 0;

	void _sync_data_size()
	{
		if (this->_data_ptr->size() != this->_mask.size()) {
			this->_mask.resize(this->_data_ptr->size());
		}
	}
};
