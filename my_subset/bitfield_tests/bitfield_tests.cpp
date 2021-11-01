#include "pch.h"
#include "../MySubset/MyBitfield.h"
#include <cinttypes>

TEST(bitfield, is_correct_element_size)
{
	ASSERT_TRUE(_bitfield_container<uint8_t>::elem_size == 8);
	ASSERT_TRUE(_bitfield_container<uint16_t>::elem_size == 16);
	ASSERT_TRUE(_bitfield_container<uint32_t>::elem_size == 32);
	ASSERT_TRUE(_bitfield_container<uint64_t>::elem_size == 64);
}

TEST(bitfield, nothrow_default_construct)
{
	ASSERT_NO_THROW(bitfield<uint8_t> {});
	ASSERT_NO_THROW(bitfield<uint16_t> {});
	ASSERT_NO_THROW(bitfield<uint32_t> {});
	ASSERT_NO_THROW(bitfield<uint64_t> {});
}

TEST(bitfield, default_constructed_object_has_null_size)
{
	bitfield<uint8_t> a;

	ASSERT_TRUE(a.size() == 0);
}

TEST(bitfield, nothrow_constructor_with_possible_size)
{
	ASSERT_NO_THROW(bitfield<uint8_t> (0));
	ASSERT_NO_THROW(bitfield<uint8_t> (1));
	ASSERT_NO_THROW(bitfield<uint8_t> (7));
	ASSERT_NO_THROW(bitfield<uint8_t> (8));
	ASSERT_NO_THROW(bitfield<uint8_t> (9));
	ASSERT_NO_THROW(bitfield<uint8_t> (10001));
}

TEST(bitfield, bitfield_has_correct_size_after_construct_0)
{
	bitfield<uint8_t> a(0);

	ASSERT_TRUE(a.size() == 0);
}

TEST(bitfield, bitfield_has_correct_size_after_construct_1)
{
	bitfield<uint8_t> a(1);

	ASSERT_TRUE(a.size() == 1);
}

TEST(bitfield, bitfield_has_correct_size_after_construct_7)
{
	bitfield<uint8_t> a(7);

	ASSERT_TRUE(a.size() == 7);
}

TEST(bitfield, bitfield_has_correct_size_after_construct_8)
{
	bitfield<uint8_t> a(8);

	ASSERT_TRUE(a.size() == 8);
}

TEST(bitfield, bitfield_has_correct_size_after_construct_9)
{
	bitfield<uint8_t> a(9);

	ASSERT_TRUE(a.size() == 9);
}

TEST(bitfield, bitfield_has_correct_size_after_construct_10001)
{
	bitfield<uint8_t> a(10001);

	ASSERT_TRUE(a.size() == 10001);
}

TEST(bitfield, capacity_greater_or_equal_minimal_memory_size)
{
	size_t count_bit = 10001;
	
	bitfield<uint8_t> a(count_bit);

	ASSERT_TRUE(a.capacity() * 8 * sizeof(uint8_t) >= count_bit);
}

TEST(bitfield, all_bits_is_null_after_construct)
{
	bitfield<uint8_t> a(10001);

	for (size_t i = 0; i < a.size(); i++)
	{
		ASSERT_TRUE(a.read_bit(i) == 0);
	}
}

TEST(bitfield, all_bits_have_specified_value_after_constructor_with_value_true)
{
	bitfield<uint8_t> a(10001, 1);

	for (size_t i = 0; i < a.size(); i++)
	{
		ASSERT_TRUE(a.read_bit(i) == 1);
	}
}

TEST(bitfield, all_bits_have_specified_value_after_constructor_with_value_false)
{
	bitfield<uint8_t> a(10001, 0);

	for (size_t i = 0; i < a.size(); i++)
	{
		ASSERT_TRUE(a.read_bit(i) == 0);
	}
}

TEST(bitfield, has_correct_size_after_constructor_with_initializer_list)
{
	bitfield<uint8_t> a{ 1, 0, 1, 1, 1, 0 };

	ASSERT_TRUE(a.size() == 6);
}

TEST(bitfield, bits_have_correct_value_after_constructor_with_initializer_list)
{
	bitfield<uint8_t> a{ 1, 0, 1, 1, 1, 0 };

	ASSERT_TRUE(a.read_bit(0) == 1);
	ASSERT_TRUE(a.read_bit(1) == 0);
	ASSERT_TRUE(a.read_bit(2) == 1);
	ASSERT_TRUE(a.read_bit(3) == 1);
	ASSERT_TRUE(a.read_bit(4) == 1);
	ASSERT_TRUE(a.read_bit(5) == 0);
}

