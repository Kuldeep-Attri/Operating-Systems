#include "labs/fiber_scheduler.h"
#include "labs/vgatext.h"

//
// stackptrs:      Type: addr_t[stackptrs_size].  array of stack pointers (generalizing: main_stack and f_stack)
// stackptrs_size: number of elements in 'stacks'.
//
// arrays:      Type: uint8_t [arrays_size]. array of memory region for stacks (generalizing: f_array)
// arrays_size: size of 'arrays'. equal to stackptrs_size*STACK_SIZE.
//
// Tip: divide arrays into stackptrs_size parts.
// Tip: you may implement a circular buffer using arrays inside shellstate_t
//      if you choose linked lists, create linked linked using arrays in
//      shellstate_t. (use array indexes as next pointers)
// Note: malloc/new is not available at this point.
//

void fun_1(addr_t* pmain_stack, addr_t* pf_stack, int* pret, bool* pdone,int args)// Finding nth prime number...
{
	addr_t& main_stack=*pmain_stack;
	addr_t& f_stack=*pf_stack;
	int& ret=*pret;
	bool& done=*pdone;

	int i,j,k,l;
	for(i=1;i<=args;i++){
		for(j=1;j<=args;j++){
			for(k=1;k<=args;k++){
				for(l=1;l<=args;l++){
                        // hoh_debug("dasd "<<i);
					ret=i+j+k+l; done =false; stack_saverestore(f_stack,main_stack);
				}
			}
		}
	}
	for(;;){
		done=true; stack_saverestore(f_stack,main_stack);
	}
}

void fun_2(addr_t* pmain_stack, addr_t* pf_stack, int* pret, bool* pdone,int args, preempt_t* preempt){

	addr_t& main_stack=*pmain_stack;
	addr_t& f_stack=*pf_stack;
	int& ret=*pret;
	bool& done=*pdone;

	int i,j,k,l,m;
	for(i=1;i<=args;i++){
		for(j=1;j<=args;j++){
			for(k=1;k<=args;k++){
				for(l=1;l<=args;l++){
					for(m=1;m<=args;m++){
						ret=i+j+k+l+m; done =false; 
                              preempt->saved_stack=0;
                            //  hoh_debug("ds "<<i);
                              stack_saverestore(f_stack,main_stack);
					}
				}
			}
		}
	}
	for(;;){
		done=true; 
          preempt->saved_stack=0;
          stack_saverestore(f_stack,main_stack);
	}
}

