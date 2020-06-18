/**
 * mnist.c
 * Copyright (C) Tony Givargis, 2019-2020
 *
 * This file is part of The Gravity Compiler.
 *
 * The Gravity Compiler is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. The Gravity Compiler is distributed in
 * the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details. You should
 * have received a copy of the GNU General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/>.
 */

//#include <sys/time.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "test-avr.h"
#include "avrmem.h"
#include "dump-ram.h"

#define BATCH  8
#define EPOCHS 4
#define REAL_T float

#define MKSTRING(x) #x
#define TOSTRING(x) MKSTRING(x)
#define UL(x)       ((unsigned long)(x))

typedef REAL_T real_t;

static int32_t
swap(int32_t x)
{
	union { int32_t i; char b[4]; } in, out;

	in.i = x;
	out.b[0] = in.b[3];
	out.b[1] = in.b[2];
	out.b[2] = in.b[1];
	out.b[3] = in.b[0];
	return out.i;
}

static int
argmax(const real_t *a, int n)
{
	real_t max;
	int i, j;

	max = a[0];
	for (i=j=0; i<n; ++i) {
		if (max < a[i]) {
			max = a[i];
			j = i;
		}
	}
	return j;
}

static int
train_and_test(
	       const uint8_t *train_y,
	       const uint8_t *train_x,
	       const uint8_t *test_y,
	       const uint8_t *test_x,
	       int train_n,
	       int test_n)
{
	const uint8_t *labels, *images;
	int i, j, k, m, error;
	real_t *x, *y, *z;

	x = (real_t *)malloc(BATCH * 28 * 28 * sizeof (x[0]));
	y = (real_t *)malloc(BATCH * 10 * sizeof (y[0]));
	if (!x || !y) {
		free(x);
		free(y);
		fprintf(stderr, "out of memory\n");
		return -1;
	}

	/* train */

	m = train_n / BATCH;
	labels = train_y;
	images = train_x;
	for (i=0; i<m; ++i) {
		for (j=0; j<BATCH; ++j) {
			for (k=0; k<(28*28); ++k) {
				x[j * (28*28) + k] = (*images++) / 255.0;
			}
			for (k=0; k<10; ++k) {
				y[j * 10 + k] = 0.0;
			}
			y[j * 10 + (*labels++)] = 1.0;
		}
		test_train(x, y);
	}

	/* test */

	error = 0;
	labels = test_y;
	images = test_x;
	for (i=0; i<test_n; ++i) {
		for (k=0; k<(28*28); ++k) {
			x[k] = (*images++) / 255.0;
		}
		z = (real_t *) test_activate(x);
		if (argmax(z, 10) != (int)(*labels++)) {
			error++;
		}
		free(z); // test_activate will leak memory otherwise
	}

	/* done */

	free(x);
	free(y);
	return 0;
}


static int test(
	       const uint8_t *test_y,
	       const uint8_t *test_x,
	       int test_n)
{
	const uint8_t *labels, *images;
	int i, j, k, m, error;
	real_t *x, *y, *z;
	
	x = (real_t *)malloc(BATCH * 28 * 28 * sizeof (x[0]));
	y = (real_t *)malloc(BATCH * 10 * sizeof (y[0]));
	if (!x || !y) {
		free(x);
		free(y);
		return -1;
	}
	
	error = 0;
	labels = test_y;
	images = test_x;
	for (i=0; i<test_n; ++i) {
		for (k=0; k<(28*28); ++k) {
			x[k] = (*images++) / 255.0;
		}
		z = (real_t *) test_activate(x);
		if (argmax(z, 10) != (int)(*labels++)) {
			error++;
		}
		free(z); // test_activate will leak memory otherwise
	}

	/* done */

	free(x);
	free(y);
	return 0;
}


static uint8_t *
load_labels(const char *pathname, int *n)
{
	int32_t meta[2];
	uint8_t *data;
	FILE *file;

	file = fopen(pathname, "r");
	if (!file) {
		return 0;
	}
	if (sizeof (meta) != fread(meta, 1, sizeof (meta), file)) {
		fclose(file);
		return 0;
	}
	if ((0x1080000 != meta[0]) || (0 >= swap(meta[1]))) {
		fclose(file);
		return 0;
	}
	(*n) = swap(meta[1]);
	meta[1] = (*n);
	data = (uint8_t *)malloc(meta[1]);
	if (!data) {
		fclose(file);
		return 0;
	}
	if ((size_t)meta[1] != fread(data, 1, meta[1], file)) {
		free(data);
		fclose(file);
		return 0;
	}
	fclose(file);
	return data;
}

static uint8_t *
load_images(const char *pathname, int *n)
{
	int32_t meta[4];
	uint8_t *data;
	FILE *file;

	file = fopen(pathname, "r");
	if (!file) {
		return 0;
	}
	if (sizeof (meta) != fread(meta, 1, sizeof (meta), file)) {
		fclose(file);
		return 0;
	}
	if ((0x3080000 != meta[0]) ||
	    (0  >= swap(meta[1])) ||
	    (28 != swap(meta[2])) ||
	    (28 != swap(meta[3]))) {
		fclose(file);
		return 0;
	}
	(*n) = swap(meta[1]);
	meta[1] = (*n) * 28 * 28;
	data = (uint8_t *)malloc(meta[1]);
	if (!data) {
		fclose(file);
		return 0;
	}
	if ((size_t)meta[1] != fread(data, 1, meta[1], file)) {
		free(data);
		fclose(file);
		return 0;
	}
	fclose(file);
	return data;
}


////int
////main()
////{
////	int train_y_n, train_x_n, test_y_n, test_x_n;
////	uint8_t *train_y, *train_x, *test_y, *test_x;
////	int i, e;
////
////	// Start the RAM
////	ram_init();
////
////	// Allocate memory space
////	// In translation to AVR, m becomes a long indicating the base address of the array
////	// ...and is henceforth never used
////	long m = avr_alloc(test_memory_size());
////
////	// Initialize memory space
////	test_initialize();
////
////	/* load train/test data */
////
////	e = 0;
////	train_y = load_labels("data/train-labels", &train_y_n);
////	train_x = load_images("data/train-images", &train_x_n);
////	test_y = load_labels("data/test-labels", &test_y_n);
////	test_x = load_images("data/test-images", &test_x_n);
////	if (!train_y ||
////	    !train_x ||
////	    !test_y ||
////	    !test_x ||
////	    (train_y_n != train_x_n) ||
////	    (test_y_n != test_x_n)) {
////		e = -1;
////	}
////	
////	/* just test instead of training */
////	
////	/*
////	read_ram();
////	
////	if (!e) {
////		if (test(
////				test_y,
////				test_x,
////				test_y_n)) {
////			e = -1;
////			fprintf(stderr, "failed to test\n");
////		}
////	}
////	
////	avr_free(m);
////	free(train_y);
////	free(train_x);
////	free(test_y);
////	free(test_x);
////	return e;
////	
////	//*/
////
////	/* train and test */
////
////	for (i=0; i<EPOCHS; ++i) {
////		if (!e) {
////			if (train_and_test(
////					   train_y,
////					   train_x,
////					   test_y,
////					   test_x,
////					   train_y_n,
////					   test_y_n)) {
////				e = -1;
////			}
////		}
////	}
////
////	/* close */
////
////	avr_free(m);
////	free(train_y);
////	free(train_x);
////	free(test_y);
////	free(test_x);
////	ram_close();
////	return e;
////}
