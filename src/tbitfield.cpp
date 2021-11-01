// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include "tbitfield.h"
#include <exception>
#include <type_traits>
#include <cstddef>
#include <string>
#include <algorithm>
#include <utility>

#pragma warning(disable:26409)
#pragma warning(disable:26481)
#pragma warning(disable:26401)

template <typename T>
inline constexpr static std::size_t _bits_to_size(std::size_t val)
{
	return (val + 8 * sizeof(T) - 1) / (8 * sizeof(T));
}

TBitField::TBitField(int len)
	: BitLen(len)
	, pMem(new std::remove_pointer_t<decltype(pMem)>[_bits_to_size<std::remove_pointer<decltype(pMem)>>(len)]{})
	, MemLen(_bits_to_size<std::remove_pointer_t<decltype(pMem)>>(len))
{
	if (len < 0) {
		delete[] pMem;
		throw std::logic_error("negative size...");
	}
}

TBitField::TBitField(const TBitField& bf) // конструктор копирования
	: BitLen(bf.BitLen)
	, pMem(new std::remove_pointer_t<decltype(pMem)>[bf.MemLen])
	, MemLen(bf.MemLen)
{
	for (size_t i = 0; i < this->MemLen; i++)
	{
		this->pMem[i] = bf.pMem[i];
	}
}

TBitField::~TBitField()
{
	delete[] this->pMem;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
{
	return n / (8 * sizeof(std::remove_pointer_t<decltype(pMem)>));
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{
	return std::remove_pointer_t<decltype(pMem)>{ 1 } << (n % (8 * sizeof(std::remove_pointer_t<decltype(pMem)>)));
}

// доступ к битам битового поля

int TBitField::GetLength() const // получить длину (к-во битов)
{
	return this->BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
	if (n < 0 || n >= this->BitLen) {
		throw std::out_of_range("invalid arg");
	}

	this->pMem[this->GetMemIndex(n)] |= this->GetMemMask(n);
}

void TBitField::ClrBit(const int n) // очистить бит
{
	if (n < 0 || n >= this->BitLen) {
		throw std::out_of_range("invalid arg");
	}
	
	this->pMem[this->GetMemIndex(n)] &= ~this->GetMemMask(n);
}

int TBitField::GetBit(const int n) const // получить значение бита
{
	if (n < 0 || n >= this->BitLen) {
		throw std::out_of_range("invalid arg");
	}

	return bool(this->pMem[this->GetMemIndex(n)] & this->GetMemMask(n));
}

// битовые операции
#pragma warning(push)
#pragma warning(disable:26440)
#pragma warning(disable:26457)
TBitField& TBitField::operator=(const TBitField& bf) // присваивание
{
	if (this == &bf) return *this;

	if (this->MemLen != bf.MemLen) {
		this->~TBitField();
		new (this) TBitField(bf);
	}
	else {
		this->BitLen = bf.BitLen;
		for (size_t i = 0; i < this->MemLen; i++)
		{
			this->pMem[i] = bf.pMem[i];
		}
	}

	return *this;
}
#pragma warning(pop)

int TBitField::operator==(const TBitField& bf) const // сравнение
{
	if (this == &bf) return true;
	if (this->BitLen != bf.BitLen) return false;

	for (std::size_t i = 0; i < this->MemLen; i++)
	{
		if (this->pMem[i] != bf.pMem[i]) return false;
	}

	return true;
}

int TBitField::operator!=(const TBitField& bf) const // сравнение
{
	return !(*this == bf);
}

TBitField TBitField::operator|(const TBitField& bf) // операция "или"
{
	if (this->BitLen != bf.BitLen) {
		try {
			throw std::logic_error("different length of bit fields");
		}
		catch (...)
		{}

		TBitField temp(std::max(this->BitLen, bf.BitLen));
		for (size_t i = 0; i < this->MemLen && i < bf.MemLen; i++)
		{
			temp.pMem[i] = this->pMem[i];
		}

		this->~TBitField();
		new (this) TBitField(std::move(temp));
	}

	TBitField temp(*this);
	for (size_t i = 0; i < this->MemLen; i++)
	{
		temp.pMem[i] |= bf.pMem[i];
	}

	return temp;
}

TBitField TBitField::operator&(const TBitField& bf) // операция "и"
{
	if (this->BitLen != bf.BitLen) {
		try {
			throw std::logic_error("different length of bit fields");
		}
		catch (...)
		{}

		TBitField temp(std::max(this->BitLen, bf.BitLen));
		for (size_t i = 0; i < this->MemLen && i < bf.MemLen; i++)
		{
			temp.pMem[i] = this->pMem[i];
		}

		this->~TBitField();
		new (this) TBitField(std::move(temp));
	}

	TBitField temp(*this);
	for (size_t i = 0; i < temp.MemLen; i++)
	{
		temp.pMem[i] &= bf.pMem[i];
	}

	return temp;
}

TBitField TBitField::operator~() // отрицание
{
	TBitField temp(*this);
	for (size_t i = 0; i < temp.MemLen; i++)
	{
		temp.pMem[i] = ~temp.pMem[i];
	}
	temp.pMem[temp.MemLen - 1] &= std::remove_pointer_t<decltype(pMem)>(-1) >> 
		(8 * sizeof(std::remove_pointer_t<decltype(pMem)>) - temp.BitLen % (8 * sizeof(std::remove_pointer_t<decltype(pMem)>)));

	return temp;
}

// ввод/вывод

#pragma warning(disable:26496)
#pragma warning(disable:26457)
#pragma warning(disable:26446)
istream& operator>>(istream& istr, TBitField& bf) // ввод
{
	std::basic_string<istream::char_type> input_data;
	istr >> input_data;

	size_t nsize = input_data.size();

	if (bf.MemLen < nsize) {
		bf.~TBitField();
		new (&bf) TBitField(nsize);
	}

	for (size_t i = 0; i < nsize; i++)
	{
		if (input_data[i] - '0' == 1) {
			bf.SetBit(i);
		}
		else {
			bf.ClrBit(i);
		}
	}
	for (size_t i = nsize; i < bf.BitLen; i++)
	{
		bf.ClrBit(i);
	}

	bf.BitLen = nsize;

	return istr;
}

ostream& operator<<(ostream& ostr, const TBitField& bf) // вывод
{
	for (size_t i = 0; i < bf.BitLen; i++)
	{
		ostr << bf.GetBit(i);
	}

	return ostr;
}
