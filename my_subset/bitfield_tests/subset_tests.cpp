#include "pch.h"
#include "../MySubset/MySubset.h"
#include <set>

TEST(subset, can_construct_default_subset)
{
	ASSERT_NO_THROW(subset<int> {});
}

TEST(subset, default_constructed_subset_has_null_size)
{
	subset<int> s;

	ASSERT_TRUE(s.size() == 0);
}

TEST(subset, default_constructed_subset_is_empty)
{
	subset<int> s;

	ASSERT_TRUE(s.empty());
}

TEST(subset, can_construct_from_two_iterators)
{
	std::set<int> data{ 1, 2, 3, 4, 5, 6, -1, -2, -3 };
	subset<int> s(data.begin(), data.end());

	for (auto x : s)
	{
		auto temp = data.find(x);
		ASSERT_TRUE(temp != data.end());
		data.erase(temp);
	}

	ASSERT_TRUE(data.empty());
}

TEST(subset, values_is_unique_after_construct_with_two_iterators)
{
	std::vector<int> in{ 1, 1, 1, 2, 2, 3, 4, 5, 5, 5, 5, -1, -1, -1 };
	std::set<int> data(in.begin(), in.end());

	subset<int> s(in.begin(), in.end());

	ASSERT_TRUE(s.size() == data.size());
	for (auto x : s)
	{
		auto temp = data.find(x);
		ASSERT_TRUE(temp != data.end());
		data.erase(temp);
	}

	ASSERT_TRUE(data.empty());
}

TEST(subset, can_construct_with_initializer_list)
{
	std::set<int> data{ 1, 2, 3, 4, 5, 6, -1, -2, -3 };
	subset<int> s{ 1, 2, 3, 4, 5, 6, -1, -2, -3 };

	for (auto x : s)
	{
		auto temp = data.find(x);
		ASSERT_TRUE(temp != data.end());
		data.erase(temp);
	}

	ASSERT_TRUE(data.empty());
}

TEST(subset, values_is_unique_after_construct_with_initializer_list)
{
	std::set<int> data{ 1, 1, 1, 2, 2, 3, 4, 5, 5, 5, 5, -1, -1, -1 };;
	subset<int> s{ 1, 1, 1, 2, 2, 3, 4, 5, 5, 5, 5, -1, -1, -1 };;

	ASSERT_TRUE(s.size() == data.size());
	for (auto x : s)
	{
		auto temp = data.find(x);
		ASSERT_TRUE(temp != data.end());
		data.erase(temp);
	}

	ASSERT_TRUE(data.empty());
}

TEST(subset, can_copy_subset)
{
	subset<int> s{ 1, 2, 3, 4, 5 };
	subset<int> s_copy(s);
	std::set<int> data;

	for (auto x : s_copy)
	{
		data.insert(x);
	}

	for (auto x : s)
	{
		auto temp = data.find(x);
		ASSERT_TRUE(temp != data.end());
		data.erase(temp);
	}

	ASSERT_TRUE(data.empty());
}

TEST(subset, subset_is_not_empty_after_insert)
{
	subset<int> s;
	
	s.insert(-1);

	ASSERT_TRUE(!s.empty());
}

TEST(subset, insert_new_value_change_size_of_subset)
{
	int n = 100;
	subset<int> s;

	for (int i = 0; i < n; i++)
	{
		s.insert(i);

		ASSERT_TRUE(s.size() == i + 1);
	}
}

TEST(subset, values_are_in_subset_after_insert)
{
	int n = 100;
	subset<int> s;

	for (int i = 0; i < n; i++)
	{
		s.insert(i);

		ASSERT_TRUE(*s.rbegin() == i);
	}

	int v = 0;
	for (auto it = s.begin(); it != s.end(); ++it, ++v)
	{
		ASSERT_TRUE(*it == v);
	}
}

TEST(subset, insert_existent_value_no_change_size_of_subset)
{
	int n = 100;
	subset<int> s;

	for (int i = 0; i < n; i++)
	{
		s.insert(i);
	}

	for (int i = 0; i < n; i++)
	{
		s.insert(i);
	}

	ASSERT_TRUE(s.size() == n);
}

TEST(subset, can_erase_non_existent_value)
{
	subset<int> s;

	ASSERT_NO_THROW(s.erase(1));
	ASSERT_NO_THROW(s.erase(10));
	ASSERT_NO_THROW(s.erase(5));
}

TEST(subset, erase_existent_value_change_size)
{
	int size = 10;
	subset<int> s;

	for (int i = 0; i < size; i++)
	{
		s.insert(i);
	}

	for (int i = 0; i < size; i += 2)
	{
		s.erase(i);

		ASSERT_TRUE(s.size() == size - i / 2 - 1);
	}
}

