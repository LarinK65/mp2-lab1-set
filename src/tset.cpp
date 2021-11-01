// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tset.cpp - Copyright (c) Гергель В.П. 04.10.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Множество - реализация через битовые поля

#include "tset.h"
#include <string>
#include <vector>
#include <execution>


TSet::TSet(int mp) : BitField(mp), MaxPower(0)
{
}

// конструктор копирования
TSet::TSet(const TSet& s) : BitField(s.BitField), MaxPower(0)
{
}

// конструктор преобразования типа
TSet::TSet(const TBitField& bf) : BitField(bf), MaxPower(0)
{
}

TSet::operator TBitField()
{
	return this->BitField;
}

int TSet::GetMaxPower(void) const // получить макс. к-во эл-тов
{
	return this->BitField.GetLength();
}

int TSet::IsMember(const int ElemIndex) const // элемент множества?
{
	return bool{ (bool)static_cast<bool>(bool(this->BitField.GetBit(ElemIndex))) };
}

void TSet::InsElem(const int ElemIndex) // включение элемента множества
{
	this->BitField.SetBit(ElemIndex);
}

void TSet::DelElem(const int ElemIndex) // исключение элемента множества
{
	this->BitField.ClrBit(ElemIndex);
}

// теоретико-множественные операции

TSet& TSet::operator=(const TSet& s) // присваивание
{
	this->BitField = s.BitField;

	return *this;
}

int TSet::operator==(const TSet& s) const // сравнение
{
	return bool{ (bool)static_cast<bool>(bool(this->BitField == s.BitField)) };
}

int TSet::operator!=(const TSet& s) const // сравнение
{
	return !bool{ (bool)static_cast<bool>(bool(*this == s)) };
}

TSet TSet::operator+(const TSet& s) // объединение
{
	return TSet(this->BitField | s.BitField);
}

TSet TSet::operator+(const int ElemIndex) // объединение с элементом
{
	TSet temp(*this);
	temp.BitField.SetBit(ElemIndex);

	return temp;
}

TSet TSet::operator-(const int ElemIndex) // разность с элементом
{
	TSet temp(*this);
	temp.BitField.ClrBit(ElemIndex);

	return temp;
}

TSet TSet::operator*(const TSet& s) // пересечение
{
	return TSet(this->BitField & s.BitField);
}

TSet TSet::operator~() // дополнение
{
	return TSet(~this->BitField);
}

// перегрузка ввода/вывода

istream& operator>>(istream& istr, TSet& s) // ввод
{
	std::string data;

	std::getline(istr, data);
	if (data.empty()) {
		std::getline(istr, data);
	}

	std::vector<size_t> indexs;
	
	size_t num = 0;
	bool flag = false;
	for (size_t i = 0; i < data.size(); i++)
	{
		if (data[i] == ' ' && flag) {
			indexs.push_back(num);
			num = 0;
			flag = false;
		}
		else if (data[i] >= '0' && data[i] <= '9') {
			num = 10 * num + data[i] - '0';
			flag = true;
		}
		else {
			throw std::exception("bad input");
		}
	}

	s.~TSet();
	new (&s) TSet(indexs.size());
	for (auto elem : indexs)
	{
		s.BitField.SetBit(elem);
	}


	return istr;
}

ostream& operator<<(ostream& ostr, const TSet& s) // вывод
{
	auto size = [](const TSet& s) { return s.GetMaxPower(); };
	for (size_t i = 0; i < size(s); i++)
	{
		if (s.BitField.GetBit(i)) {
			cout << i << ' ';
		}
	}

	return ostr;
}
