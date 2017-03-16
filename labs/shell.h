#pragma once
#include "util/config.h"
#include "util/debug.h"


struct fiber_scheduler_t{
	int arg;
	int out;
	bool used;
	bool done;
	bool run;
	bool init;
	int stack;
};

struct shellstate_t{
	uint8_t input_array[1024]; // This is for each line
	uint8_t storage[2048]; // This is my main storage
	int start_pointer; // This is the starting pointer of each line
	int end_pointer; // This is the end pointer of each line
	int space_pointer; // This is the space pointer in each line
	int main_end_pointer; // This is the end pointer in main storage
	int key_counter; // This is the number of key pressed counter 
	int args; // This is the input argument to function fact() and fib()
	char echo_args[1024]; // This is char* in front of echo()
	int echo_end_pointer; // This is the end pointer in char* for echo
	int output_args[1024];
	int output_args_size; // This is the output of functions fact() or fib()
	int function; // This is for the which function fact() or fib() or echo()
	int current_pointer;
	int current_line;
	int num_next_line;

	int w;
	int h;
	addr_t vgatext_base;

	// For Coroutine part (2.1)
	bool lcc_done;
	bool lcc_run;
	int lcc_out;
	
	// For Fiber part (2.2)
	bool lcf_done;
	bool lcf_run;
	bool lcf_init;
	int lcf_out;

	// For Fiber_Scheduler part (2.3)
	fiber_scheduler_t fun1_vars[3];
	fiber_scheduler_t fun2_vars[3];

	int number_of_fiber;
	bool schedule_list[6];

	int current_fun;

	bool which_stack[5];

	bool illegal_operation;
};

struct renderstate_t{
	uint8_t output_array[1024]; // This is the output line 
	char echo_args[1024]; // This is char* to store in front of echo()
	int echo_end_pointer; // This is the char* end pointer
	int key_counter; // This is the number of key pressed 
	int result_args[1024];
	int result_args_size=0; // This is the result of functions called fact() or fib()
	int end_pointer=0; // This is the end pointer of output line
	int which_fun=0; // This is to check which function is called
	int starter; // This is for the starting point after pressing enter
	int current_pointer;
	int num_next_line;
	int current_line;
	int number_of_fiber;
	bool illegal_operation;
};

void shell_init(shellstate_t& state);
void shell_update(uint8_t scankey, shellstate_t& stateinout);
void shell_step(shellstate_t& stateinout);
void shell_render(const shellstate_t& shell, renderstate_t& render);

bool render_eq(const renderstate_t& a, const renderstate_t& b);
void render(const renderstate_t& state, int w, int h, addr_t display_base);