void shell_step_fiber_scheduler(shellstate_t& shellstate,addr_t& main_stack, preempt_t& preempt, addr_t stackptrs[], size_t stackptrs_size, addr_t arrays, size_t arrays_size, dev_lapic_t& lapic){

	if(shellstate.schedule_list[shellstate.current_fun]){
		if(shellstate.current_fun<=2){
              // preempt.saved_stack=0;
     		if(shellstate.fun1_vars[shellstate.current_fun].run && shellstate.fun1_vars[shellstate.current_fun].init){
     			shellstate.fun1_vars[shellstate.current_fun].init=false;
     			shellstate.fun1_vars[shellstate.current_fun].done=false;
     			stack_init5(stackptrs[2*shellstate.fun1_vars[shellstate.current_fun].stack-1],arrays+(4096*shellstate.fun1_vars[shellstate.current_fun].stack-1),(arrays_size/10)*shellstate.fun1_vars[shellstate.current_fun].stack,&fun_1,&main_stack,&stackptrs[2*shellstate.fun1_vars[shellstate.current_fun].stack-1],&shellstate.fun1_vars[shellstate.current_fun].out,&shellstate.fun1_vars[shellstate.current_fun].done, shellstate.fun1_vars[shellstate.current_fun].arg);
     		}
     		else if(shellstate.fun1_vars[shellstate.current_fun].run && !shellstate.fun1_vars[shellstate.current_fun].done){
     			stack_saverestore(main_stack,stackptrs[2*shellstate.fun1_vars[shellstate.current_fun].stack-1]);
     		}
     		else if(shellstate.fun1_vars[shellstate.current_fun].run && shellstate.fun1_vars[shellstate.current_fun].done){
     			vgatext::writechar(40+shellstate.w*(19+shellstate.fun1_vars[shellstate.current_fun].stack),shellstate.fun1_vars[shellstate.current_fun].out%10+'0',0,6,shellstate.vgatext_base);
     			shellstate.fun1_vars[shellstate.current_fun].run=false;
     			shellstate.fun1_vars[shellstate.current_fun].init=false;
     			shellstate.fun1_vars[shellstate.current_fun].done=false;
     			shellstate.fun1_vars[shellstate.current_fun].used=false;
     			shellstate.fun1_vars[shellstate.current_fun].arg=0;
     			shellstate.fun1_vars[shellstate.current_fun].out=0;
     			shellstate.which_stack[shellstate.fun1_vars[shellstate.current_fun].stack-1]=false;
     			shellstate.fun1_vars[shellstate.current_fun].stack=0;
     			shellstate.number_of_fiber--;
     			shellstate.schedule_list[shellstate.current_fun]=false;
     		}
     	}
     	else if(shellstate.current_fun<=5 && shellstate.current_fun>=3){
     		if(shellstate.fun2_vars[shellstate.current_fun-3].run && shellstate.fun2_vars[shellstate.current_fun-3].init){
     			shellstate.fun2_vars[shellstate.current_fun-3].init=false;
     			shellstate.fun2_vars[shellstate.current_fun-3].done=false;
     			stack_init6(stackptrs[2*shellstate.fun2_vars[shellstate.current_fun-3].stack-1],arrays+(4096*shellstate.fun2_vars[shellstate.current_fun-3].stack-1),(arrays_size/10)*shellstate.fun2_vars[shellstate.current_fun-3].stack,&fun_2,&main_stack,&stackptrs[2*shellstate.fun2_vars[shellstate.current_fun-3].stack-1],&shellstate.fun2_vars[shellstate.current_fun-3].out,&shellstate.fun2_vars[shellstate.current_fun-3].done, shellstate.fun2_vars[shellstate.current_fun-3].arg,&preempt);
     		}
     		else if(shellstate.fun2_vars[shellstate.current_fun-3].run && !shellstate.fun2_vars[shellstate.current_fun-3].done){
     			//preempt.saved_stack=&stackptrs[2*shellstate.fun2_vars[shellstate.current_fun-3].stack-1];
                    lapic.reset_timer_count(100);
                    stack_saverestore(main_stack,stackptrs[2*shellstate.fun2_vars[shellstate.current_fun-3].stack-1]);
     		     lapic.reset_timer_count(0);   
               }
     		else if(shellstate.fun2_vars[shellstate.current_fun-3].run && shellstate.fun2_vars[shellstate.current_fun-3].done){
     			vgatext::writechar(40+shellstate.w*(19+shellstate.fun2_vars[shellstate.current_fun-3].stack),shellstate.fun2_vars[shellstate.current_fun-3].out%10+'0',0,6,shellstate.vgatext_base);
     			shellstate.fun2_vars[shellstate.current_fun-3].run=false;
     			shellstate.fun2_vars[shellstate.current_fun-3].init=false;
     			shellstate.fun2_vars[shellstate.current_fun-3].done=false;
     			shellstate.fun2_vars[shellstate.current_fun-3].used=false;
     			shellstate.fun2_vars[shellstate.current_fun-3].arg=0;
     			shellstate.fun2_vars[shellstate.current_fun-3].out=0;
     			shellstate.which_stack[shellstate.fun2_vars[shellstate.current_fun-3].stack-1]=false;
     			shellstate.fun2_vars[shellstate.current_fun-3].stack=0;
     			shellstate.number_of_fiber--;
     			shellstate.schedule_list[shellstate.current_fun-3]=false;
     		}
     	}
    }
    if(shellstate.current_fun<5){shellstate.current_fun++;}else{shellstate.current_fun=0;}
}
