/*
  Copyright 2011 James Hunt <james@jameshunt.us>

  This file is part of libgear, a C framework library.

  libgear is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  libgear is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with libgear.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "test.h"

struct test_struct {
	int value;
	struct list entry;
};

static struct test_struct *new_test_struct(int value)
{
	struct test_struct *ts;
	ts = malloc(sizeof(struct test_struct));
	if (!ts) {
		return NULL;
	}
	list_init(&ts->entry);

	ts->value = value;
	return ts;
}

#define SETUP_THE_NUMBERS \
	struct test_struct \
	          *t4  = new_test_struct(4),  \
	          *t8  = new_test_struct(8),  \
	          *t15 = new_test_struct(15), \
	          *t16 = new_test_struct(16), \
	          *t23 = new_test_struct(23), \
	          *t42 = new_test_struct(42);

#define TEARDOWN_THE_NUMBERS do { \
	free(t4);  \
	free(t8);  \
	free(t15); \
	free(t16); \
	free(t23); \
	free(t42); \
} while (0)

static void assert_list_bounds(struct list *l, const char *desc, int head, int tail)
{
	char buf[128];

	snprintf(buf, 128, "list %s is not empty", desc);
	assert_true(buf, !list_empty(l));

	snprintf(buf, 128, "head of list %s is %d", desc, head);
	assert_int_eq(buf, list_head(l, struct test_struct, entry)->value, head);

	snprintf(buf, 128, "tail of list %s is %d", desc, tail);
	assert_int_eq(buf, list_tail(l, struct test_struct, entry)->value, tail);
}

static void assert_list_sum(const char *msg, struct list *head, int expected)
{
	struct test_struct *ts;
	int sum = 0;

	for_each_node(ts, head, entry) { sum += ts->value; }
	assert_int_eq(msg, sum, expected);
}

/*************************************************************************/

NEW_TEST(list_insertion)
{
	struct test_struct *ts1, *ts2, *ts3;
	LIST(l);

	ts1 = new_test_struct(4);
	ts2 = new_test_struct(8);
	ts3 = new_test_struct(15);


	test("List: entry addition via list_add_*");
	assert_true("LIST structure is empty by default", list_empty(&l));

	list_add_tail(&ts2->entry, &l);
	assert_list_bounds(&l, "(8)", 8, 8);

	list_add_head(&ts1->entry, &l);
	assert_list_bounds(&l, "(4, 8)", 4, 8);

	list_add_tail(&ts3->entry, &l);
	assert_list_bounds(&l, "(4, 8, 15)", 4, 15);

	free(ts1);
	free(ts2);
	free(ts3);
}

NEW_TEST(list_removal)
{
	struct test_struct *ts1, *ts2, *ts3;
	LIST(l);

	ts1 = new_test_struct(4);
	ts2 = new_test_struct(8);
	ts3 = new_test_struct(15);

	list_add_tail(&ts1->entry, &l);
	list_add_tail(&ts2->entry, &l);
	list_add_tail(&ts3->entry, &l);

	test("List: entry removal via list_del");
	list_del(&ts1->entry);
	assert_list_bounds(&l, "(8 15)", 8, 15);

	list_add_tail(&ts1->entry, &l);
	assert_list_bounds(&l, "(8 15, 4)", 8, 4);

	list_del(&ts3->entry);
	assert_list_bounds(&l, "(8 4)", 8, 4);

	list_del(&ts2->entry);
	assert_list_bounds(&l, "(4 4)", 4, 4);

	free(ts1);
	free(ts2);
	free(ts3);
}

NEW_TEST(list_iteration)
{
	LIST(l);
	struct test_struct *ts;
	int sum;

	SETUP_THE_NUMBERS;

	list_add_tail(&t4->entry,  &l);
	list_add_tail(&t8->entry,  &l);
	list_add_tail(&t15->entry, &l);
	list_add_tail(&t16->entry, &l);
	list_add_tail(&t23->entry, &l);
	list_add_tail(&t42->entry, &l);

	test("List: for_each* macros for list traversal");
	sum = 0;
	for_each_node(ts, &l, entry) { sum += ts->value; }
	assert_int_eq("for_each_node traversal", sum, 108);

	sum = 0;
	for_each_node_r(ts, &l, entry) { sum += ts->value; }
	assert_int_eq("for_each_node_r traversal", sum, 108);

	TEARDOWN_THE_NUMBERS;
}

