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

static void assert_canon(const char *orig_path, const char *expected)
{
	struct path *p;
	p = path_new(orig_path);
	assert_not_null("path_new succeeds", p);
	assert_int_eq("path_canon returns 0", path_canon(p), 0);
	assert_str_eq("path canonicalized properly", path(p), expected);
	path_free(p);
}

NEW_TEST(path_creation)
{
	struct path *p;

	test("path: Creation of new paths");

	p = path_new(NULL);
	assert_null("NULL path string should yield NULL path", p);

	p = path_new("/etc/test");
	assert_not_null("Creation of new path succeeds", p);
	assert_str_eq("path buffer set properly", path(p), "/etc/test");

	path_free(p);
}

NEW_TEST(path_canon)
{
	test("path: Canonicalization (normal case)");
	assert_canon("/usr/local/sbin", "/usr/local/sbin");

	test("path: Canonicalization (empty path)");
	assert_canon("", "");

	test("path: Canonicalization (single . component)");
	assert_canon("/usr/./local/sbin", "/usr/local/sbin");
	test("path: Canonicalization (multiple . components)");
	assert_canon("/usr/./././local/./sbin", "/usr/local/sbin");

	test("path: Canonicalization (single .. component)");
	assert_canon("/usr/local/opt/../sbin", "/usr/local/sbin");
	test("path: Canonicalization (multiple .. components)");
	assert_canon("/usr/share/man/../../root/../local/opt/../sbin", "/usr/local/sbin");

	test("path: Canonicalization (single /. at the end)");
	assert_canon("/usr/local/sbin/.", "/usr/local/sbin");
	test("path: Canonicalization (multiple /. at the end)");
	assert_canon("/usr/local/sbin/././.", "/usr/local/sbin");

	test("path: Canonicalization (single /./ at the end)");
	assert_canon("/usr/local/sbin/./", "/usr/local/sbin");
	test("path: Canonicalization (multiple /./ at the end)");
	assert_canon("/usr/local/sbin/./././", "/usr/local/sbin");

	test("path: Canonicalization (single /.. at the end)");
	assert_canon("/usr/local/sbin/..", "/usr/local");
	test("path: Canonicalization (multiple /.. at the end)");
	assert_canon("/usr/local/sbin/../..", "/usr");

	test("path: Canonicalization (single /../ at the end)");
	assert_canon("/usr/local/sbin/../", "/usr/local");
	test("path: Canonicalization (multiple /../ at the end)");
	assert_canon("/usr/local/sbin/../../", "/usr");

	test("path: Canonicalization (backup to /)");
	assert_canon("/usr/local/sbin/../../../etc", "/etc");
	test("path: Canonicalization (backup past /)");
	assert_canon("/etc/../../../", "/");

	test("path: Canonicalization (backup to / and stop)");
	assert_canon("/usr/local/sbin/../../../", "/");
	assert_canon("/usr/local/sbin/../../..",  "/");

	test("path: Canonicalization (stay at /)");
	assert_canon("/././././", "/");

	test("path: Canonicalization (free-for-all)");
	assert_canon("/etc/sysconfig/../.././etc/././../usr/local/./bin/../sbin", "/usr/local/sbin");

	test("path: Canonicalization (bogus paths)");
	assert_canon("/a/b/c/d/e/f/../", "/a/b/c/d/e");
}

NEW_TEST(path_push_pop)
{
	struct path *p;

	test("path: path_pop");

	p = path_new("/var/clockwork/ssl/pending");
	assert_not_null("path_new succeeded", p);

	assert_int_ne("pop(1) returns non-zero (more to go)", path_pop(p), 0);
	assert_str_eq("pop(1) yields proper path", path(p), "/var/clockwork/ssl");

	assert_int_ne("pop(2) returns non-zero (more to go)", path_pop(p), 0);
	assert_str_eq("pop(2) yields proper path", path(p), "/var/clockwork");

	assert_int_ne("pop(3) returns non-zero (more to go)", path_pop(p), 0);
	assert_str_eq("pop(3) yields proper path", path(p), "/var");

	assert_int_eq("pop(4) returns non-zero (done)", path_pop(p), 0);

	test("path: path_push");

	assert_int_ne("push(1) returns non-zero (more to go)", path_push(p), 0);
	assert_str_eq("push(1) yields proper path", path(p), "/var/clockwork");

	assert_int_ne("push(2) returns non-zero (more to go)", path_push(p), 0);
	assert_str_eq("push(2) yields proper path", path(p), "/var/clockwork/ssl");

	assert_int_ne("push(3) returns non-zero (more to go)", path_push(p), 0);
	assert_str_eq("push(3) yields proper path", path(p), "/var/clockwork/ssl/pending");

	assert_int_eq("push(4) returns zero (done)", path_push(p), 0);

	path_free(p);
}

NEW_TEST(path_free_null)
{
	struct path *p;

	test("path: path_free(NULL)");
	p = NULL;
	path_free(p);
	assert_null("path_free(NULL) doesn't segfault", p);
}

NEW_SUITE(path)
{
	RUN_TEST(path_creation);
	RUN_TEST(path_canon);
	RUN_TEST(path_push_pop);
	RUN_TEST(path_free_null);
}
