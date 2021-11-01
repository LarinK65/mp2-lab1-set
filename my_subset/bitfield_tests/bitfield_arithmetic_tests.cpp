#include "pch.h"
#include "../MySubset/MyBitfield.h"

TEST(bitfield_arithmetic, is_nothrow_default_constructed)
{
	ASSERT_NO_THROW(bitfield_arithmetic<uint8_t> {});
}

TEST(bitfield_arithmetic, is_nothrow_constructed_with_size)
{
	size_t size = 10005;

	ASSERT_NO_THROW(bitfield_arithmetic<uint8_t> (size));
	ASSERT_TRUE((bitfield_arithmetic<uint8_t> (size)).size() == size);
}

TEST(bitfield_arithmetic, is_null_after_construct_with_size)
{
	size_t size = 10005;
	bitfield_arithmetic<uint8_t> a(size);

	for (size_t i = 0; i < a.size(); i++)
	{
		ASSERT_TRUE(a.read_bit(i) == 0);
	}
}

TEST(bitfield_arithmetic, constructor_with_size_ans_value_work)
{
	size_t size = 10005;
	bitfield_arithmetic<uint8_t> a(size, 1);

	for (size_t i = 0; i < a.size(); i++)
	{
		ASSERT_TRUE(a.read_bit(i) == 1);
	}
}

TEST(bitfield_arithmetic, constructor_with_initializer_list_work)
{
	bitfield_arithmetic<uint8_t> a{ 1, 0, 0, 1, 1, 1, 0, 0, 1 };

	ASSERT_TRUE(a.read_bit(0) == 1);
	ASSERT_TRUE(a.read_bit(1) == 0);
	ASSERT_TRUE(a.read_bit(2) == 0);
	ASSERT_TRUE(a.read_bit(3) == 1);
	ASSERT_TRUE(a.read_bit(4) == 1);
	ASSERT_TRUE(a.read_bit(5) == 1);
	ASSERT_TRUE(a.read_bit(6) == 0);
	ASSERT_TRUE(a.read_bit(7) == 0);
	ASSERT_TRUE(a.read_bit(8) == 1);
}

TEST(bitfield_arithmetic, can_convert_type)
{
	bitfield_arithmetic<uint32_t> a{ 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1 };
	bitfield_arithmetic<uint8_t> b(a);

	ASSERT_TRUE(a.size() == b.size());
	for (size_t i = 0; i < a.size(); i++)
	{
		ASSERT_TRUE(a.read_bit(i) == b.read_bit(i));
	}
}

TEST(bitfield_arithmetic, and_operator_work)
{
	bitfield_arithmetic<uint8_t> a{ 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1 };
	bitfield_arithmetic<uint8_t> b{ 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1 };
	bitfield_arithmetic<uint8_t> c{ 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1 };

	ASSERT_TRUE((a & b) == c);
}

TEST(bitfield_arithmetic, or_operator_work)
{
	bitfield_arithmetic<uint8_t> a{ 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1 };
	bitfield_arithmetic<uint8_t> b{ 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1 };
	bitfield_arithmetic<uint8_t> c{ 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1 };

	ASSERT_TRUE((a | b) == c);
}

TEST(bitfield_arithmetic, xor_oprator_work)
{
	bitfield_arithmetic<uint8_t> a{ 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1 };
	bitfield_arithmetic<uint8_t> b{ 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1 };
	bitfield_arithmetic<uint8_t> c{ 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0 };

	ASSERT_TRUE((a ^ b) == c);
}

TEST(bitfield_arithmetic, dif_opeator_work)
{
	bitfield_arithmetic<uint8_t> a{ 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1 };
	bitfield_arithmetic<uint8_t> b{ 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1 };
	bitfield_arithmetic<uint8_t> c{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0 };

	ASSERT_TRUE((a - b) == c);
}

TEST(bitfield_arithmetic, inv_operator_work)
{
	bitfield_arithmetic<uint8_t> a{ 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1 };
	bitfield_arithmetic<uint8_t> b{ 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0 };

	ASSERT_TRUE(~(~a) == a);
	ASSERT_TRUE(~a == b);
}

TEST(bitfield_arithmetic, and_for_operands_with_different_len)
{
	bitfield_arithmetic<uint8_t> a{ 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1 };
	bitfield_arithmetic<uint8_t> b{ 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0 };

	ASSERT_ANY_THROW(a & b);
}



