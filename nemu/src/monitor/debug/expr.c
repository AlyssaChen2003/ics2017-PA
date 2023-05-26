#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stddef.h>
#include <stdint.h>

#define MEM_SIZE 0x100000

uint8_t g_mem[MEM_SIZE];

typedef uint32_t swaddr_t;

static inline uint8_t swaddr_read(swaddr_t addr, size_t len) {
    assert(len == 1 || len == 2 || len == 4);
    return *(uint8_t *)(g_mem + addr);
}

enum {
	NOTYPE = 256, EQ, UEQ, AND, OR, NUM, HNUM, REG

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	{"[0][x]([0-9,a-f,A-F]){1,8}",HNUM},
	{"[$][a-z,A-Z]+",REG},	
	{"[0-9]+",NUM},	
	{"\\)",')'},
	{"\\(",'('},	
	{"/",'/'},	
	{"\\*",'*'},	
	{"-", '-'},
	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"==", EQ},					// equal
	{"!=",UEQ},
	{"!",'!'},
	{"[&][&]",AND},
	{"[|][|]",OR}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				switch(rules[i].token_type) {
					case '+':
					case '-':
					case '*':
					case '/':
					case '(':
					case ')':
					case '!':
					case EQ :
					case UEQ :
					case AND :
					case OR :
						{						
						tokens[nr_token].type=rules[i].token_type;
						nr_token++;
						if(nr_token>=32)
							assert(0);
						break;
						}
					case NOTYPE:
						{
						break;
						}
					case NUM:
					case HNUM:
						{
						tokens[nr_token].type=rules[i].token_type;
						int ini;
						if(substr_len>31)
							assert(0);					
						for(ini=0;ini<substr_len;ini++)						
						{
							tokens[nr_token].str[ini]=e[position-substr_len+ini];
						}
						tokens[nr_token].str[substr_len]='\0';
						nr_token++;
						if(nr_token>=32)
							assert(0);
						break;
						}
					case REG:{// rm $
						tokens[nr_token].type=rules[i].token_type;
						int ini;
						if(substr_len>31)
							assert(0);					
						for(ini=0;ini<substr_len-1;ini++)						
						{
							tokens[nr_token].str[ini]=e[position-substr_len+1+ini];
						}
						tokens[nr_token].str[substr_len]='\0';
						nr_token++;
						if(nr_token>=32)
							assert(0);
						break;
						}				
					default: 
						break;
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}
extern CPU_state cpu;
// uint32_t get_token_num_value(Token to){
	
// }

bool check_pre_valid(int l, int r){
  int tot = 0;
  for(int i=l;i<=r;i++){
    if(tokens[i].type=='('){
      tot+=1;
    }
    else if(tokens[i].type==')'){
      tot-=1;
    }
    if(tot<0){
      return false;
    }
  }
  return tot==0;
}

