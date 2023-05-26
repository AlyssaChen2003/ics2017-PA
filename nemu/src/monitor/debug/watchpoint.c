#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
int used_next;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].result = 0;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp()
{
    if(free_ == NULL) {
        assert(0);
    }

    WP *temp = free_;
    free_ = free_->next;

    if(head == NULL) {
        head = temp;
    } else {    
        WP *search_head = head;
        while(search_head->next != NULL) {
            search_head = search_head->next;
        }
        search_head->next = temp;
    }

    temp->next = NULL;
    return temp;
}
void free_wp(int wpid)
{
    WP* wp = &wp_pool[wpid];
    WP *temp = head;

    if(temp == wp) {
        head = wp->next;
    } else {    
        while(temp->next != wp) {
            temp = temp->next;
        }
        temp->next = wp->next;
    }

    wp->next = free_;
    free_ = wp;
}

bool check_watchpoint()
{
	WP *wp=head;
	bool* success_flag=malloc(1);
	bool ret=true;	
	while(wp!=NULL)
	{
		uint32_t new_value=expr(wp->expr,success_flag);
		//printf("new value %d\n",new_value);
		if(new_value!=wp->expr_record_val){
			printf("\nHit watchpoint %d  old value = 0x%x, new value = 0x%x .\n"
				,wp->NO,wp->expr_record_val,new_value);
			ret=false;
			goto end;			
		}
		wp=wp->next;
	}
end:	
	//free(success_flag);
	return ret;
}

void info_watchpoint()
{
	WP *temp=head;	
	printf("WID	expr		record value\n");
	while(temp!=NULL)
	{
		printf("%d	%s		0x%x\n",temp->NO,temp->expr,temp->expr_record_val);
		temp=temp->next;
	}
}