TEST(subset, erased_values_are_not_in_subset)
{
	int size = 10;
	subset<int> s;
	std::set<int> ans{ 1, 3, 5, 7, 9 };

	for (int i = 0; i < size; i++)
	{
		s.insert(i);
	}

	for (int i = 0; i < size; i += 2)
	{
		s.erase(i);
	}

	ASSERT_TRUE(s.size() == size / 2);
	for (auto x : s)
	{
		ASSERT_TRUE(ans.find(x) != ans.end());
	}
}

TEST(subset, subset_is_empty_after_erase_all_values)
{
	int size = 100;
	subset<int> s;

	for (int i = 0; i < size; i++)
	{
		s.insert(i);
	}

	ASSERT_TRUE(!s.empty());

	for (int i = 0; i < size; i++)
	{
		s.erase(i);
	}

	ASSERT_TRUE(s.empty());
}

TEST(subset, insert_do_not_change_copies_of_subset)
{
	int size = 10;
	std::set<int> data{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	subset<int> s(data.begin(), data.end());
	subset<int> s_copy(s);

	s.insert(-1);
	ASSERT_TRUE(std::count(s_copy.begin(), s_copy.end(), -1) == 0);

	for (auto x : s_copy)
	{
		auto temp = data.find(x);
		ASSERT_TRUE(temp != data.end());
		data.erase(temp);
	}

	ASSERT_TRUE(data.empty());
}

TEST(subset, erase_do_not_change_copies_of_subset)
{
	int size = 10;
	std::set<int> data{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	subset<int> s(data.begin(), data.end());
	subset<int> s_copy(s);

	s.erase(5);
	ASSERT_TRUE(std::count(s_copy.begin(), s_copy.end(), 5) == 1);

	for (auto x : s_copy)
	{
		auto temp = data.find(x);
		ASSERT_TRUE(temp != data.end());
		data.erase(temp);
	}

	ASSERT_TRUE(data.empty());
}

TEST(subset, can_do_many_inserts_and_erases)
{
	int size = 100;
	subset<int> s;

	for (int i = 0; i < size; i++)
	{
		s.insert(i);
	}

	for (int i = 0; i < size; i++)
	{
		s.erase(i);
		s.insert(-(i % 2) * i);
	}

	for (int i = 1; i < size; i += 2)
	{
		ASSERT_TRUE(std::count(s.begin(), s.end(), -int(i)) == 1);
		s.erase(-int(i));
	}

	ASSERT_TRUE(std::count(s.begin(), s.end(), 0) == 1);
	s.erase(0);

	ASSERT_TRUE(s.empty());
}

TEST(subset, comparison_is_correct_1)
{
	subset<int> a;
	subset<int> b;

	ASSERT_TRUE(a == b);
}

TEST(subset, comparison_is_correct_2)
{
	subset<int> a{ 1, 2, 3, 4 };
	subset<int> b{ 1, 2, 3, 4 };

	ASSERT_TRUE(a == b);
}

TEST(subset, comparison_is_correct_3)
{
	subset<int> a{ 1, 2, 3, 4 };
	subset<int> b(a);

	ASSERT_TRUE(a == b);
}

TEST(subset, comparison_is_correct_4)
{
	subset<int> a{ 1, 2, 3, 4 };
	subset<int> b(a);

	a.erase(2);
	b.erase(2);

	ASSERT_TRUE(a == b);
}

TEST(subset, comparison_is_correct_5)
{
	subset<int> a{ 1, 2, 4 };
	subset<int> b{ 1, 2, 3, 4, 5 };

	a.insert(3);
	b.erase(5);

	ASSERT_TRUE(a == b);
}

TEST(subset, comparison_is_correct_6)
{
	subset<int> a;

	a.insert(3);
	a.insert(10);
	a.insert(15);

	subset<int> b(a);

	ASSERT_TRUE(a == b);

	a.erase(10);
	b.insert(20);
	
	ASSERT_TRUE(a != b);
	
	a.erase(20);
	b.insert(10);

	ASSERT_TRUE(a != b);

	a.insert(20);
	b.erase(10);

	ASSERT_TRUE(a == b);
}

TEST(subset, comparison_is_correct_7)
{
	std::vector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8 , 7, 6, 5, 4, 3, 2, 1 };

	subset<int> s1(v.begin(), v.end());
	subset<int> s2{ 1, 2, 3, 4, 5, 6, 7, 8 };

	ASSERT_TRUE(s1 == s2);
}

TEST(subset, range_based_for)
{
	std::set<int> data{ 1, 2, 3 ,4, 5, 6, 7, 8, 9 };
	subset<int> s(data.begin(), data.end());

	for (auto elem : s)
	{
		ASSERT_TRUE(data.find(elem) != data.end());
		data.erase(elem);
	}

	ASSERT_TRUE(data.empty());
}