bool check_parentheses(int l, int r, bool *success){
  if(!check_pre_valid(l,r)){
    *success=false;
    return *success;
  }
  if(tokens[l].type=='('&&tokens[r].type==')'){
    if(check_pre_valid(l+1,r-1)){
      return true;
    }
  }
  *success=true;
  return false;
}
uint32_t eval(int start,int end)
{
    int i;
    bool success=true;
	if(start>end){
		printf("%d %d \n",start,end);
		assert(0);
	}
	else if(start==end){
	
    Token to=tokens[start];	
	uint32_t ret=0;
	char* str=to.str;
	if(to.type==NUM){
		i=0;		
		while(str[i]!='\0')
		{	
		ret=ret*10;		
		ret+=str[i]-'0';
		i++;
		}
	}
	else if(to.type==HNUM){
		i=2;
		while(str[i]!='\0')
		{	
		ret=ret*16;		
		ret+=str[i]-'0';
		i++;
		}
	}
	else if(to.type==REG){		
		if(strcmp(to.str,"eax")==0)
			ret=cpu.eax;
		else if(strcmp(to.str,"ecx")==0)
			ret=cpu.ecx;
		else if(strcmp(to.str,"edx")==0)
			ret=cpu.edx;
		else if(strcmp(to.str,"ebx")==0)
			ret=cpu.ebx;
		else if(strcmp(to.str,"esp")==0)
			ret=cpu.esp;
		else if(strcmp(to.str,"ebp")==0)
			ret=cpu.ebp;
		else if(strcmp(to.str,"esi")==0)
			ret=cpu.esi;
		else if(strcmp(to.str,"edi")==0)
			ret=cpu.edi;
		else if(strcmp(to.str,"eip")==0)
			ret=cpu.eip;
	}
	else
		ret=0xffffffff;
	return ret;
	}
	else if(check_parentheses(start,end,&success)){
		return eval(start+1,end-1);
	}
	else{
		int op=-1;
		int list_num=0;//'(' & ')' num
		//int i;
		for(i=start;i<=end;i++)
		{

            switch(tokens[i].type) {
                case '(':
                    list_num++;
                    break;
                case ')':
                    list_num--;
                    break;
                case '!':
                    if (op < 0 && list_num == 0)
                        op = i;
                    break;
                case '*':
                case '/':
                    if (list_num == 0) {
                        switch(op) {
                            case -1:
                                op = i;
                                break;
                            default:
                                if (tokens[i].type == '!') op = i;
                                break;
                        }
                    }
                    break;
                case '+':
                case '-':
                    if (list_num == 0) {
                        switch(op) {
                            case -1:
                                op = i;
                                break;
                            default:
                                switch(tokens[op].type) {
                                    case '*':
                                    case '/':
                                    case '!':
                                        op = i;
                                        break;
                                    default:
                                        if (tokens[i].type == '!') op = i;
                                        break;
                                }
                                break;
                        }
                    }
                    break;
                case EQ:
                case UEQ:
                    if (list_num == 0) {
                        switch(op) {
                            case -1:
                                op = i;
                                break;
                            default:
                                switch(tokens[op].type) {
                                    case '*':
                                    case '/':
                                    case '+':
                                    case '-':
                                    case '!':
                                        op = i;
                                        break;
                                    default:
                                        if (tokens[i].type == '!') op = i;
                                        break;
                                }
                                break;
                        }
                    }
                    break;
                case AND:
                    if (list_num == 0) {
                        switch(op) {
                            case -1:
                                op = i;
                                break;
                            default:
                                switch(tokens[op].type) {
                                    case '*':
                                    case '/':
                                    case '+':
                                    case '-':
                                    case '!':
                                    case EQ:
                                    case UEQ:
                                        op = i;
                                        break;
                                    default:
                                        if (tokens[i].type == '!') op = i;
                                        break;
                                }
                                break;
                        }
                    }
                    break;
                case OR:
                    if (list_num == 0) {
                        switch(op) {
                            case -1:
                                op = i;
                                break;
                            default:
                                switch(tokens[op].type) {
                                    case '*':
                                    case '/':
                                    case '+':
                                    case '-':
                                    case '!':
                                    case EQ:
                                    case UEQ:
                                    case AND:
                                        op = i;
                                        break;
                                    default:
                                        if (tokens[i].type == '!') op = i;
                                        break;
                                }
                                break;
                        }
                    }
                    break;
            }
        
            
    }
		
		
		uint32_t val1=0;
		uint32_t val2=0;
		if(op==start){//! * etc..
			val1=eval(op+1,end);
		}
		else{		
			val1=eval(start,op-1);
			val2=eval(op+1,end);
		}
		switch(tokens[op].type){

            case '+':
                return val1 + val2;
            case '-':
                return val1 - val2;
            case '*':
                if (op == start) {
                    return swaddr_read(val1, 4);
                }
                return val1 * val2;
            case '/':
                return val1 / val2;
            case EQ:
                return val1 == val2 ? 1 : 0;
            case UEQ:
                return val1 != val2 ? 1 : 0;
            case AND:
                return val1 && val2 ? 1 : 0;
            case OR:
                return val1 || val2 ? 1 : 0;
            case '!':
                return !val2 ? 1 : 0;
            default:
                panic("Invalid operator.");

		}	
	}
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	uint32_t ret=eval(0,nr_token-1);
	return ret;
}
