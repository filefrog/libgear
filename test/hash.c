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

NEW_TEST(hash_functions)
{
	test("hash: H64");

	assert_int_eq("H64 equivalency", H64("test"), H64("test"));
}

NEW_TEST(hash_basics)
{
	struct hash *h;

	char *path = strdup("/some/path/some/where");
	char *name = strdup("staff");

	test("hash: Insertion and Lookup");
	assert_int_ne("no H64 collision", H64("path"), H64("name"));

	h = hash_new();
	assert_not_null("hash_new returns a pointer", h);

	assert_null("get 'path' fails prior to set", hash_get(h, "path"));
	assert_null("get 'name' fails prior to set", hash_get(h, "name"));

	assert_ptr_eq("set 'path' succeeds", path, hash_set(h, "path", path));
	assert_ptr_eq("set 'name' succeeds", name, hash_set(h, "name", name));

	assert_str_eq("get 'path' succeeds", path, hash_get(h, "path"));
	assert_str_eq("get 'name' succeeds", name, hash_get(h, "name"));

	hash_free(h);
	free(path);
	free(name);
}

NEW_TEST(hash_collisions)
{
	struct hash *h;
	char *path  = strdup("/some/path/some/where");
	char *group = strdup("staff");

	test("hash: Handling of Keyspace Collisions");
	assert_int_eq("H64 collision", H64("path"), H64("group"));

	h = hash_new();
	assert_not_null("hash_new returns a pointer", h);

	assert_null("get 'path' fails prior to set",  hash_get(h, "path"));
	assert_null("get 'group' fails prior to set", hash_get(h, "group"));

	assert_ptr_eq("set 'path' succeeds",  path,  hash_set(h, "path",  path));
	assert_ptr_eq("set 'group' succeeds", group, hash_set(h, "group", group));

	assert_str_eq("get 'path' succeeds",  path,  hash_get(h, "path"));
	assert_str_eq("get 'group' succeeds", group, hash_get(h, "group"));

	hash_free(h);
	free(path);
	free(group);
}

NEW_TEST(hash_overrides)
{
	struct hash *h;

	char *value1 = strdup("value1");
	char *value2 = strdup("value2");

	test("hash: value overriding");

	h = hash_new();
	assert_not_null("hash_new returns a pointer", h);

	assert_ptr_eq("set (1st) succeeds", value1, hash_set(h, "key", value1));
	assert_str_eq("get (1st) succeeds", "value1", hash_get(h, "key"));

	assert_ptr_eq("set (2nd) succeeds (returning prev. value ptr)", value1, hash_set(h, "key", value2));
	assert_str_eq("get (2nd) succeeds", "value2", hash_get(h, "key"));

	hash_free(h);
	free(value1);
	free(value2);
}

NEW_TEST(hash_get_null)
{
	struct hash *h = NULL;

	test("hash: Lookups against a NULL hash pointer");
	assert_null("get('test') always returns NULL", hash_get(h, "test"));

	h = hash_new();
	hash_set(h, "test", "valid");
	test("hash: Lookup a NULL key against a valid hash");
	assert_null("get(NULL) always returns NULL", hash_get(h, NULL));
	hash_free(h);
}

NEW_TEST(hash_for_each)
{
	struct hash *h = hash_new();
	struct hash_cursor c;
	char *key, *value;

	int saw_promise = 0;
	int saw_snooze  = 0;
	int saw_central = 0;
	int saw_bridge  = 0;

	hash_set(h, "promise", "esimorp");
	hash_set(h, "snooze",  "ezoons");
	hash_set(h, "central", "lartnec");
	hash_set(h, "bridge",  "egdirb");

	for_each_key_value(h, &c, key, value) {
		if (strcmp(key, "promise") == 0) {
			saw_promise = 1;
			assert_str_eq("Check value of 'promise'", "esimorp", value);
		} else if (strcmp(key, "snooze") == 0) {
			saw_snooze = 1;
			assert_str_eq("Check value of 'snooze'", "ezoons", value);
		} else if (strcmp(key, "central") == 0) {
			saw_central = 1;
			assert_str_eq("Check value of 'central'", "lartnec", value);
		} else if (strcmp(key, "bridge") == 0) {
			saw_bridge = 1;
			assert_str_eq("Check value of 'bridge'", "egdirb", value);
		} else {
			assert_fail("Unexpected value found during for_each_key_value");
		}
	}
	hash_free(h);
}

NEW_SUITE(hash)
{
	RUN_TEST(hash_functions);
	RUN_TEST(hash_basics);
	RUN_TEST(hash_collisions);
	RUN_TEST(hash_overrides);
	RUN_TEST(hash_get_null);

	RUN_TEST(hash_for_each);
}
