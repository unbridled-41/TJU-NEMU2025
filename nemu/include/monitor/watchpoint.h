#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */
	uint32_t old_val;
	uint32_t new_val;
	char exp[1024];
} WP;

WP *first_wp();

WP *new_wp();

void free_wp(WP *wp);

void delete_wp(int no);

void print_wp();

bool check_wp(swaddr_t eip);

#endif
