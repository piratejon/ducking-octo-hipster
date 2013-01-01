## Process this file with automake to produce Makefile.in

TESTS = tests_bignum
check_PROGRAMS = tests_bignum
tests_bignum_SOURCES = tests_main.c tests_bignum.c $(top_builddir)/src/bignum.h
tests_bignum_CFLAGS = -Wall -g3 -std=c99 -I$(top_builddir)/src -I$(top_builddir)/tests
tests_bignum_LDADD = $(top_builddir)/src/libbignum.la
tests_bignum_LDFLAGS = -no-install

noinst_PROGRAMS = $(TESTS)