TEST(bitfield, is_correct_comparison_1)
{
	bitfield<uint8_t> a{ 1, 0, 1, 1, 1, 0 };
	bitfield<uint8_t> b{ 1, 0, 1, 1, 1, 0 };

	ASSERT_TRUE(a == b);
}

TEST(bitfield, is_correct_comparison_2)
{
	bitfield<uint8_t> a{ 1, 0, 1, 1, 1, 0 };
	bitfield<uint8_t> b{ 1, 1, 1, 1, 1, 0 };

	ASSERT_FALSE(a == b);
}


TEST(bitfield, is_correct_comparison_3)
{
	bitfield<uint8_t> a{ 1, 0, 1, 1, 1, 0 };
	bitfield<uint8_t> b{ 1, 0, 1, 1, 1};

	ASSERT_FALSE(a == b);
}


TEST(bitfield, is_correct_comparison_4)
{
	bitfield<uint8_t> a{ 1, 0, 1, 1, 1, 0 };

	ASSERT_TRUE(a == a);
}


TEST(bitfield, is_correct_comparison_5)
{
	bitfield<uint8_t> a{ 1, 0, 1, 1, 1, 0 };
	bitfield<uint8_t> b(105);

	ASSERT_FALSE(a == b);
}

TEST(bitfield, is_correct_comparison_6)
{
	bitfield<uint8_t> a(10, 1);
	bitfield<uint8_t> b(10, 1);

	ASSERT_TRUE(a == b);
}

TEST(bitfield, is_correct_comparison_7)
{
	bitfield<uint8_t> a(1, 1);
	bitfield<uint8_t> b(1, 0);

	ASSERT_TRUE(a != b);
}

TEST(bitfield, is_correct_comparison_8)
{
	bitfield<uint8_t> a;
	bitfield<uint8_t> b;

	ASSERT_TRUE(a == b);
}

TEST(bitfield, can_set_bit)
{
	bitfield<uint8_t> a(100);

	for (size_t i = 0; i < a.size(); i++)
	{
		a.set_bit(i);
		for (size_t j = 0; j < a.size(); j++)
		{
			if (j == i) {
				ASSERT_TRUE(a.read_bit(j) == 1);
			}
			else {
				ASSERT_TRUE(a.read_bit(j) == 0);
			}
		}

		a.clear_bit(i);
	}
}

TEST(bitfield, can_set_many_bits)
{
	bitfield<uint8_t> a(100);

	for (size_t i = 0; i < a.size(); i += i + 1)
	{
		a.set_bit(i);
	}

	for (size_t i = 0, j = 0; j < a.size(); j++)
	{
		if (j == i) {
			ASSERT_TRUE(a.read_bit(j) == 1);
			i += i + 1;
		}
		else {
			ASSERT_TRUE(a.read_bit(j) == 0);
		}
	}
}

TEST(bitfield, can_set_bit_many_times)
{
	bitfield<uint8_t> a(10);

	a.set_bit(5);
	ASSERT_TRUE(a.read_bit(5));
	a.set_bit(5);
	ASSERT_TRUE(a.read_bit(5));
	a.set_bit(5);
	ASSERT_TRUE(a.read_bit(5));

	a.clear_bit(5);
	ASSERT_FALSE(a.read_bit(5));

	a.set_bit(5);
	ASSERT_TRUE(a.read_bit(5));
	a.set_bit(5);
	ASSERT_TRUE(a.read_bit(5));

	for (size_t i = 0; i < a.size(); i++)
	{
		if (i == 5) {
			ASSERT_TRUE(a.read_bit(i));
		}
		else {
			ASSERT_FALSE(a.read_bit(i));
		}
	}
}

TEST(bitfield, can_convert_bitfields_with_different_types_of_element)
{
	size_t size = 1000;

	bitfield<uint16_t> a(1000);

	for (size_t i = 1; i < size; i <<= 1)
	{
		for (size_t j = 0; i + j < size && j <= i; j++)
		{
			if (i % 2) {
				a.set_bit(i + j);
			}
		}
	}

	bitfield<uint8_t> b(a);

	ASSERT_TRUE(b.size() == a.size());

	for (size_t i = 0; i < a.size(); i++)
	{
		ASSERT_TRUE(a.read_bit(i) == b.read_bit(i));
	}
}

TEST(bitfield, can_be_copied)
{
	bitfield<uint8_t> a(1000);

	for (size_t i = 1; i < a.size(); i <<= 1)
	{
		for (size_t j = 0; i + j < a.size() && j <= i; j++)
		{
			if (i % 2) {
				a.set_bit(i + j);
			}
		}
	}

	bitfield<uint8_t> b(a);

	ASSERT_TRUE(a == b);
}

