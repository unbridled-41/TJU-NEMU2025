#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include <string.h>
#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
void clean_WP(WP* wp);
void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
		wp_pool[i].empty = true;
		int j = 0;
		for (; j < 100; j++) {
			wp_pool[i].expr[j] = '\0';
		}
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}
WP* new_wp() {
	WP* p = free_;
	if(p == NULL)
		panic("NO useable WP!");
	else{
		free_ = free_->next;
		clean_WP(p);
		p->next = NULL;
		return p;
	}
	
}
void create_wp(char* args) {
	//printf("create_wp diaoyong\n");
	WP* p = new_wp();
	strcpy(p->expr, args);
	bool success = false;
	uint32_t tmp = expr(p->expr, &success);
	if (success)
		p->old_value = tmp;
	else
		panic("cannot load the value of expr!\n");
	printf("Set watchpoint #%d \n", p->NO);
	p->empty = false;
	if (head == NULL) {
		head = p;
	}
	else {
		WP* h = head;
		while (h->next != NULL) {
			h = h->next;
		}
		h->next = p;
		p->next = NULL;
	}
}
void delete_wp(int num) {
	int i = 0;
	for (; i < NR_WP; i++) {
		if (wp_pool[i].NO == num) {
			if (wp_pool[i].empty)
				printf("Watchpoint #%d does not exist\n",wp_pool[i].NO);
			else {
				free_wp(&wp_pool[i]);
			}
		}
	}
}
void free_wp(WP* wp) {
	if (head->NO == wp->NO) {
		if (head->next == NULL) {
			head->empty = true;
			head = NULL;
		}
		else {
			head->empty = true;
			clean_WP(head);
			WP* h = head;
			head = head->next;
			h->next = free_;
			free_ = h;
			//printf("delete successfully\n");

		}
	}
	else {
		WP* p = head;
		for (; p->next != NULL; p = p->next) {
			if (p->next ->NO == wp->NO) {
				if (free_ == NULL) {
					free_ = p->next;
					free_->empty = true;
					clean_WP(free_);
					p->next = free_->next;
					free_->next = NULL;
					//printf("delete successfully\n");
				}
				else {
					WP* tem = p->next;
					p->next->empty = true;
					clean_WP(p->next);
					p->next = p->next->next;
					tem->next = free_;
					free_ = tem;
					//printf("delete successfully\n");
				}
				return;
			}
		}
		printf("Watchpoint #%d does not exist\n",wp->NO);
	}
}
void display_wp() {
	if (head == NULL) {
		panic("no watchpoints,please set watchpoint first.");
		return;
	}
	printf("NO				Address					Enable\n");
	WP* h = head;
	printf("%d			%s=0x%08x				0x%08x\n", h->NO, h->expr, h->old_value, h->old_value);
	while (h->next != NULL) {
		h = h->next;
		printf("%d			%s=0x%08x				0x%08x\n", h->NO, h->expr, h->old_value, h->old_value );
	}
}
bool check_watchpoint_value() {
	WP* h = head;
	if (head == NULL)
		return true;
	bool flag = false;
	{
		bool success = false;
		//printf("expr(%s )\n", h->expr);
		uint32_t tmp = expr(h->expr, &success);
		if (success)
			h->new_value = tmp;
		else
			panic("cannot load NEW value !\n");
		if (h->new_value != h->old_value) {
			flag = true;
			printf("Hint watchpoint %d at address 0x%08x, expr= %s\n", h->NO, expr("$eip", &success), h->expr);
			printf("old value = 0x%08x\n", h->old_value);
			printf("new value = 0x%08x\n", h->new_value);
			h->old_value = h->new_value;
		}
	}
	while (h->next != NULL) {
		h = h->next;
		bool success = false;
		//printf("expr(%s )\n", h->expr);
		uint32_t tmp = expr(h->expr, &success);
		if (success)
			h->new_value = tmp;
		else
			panic("cannot load NEW value !\n");
		if (h->new_value != h->old_value) {
			flag = true;
			printf("Hint watchpoint %d at address 0x%08x, expr= %s\n", h->NO, expr("$eip", &success), h->expr);
			printf("old value = 0x%08x\n", h->old_value);
			printf("new value = 0x%08x\n", h->new_value);
			h->old_value = h->new_value;
		}
	}
	if (flag)
		return false;
	else
		return true;
}
void clean_WP(WP* wp) {
	int i = 0;
	for (; i < 100; i++) {
		wp->expr[i] = '\0';
	}
}
/* TODO: Implement the functionality of watchpoint */


