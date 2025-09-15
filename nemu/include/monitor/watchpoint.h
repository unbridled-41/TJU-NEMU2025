#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */
	char expr[100];
	uint32_t old_value, new_value;
	bool empty;
} WP;
WP* new_wp();
void free_wp(WP* wp);
void create_wp(char* args);
void delete_wp(int num);
void display_wp();
bool check_watchpoint_value();
#endif
