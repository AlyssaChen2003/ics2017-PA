#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
WP* new_wp();
void free_wp(int wpid);
void info_watchpoint();
/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_p(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);
static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  /* TODO: Add more commands */
  { "si", "Let the program execute N instructions step by step", cmd_si },
  { "info", "Print registers' status for r, checkpoint informations for w", cmd_info },
  { "x", "Scan the consecutive 4N bytes from Address EXPR", cmd_x },
  { "p", "Calculate the expression's value", cmd_p },
  { "w", "Set watchpoint i.e. pause the program until the EXPR's value changes", cmd_w },
  { "d", "Delete the watchpoint which number is N", cmd_d }



  

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
static int cmd_si(char *args){
  char *arg = strtok(NULL, " ");
  if(arg!=NULL){
    cpu_exec(atoi(arg));
  }
  else{
    cpu_exec(1);
  }
  return 0;
}
static int cmd_info(char *args){
  char *arg = strtok(NULL, " ");
  if(arg==NULL){
    printf("args error in cmd_info\n");
    return 0;
  }
  char s;
  int nRet = sscanf(args, "%c", &s);
  if(nRet<=0){
    printf("args error in cmd_info\n");
    return 0;
  }
  if(s == 'r'){
    int i;
    for(i=0;i<8;i++){
      printf("%s        0x%x\n", regsl[i], reg_l(i));
    }
    printf("eip        0x%x\n", cpu.eip);
    for(i=0;i<8;i++){
      printf("%s        0x%x\n", regsw[i], reg_w(i));
    }
    for(i=0;i<8;i++){
      printf("%s        0x%x\n", regsb[i], reg_b(i));
    }
  }
  else if(s=='w'){
    info_watchpoint();
  }
  return 0;
}
static int cmd_x(char *args){
   char *arg1 = strtok(NULL, " ");
  if(arg1==NULL){
    printf("u shall input the parameter N to specify the consecutive N..\n");
    return 0;
  }
  int i_arg1 = atoi(arg1);
  char *arg2 = strtok(NULL, " ");
  /* TODO: now i just implement the function given accurate number, must fix it in 1-2 or 1-3*/
  if(arg2==NULL){
    printf("u shall input the parameter EXPR must generate from keyboard input..!\n");
    return 0;
  }
  uint32_t addr_begin = strtoul(arg2,NULL,16);
  int i;
  for(i=0;i<i_arg1;i++){
    printf("0x%x ", vaddr_read(addr_begin,1));
    addr_begin+=1;
  }
  printf("\n");
  return 0;

}
static int cmd_p(char *args){
  char *arg = strtok(NULL," ");
  if(arg==NULL){
    printf("please input the expression u wanna calculate..!\n");
    return 0;
  }
  bool is_finish=true;
  uint32_t ans = expr(arg,&is_finish);
  if(!is_finish){
    printf("please check your expression's format..!\n");
  }
  else{
    printf("%d\n", ans);
  }
  return 0;
}

static int cmd_w(char *args){
	if(args==NULL){
		printf("please input expr again \n");
		return 0;	
	}	
	else{
		WP* wp=new_wp();
		strcpy(wp->expr,args);
		bool* success = malloc(4);
		wp->expr_record_val=expr(args,success);
		printf("watchpoint %d is set\n",wp->NO);
		return 0;
	}
}

static int cmd_d(char *args){
	if(args==NULL){
		printf("please input expr again \n");
		return 0;	
	}	
	else{
		int wpid=args[0]-'0';
		free_wp(wpid);
		printf("watchpoint %d is deleted\n",wpid);
		return 0;
	}
}
