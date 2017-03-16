#include "labs/fiber.h"
#include "labs/vgatext.h"


void fun(addr_t* pmain_stack, addr_t* pf_stack, int* pret, bool* pdone,int args){
	addr_t& main_stack=*pmain_stack;
	addr_t& f_stack=*pf_stack;
	int& ret=*pret;
	bool& done=*pdone;

	int i,j,k; // Local Variables.
	//hoh_debug("iske ander nahi ja raha hai...");
	for(i=1;i<=args;i++){
		for(j=1;j<=args;j++){
			for(k=1;k<=args;k++){
				//hoh_debug("long computation is running(fiber).."<<i);
				ret=i+j+k; done=false; stack_saverestore(f_stack,main_stack);
			}
		}
	}
	for(;;){
		done=true; stack_saverestore(f_stack,main_stack);
	}
}

void shell_step_fiber(shellstate_t& shellstate, addr_t& main_stack, preempt_t& preempt, addr_t& f_stack, addr_t f_array, uint32_t f_arraysize, dev_lapic_t& lapic){

	if(shellstate.lcf_run && shellstate.lcf_init){
		shellstate.lcf_done=false;
		shellstate.lcf_init=false;
		stack_init5(f_stack,f_array,f_arraysize,&fun,&main_stack,&f_stack,&shellstate.lcf_out,&shellstate.lcf_done,shellstate.args);
	}
	else if(!shellstate.lcf_done && shellstate.lcf_run){
		stack_saverestore(main_stack,f_stack);
	}
	else if(shellstate.lcf_done && shellstate.lcf_run){
		shellstate.lcf_run=false;
		shellstate.lcf_done=false;
		vgatext::writechar(30+shellstate.w*19,shellstate.lcf_out%10+'0',0,6,shellstate.vgatext_base);
	}
}

