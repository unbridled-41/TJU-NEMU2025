#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */


WP *new_wp() {
	if(free_ == NULL)return NULL;
	WP *cur_wp = free_;
	free_ = free_->next;
	if(head == NULL){
		head = cur_wp;
		cur_wp->next = NULL;
	}else{
		cur_wp->next = head;
		head = cur_wp;
	}
	return cur_wp;
}

void free_wp(WP *wp){
	if(wp == head){
		head = wp->next;
	}else{
		WP *p=head;
		while(p->next != wp)
		      p = p->next;
		p->next = wp->next;
	}
	if(free_ == NULL){
		free_ = wp;
		wp->next = NULL;
	}else{
		wp->next = free_;
		free_ = wp;
	}
}
void delete_wp(int no){
	WP *wp_temp = head;
	while(wp_temp != NULL){
		if(wp_temp->NO == no){
			free_wp(wp_temp);
			printf("Deleted watchponit %d.\n",no);
			return ;
		}
		wp_temp = wp_temp->next;
	}
	if(wp_temp == NULL)
	      printf("NO watchponit %d !\n",no);
}
void print_wp(){
	printf("NO\tType      \tExpression\n");
	WP *wp_temp = head;
	while(wp_temp != NULL){
		printf("%d\twatchpoint\t%s\n",wp_temp->NO,wp_temp->exp);
		wp_temp = wp_temp->next;
	}
}

bool check_wp(swaddr_t eip){
	WP *wp = head;
	bool success = false, has_wp = false;
	while(wp != NULL){
		success = true;
		wp->new_val = expr(wp->exp,&success);
		assert(success);
		wp = wp->next;
	}
	wp = head;
	while(wp != NULL){
		if(wp->new_val != wp->old_val){
			has_wp = true;
			printf("Hint watchpoint %d at address 0x%.8x , expr = %s\n\nOld value = %d (0x%x)\nNew value = %d (0x%x)\n", wp->NO, eip, wp->exp, wp->old_val, wp->old_val, wp->new_val, wp->new_val);
			wp->old_val = wp->new_val;
		}
		wp = wp->next;
	}
	return has_wp;
}