TEST(bitfield, can_be_moved)
{
	bitfield<uint8_t> a(1005);

	for (size_t i = 1; i < a.size(); i <<= 1)
	{
		for (size_t j = 0; i + j < a.size() && j <= i; j++)
		{
			if (i % 2) {
				a.set_bit(i + j);
			}
		}
	}

	bitfield<uint8_t> c(a);
	bitfield<uint8_t> b(std::move(a));

	ASSERT_TRUE(b == c);
}

TEST(bitfield, is_correct_write_bit)
{
	bitfield<uint8_t> a(1005);
	bitfield<uint8_t> b(1005);

	for (size_t i = 0; i < a.size(); i++)
	{
		a.write_bit(i, i % 2);
		if (i % 2 == 1) {
			b.set_bit(i);
		}
	}

	ASSERT_TRUE(a == b);
}

TEST(bitfield, is_correct_writer_single_bit)
{
	bitfield<uint8_t> a(1005);

	for (size_t i = 0; i < a.size(); i++)
	{
		a.write_bit(i, 0);
		ASSERT_TRUE(a.read_bit(i) == 0);
		a.write_bit(i, 1);
		ASSERT_TRUE(a.read_bit(i) == 1);
		a.write_bit(i, 0);
		ASSERT_TRUE(a.read_bit(i) == 0);
	}
}

TEST(bitfield, result_of_brackets_equal_to_read_bit)
{
	bitfield<uint8_t> a(1005);

	for (size_t i = 1; i < a.size(); i <<= 1)
	{
		for (size_t j = 0; i + j < a.size() && j <= i; j++)
		{
			a.write_bit(i, i % 2);
		}
	}

	for (size_t i = 0; i < a.size(); i++)
	{
		ASSERT_TRUE(a.read_bit(i) == a[i]);
	}
}

TEST(bitfield, can_write_through_bracets)
{
	bitfield<uint8_t> a(1005);

	for (size_t i = 0; i < a.size(); i++)
	{
		a[i] = true;
		for (size_t j = 0; j < a.size(); j++)
		{
			if (i == j) {
				ASSERT_TRUE(a[j] == 1);
			}
			else {
				ASSERT_TRUE(a[j] == 0);
			}
		}

		a[i] = false;
	}
}

TEST(bitfield, can_swap_results_of_bracets)
{
	bitfield<uint8_t> a(106);

	for (size_t i = 0; i * 2 < a.size(); i++)
	{
		a.set_bit(i);
	}

	for (size_t i = 0; i * 2 < a.size(); i += 2)
	{
		swap(a[i], a[a.size() - i - 1]);
	}

	for (size_t i = 0; i < a.size(); i++)
	{
		ASSERT_TRUE(a[i] == i % 2);
	}
}

TEST(bitfield, can_reduce_the_size)
{
	bitfield<uint8_t> a(100);

	for (size_t i = 0; 2 * i < a.size(); i++)
	{
		a.set_bit(i);
	}

	bitfield<uint8_t> b(a);

	a.resize(60);

	ASSERT_TRUE(a.size() == 60);

	for (size_t i = 0; i < a.size(); i++)
	{
		ASSERT_TRUE(a.read_bit(i) == b.read_bit(i));
	}
}

TEST(bitfield, can_increase_the_size)
{
	bitfield<uint8_t> a(20, 1);

	a.resize(100);

	ASSERT_TRUE(a.size() == 100);

	for (size_t i = 0; i < 20; i++)
	{
		ASSERT_TRUE(a.read_bit(i) == 1);
	}
	for (size_t i = 20; i < 100; i++)
	{
		ASSERT_TRUE(a.read_bit(i) == 0);
	}
}

TEST(bitfield, can_resize_to_the_same)
{
	bitfield<uint8_t> a{ 1, 0, 1, 0, 1, 0, 1, 0 };
	bitfield<uint8_t> b(a);

	a.resize(a.size());

	ASSERT_TRUE(a == b);
}

TEST(bitfield, can_resize_to_null_size)
{
	bitfield<uint8_t> a(10, 1);

	a.resize(0);

	ASSERT_TRUE(a.size() == 0);
}

TEST(bitfield, new_elements_after_resize_are_null)
{
	bitfield<uint8_t> a(10, 1);

	a.resize(2);

	a.resize(10);

	ASSERT_TRUE(a.read_bit(0) == 1);
	ASSERT_TRUE(a.read_bit(1) == 1);
	for (size_t i = 2; i < 10; i++)
	{
		ASSERT_TRUE(a.read_bit(i) == 0);
	}
}

