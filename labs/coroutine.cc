#include "labs/coroutine.h"
#include "labs/vgatext.h"


void fun(int arg, coroutine_t* pf_coro, f_t* pf_locals, int* pret, bool* pdone){
	coroutine_t& f_coro=*pf_coro;
	int& ret=*pret;
	bool& done=*pdone;
	int& i=pf_locals->i;
	int& j=pf_locals->j;
	int& k=pf_locals->k;

	h_begin(f_coro);
	for(i=1;i<=arg;i++){
		for(j=1;j<=arg;j++){
			for(k=1;k<=arg;k++){
				//hoh_debug("long computation is running(coroutine).."<<i);
				ret=i+j+k;done=false;h_yield(f_coro);
			}
		}
	}
	done=true;h_end(f_coro);
}


void shell_step_coroutine(shellstate_t& shellstate, coroutine_t& f_coro, f_t& f_locals){
	if(shellstate.lcc_run && !shellstate.lcc_done){
		fun(shellstate.args,&f_coro, &f_locals, &shellstate.lcc_out, &shellstate.lcc_done);
	} 
	else if(shellstate.lcc_run && shellstate.lcc_done){
	 	coroutine_reset(f_coro);
	 	vgatext::writechar(30+shellstate.w*18,shellstate.lcc_out%10+'0',0,6,shellstate.vgatext_base);
	 	shellstate.lcc_done=false;
	 	f_locals.i=0;
	 	f_locals.j=0;
	 	f_locals.k=0;
	 	shellstate.lcc_run=false;
	}
}