NEW_TEST(list_join)
{
	LIST(first);
	LIST(last);

	SETUP_THE_NUMBERS;

	list_add_tail(&t4->entry,  &first);
	list_add_tail(&t8->entry,  &first);
	list_add_tail(&t15->entry, &first);

	list_add_tail(&t16->entry, &last);
	list_add_tail(&t23->entry, &last);
	list_add_tail(&t42->entry, &last);

	test("List: list_join");
	assert_list_sum("sum(4 8 15) is 27", &first, 27);
	assert_list_sum("sum(16 23 42) is 81", &last, 81);

	list_join(&first, &last);
	assert_list_sum("after join; sum(4 8 15 16 23 42) is 108", &first, 108);
	assert_list_bounds(&first, "(4 8 15 16 23 42)", 4, 42);

	assert_true("&last is empty after join", list_empty(&last));

	TEARDOWN_THE_NUMBERS;
}

NEW_TEST(list_moves)
{
	LIST(first);
	LIST(last);
	SETUP_THE_NUMBERS;

	list_add_tail(&t4->entry,  &first);
	list_add_tail(&t8->entry,  &first);
	list_add_tail(&t15->entry, &first);

	list_add_tail(&t16->entry, &last);
	list_add_tail(&t23->entry, &last);
	list_add_tail(&t42->entry, &last);

	test("List: moving entries between lists");
	assert_list_sum("sum(4 8 15) is 27", &first, 27);
	assert_list_bounds(&first, "(4 8 15)", 4, 15);

	assert_list_sum("sum(16 23, 42) is 81", &last, 81);
	assert_list_bounds(&last, "(16 23 42)", 16, 42);

	list_move_head(&t16->entry, &first);
	assert_list_sum("sum(16 4 8 15) is 43", &first, 43);
	assert_list_bounds(&first, "(16 4 8 15)", 16, 15);

	assert_list_sum("sum(23 42) is 65", &last, 65);
	assert_list_bounds(&last, "(23 42)", 23, 42);

	list_move_tail(&t4->entry, &last);
	assert_list_sum("sum(16 8 15) is 43", &first, 39);
	assert_list_bounds(&first, "(16 8 15)", 16, 15);

	assert_list_sum("sum(23 42 4) is 65", &last, 69);
	assert_list_bounds(&last, "(23 42 4)", 23, 4);

	TEARDOWN_THE_NUMBERS;
}

NEW_TEST(list_replacements)
{
	LIST(l);
	SETUP_THE_NUMBERS;

	list_add_tail(&t4->entry,  &l);
	list_add_tail(&t8->entry,  &l);
	list_add_tail(&t15->entry, &l);

	test("List: replacing arbitrary elements");
	assert_list_sum("sum(4 8 15) is 27", &l, 27);
	assert_list_bounds(&l, "(4 8 15)", 4, 15);

	list_replace(&t4->entry, &t16->entry);
	assert_list_sum("sum(16 8 15) is 39", &l, 39);
	assert_list_bounds(&l, "(16 8 15)", 16, 15);

	list_replace(&t15->entry, &t42->entry);
	assert_list_sum("sum(16 8 42) is 66", &l, 66);
	assert_list_bounds(&l, "(16 8 42)", 16, 42);

	list_replace(&t8->entry, &t23->entry);
	assert_list_sum("sum(16 23 42) is 81", &l, 81);
	assert_list_bounds(&l, "(16 23 42)", 16, 42);

	TEARDOWN_THE_NUMBERS;
}

NEW_SUITE(list)
{
	RUN_TEST(list_insertion);
	RUN_TEST(list_removal);
	RUN_TEST(list_iteration);
	RUN_TEST(list_join);
	RUN_TEST(list_moves);
	RUN_TEST(list_replacements);
}
