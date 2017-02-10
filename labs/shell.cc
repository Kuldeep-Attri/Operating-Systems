#include "labs/shell.h"
#include "labs/vgatext.h"

//
// initialize shellstate
//
static void writecharxy(int x, int y, uint8_t c, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);


void number_key_pressed(int temp,int w, int h, addr_t vgatext_base){
    char key_counter[100];
    int size_key_counter=0;
    while(temp>0){
      key_counter[size_key_counter]=((char)(temp%10))+'0';
      temp=temp/10;
      size_key_counter++;
    }
    int j = 0;
    while(j < size_key_counter/2)
    {
      char temp = key_counter[size_key_counter-j-1];
      key_counter[size_key_counter-j-1] = key_counter[j];
      key_counter[j] = temp;
      ++j;
    }
    char *p = "$ Number of Keys pressed --> ";
      for(int loc=0;*p;loc++,p++){
        vgatext::writechar(loc+w*1,*p,0,5,vgatext_base);
      }
      for(int i=0;i<size_key_counter;i++){
        vgatext::writechar(i+32+w*1,key_counter[i],0,6,vgatext_base);
      }
}

int fibo(int n){
  int f1=0,f2=1,f;
  do{f=f1+f2;f1=f2;f2=f;n--;
  }while(n>1);
  return f;
}

int pow(int a){
  int c=1;while(a>0){c*=10;a--;}
  return c;
}

int multiply(int x, int res[], int res_size)
{
    int carry = 0;  // Initialize carry
 
    // One by one multiply n with individual digits of res[]
    for (int i=0; i<res_size; i++)
    {
        int prod = res[i] * x + carry;
        res[i] = prod % 10;  // Store last digit of 'prod' in res[]
        carry  = prod/10;    // Put rest in carry
    }
 
    // Put carry in res and increase result size
    while (carry)
    {
        res[res_size] = carry%10;
        carry = carry/10;
        res_size++;
    }
    return res_size;
}// This function finds factorial of large numbers and prints them
int  fact(int n,int *res)
{
    //int res[1000];
 
    // Initialize result
    res[0] = 1;
    int res_size = 1;
 
    // Apply simple factorial formula n! = 1 * 2 * 3 * 4...*n
    for (int x=2; x<=n; x++)
        res_size = multiply(x, res, res_size);
    
    return res_size;
   // cout << "Factorial of given number is \n";
    // for (int i=res_size-1; i>=0; i--)
    //     cout << res[i];
}




void shell_init(shellstate_t& state){
  state.key_counter=0; // It keep tracks of number of key pressed
  state.end_pointer=0; // It keep tracks of end position
  state.space_pointer=0; // it keep track of the position of space in the line 
  state.args=-1; // This is the argument which will in the fact(_) and fibo(_) functions
  state.start_pointer=0;
  state.function=0;
  state.main_end_pointer=0;
  state.echo_end_pointer=0;
  state.current_pointer=0;
  state.output_args_size=0;
  state.num_next_line=0;
  state.current_line=2;
}

//
// handle keyboard event.
// key is in scancode format.
// For ex:
// scancode for following keys are:
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     | esc |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 |  0 |  - |  = |back|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 01  | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0a | 0b | 0c | 0d | 0e |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     | tab |  q |  w |  e |  r |  t |  y |  u |  i |  o |  p |  [ |  ] |entr|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 0f  | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 1a | 1b | 1c |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     |ctrl |  a |  s |  d |  f |  g |  h |  j |  k |  l |  ; |  ' |    |shft|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 1d  | 1e | 1f | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 2a |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
// so and so..
//
// - restrict yourself to: 0-9, a-z, esc, enter, arrows
// - ignore other keys like shift, control keys
// - only handle the keys which you're interested in
// - for example, you may want to handle up(0x48),down(0x50) arrow keys for menu.
//
void shell_update(uint8_t scankey, shellstate_t& stateinout){

    hoh_debug("Got: "<<unsigned(scankey));
    if(scankey==0x39){ // It handle the Space key.
      if(stateinout.end_pointer==0){
        stateinout.key_counter++;
        stateinout.start_pointer++;
        stateinout.input_array[stateinout.end_pointer]=scankey;
        stateinout.storage[stateinout.main_end_pointer]=scankey;
        stateinout.main_end_pointer++;
        stateinout.end_pointer++;
        stateinout.current_pointer++;
      }
      else if(stateinout.input_array[stateinout.end_pointer-1]==0x39){
        stateinout.input_array[stateinout.end_pointer]=scankey;
        stateinout.storage[stateinout.main_end_pointer]=scankey;
        stateinout.main_end_pointer++;
        stateinout.end_pointer++;
        stateinout.key_counter++;
        stateinout.current_pointer++;
        stateinout.start_pointer++;
      }
      else if(stateinout.input_array[stateinout.end_pointer-1]!=0x39){
        stateinout.input_array[stateinout.end_pointer]=scankey;
        stateinout.storage[stateinout.main_end_pointer]=scankey;
        stateinout.main_end_pointer++;
        stateinout.end_pointer++;
        stateinout.key_counter++;
        stateinout.current_pointer++;
        if(stateinout.space_pointer==0){stateinout.space_pointer=stateinout.end_pointer-1;}
      }
    }
    else if(scankey==0x4b){ // This handle the Left Arrow
      if(stateinout.end_pointer==0){  
        stateinout.storage[stateinout.main_end_pointer]=scankey;
        stateinout.main_end_pointer++;
      }
      else if(stateinout.end_pointer!=0){
        if(stateinout.current_pointer>0){
          stateinout.storage[stateinout.main_end_pointer]=scankey;
          stateinout.main_end_pointer++;
          stateinout.current_pointer--;
        }
      }
      stateinout.key_counter++;
    }
    else if(scankey==0x4d){ // This handle the Right Arrow
      if(stateinout.end_pointer==0){  
        stateinout.storage[stateinout.main_end_pointer]=scankey;
        stateinout.main_end_pointer++;
      }
      else if(stateinout.end_pointer!=0){
        if(stateinout.current_pointer<stateinout.end_pointer){
          stateinout.storage[stateinout.main_end_pointer]=scankey;
          stateinout.main_end_pointer++;
          stateinout.current_pointer++;
        }
      }
      stateinout.key_counter++;

    }
    else if(scankey==0x0e){ // This handle the Backspace
      if(stateinout.end_pointer==0){  
        stateinout.storage[stateinout.main_end_pointer]=scankey;
        stateinout.main_end_pointer++;
      }
      else if(stateinout.end_pointer!=0){
        if(stateinout.current_pointer>0){
          stateinout.storage[stateinout.main_end_pointer]=scankey;
          stateinout.main_end_pointer++;
          stateinout.current_pointer--;
        }
      }
      stateinout.key_counter++;
    }
    else{ // This handle Rest of the keys for the Moment
      stateinout.key_counter++;
      stateinout.storage[stateinout.main_end_pointer]=scankey;
      stateinout.input_array[stateinout.current_pointer]=scankey;
      if(stateinout.current_pointer==stateinout.end_pointer){stateinout.end_pointer++;}
      stateinout.main_end_pointer++;
      if(stateinout.current_pointer%78==0){stateinout.current_line+=stateinout.current_pointer/78;}
      stateinout.current_pointer++; 

    }

}


//
// do computation
//
void shell_step(shellstate_t& stateinout){

  char scan_code[] = {' ',27,
                        '1','2','3','4','5','6','7','8','9','0','-','=',8,'\t',
                        'q','w','e','r','t','y','u','i','o','p','[',']',13,
                        ' ','a','s','d','f','g','h','j','k','l',';','\'','`',
                        ' ','\\','z','x','c','v','b','n','m',',','.','/',
                        ' ',42,' ',' '};
  if(stateinout.input_array[stateinout.end_pointer-1]==0x1c){ // This handle the Enter key
    stateinout.echo_end_pointer=0;
    if(stateinout.input_array[stateinout.start_pointer]==0x21 && stateinout.input_array[stateinout.start_pointer+1]==0x1e && stateinout.input_array[stateinout.start_pointer+2]==0x2e && stateinout.input_array[stateinout.start_pointer+3]==0x14 && stateinout.space_pointer-stateinout.start_pointer==4){
      hoh_debug("running Factorial...");
      int i=stateinout.space_pointer+1;
      int temp=0;
      while(i<stateinout.end_pointer-1){
        if(scan_code[stateinout.input_array[i]]-'0'>=0 && scan_code[stateinout.input_array[i]]-'0'<=9){
          temp+=(scan_code[stateinout.input_array[i]]-'0')*pow(stateinout.end_pointer-2-i);
        }
        else{
          hoh_debug("Wrong input to Factorial..:(");
          break;
        }
        i++;
      }
      stateinout.args=temp;
      stateinout.output_args_size=fact(stateinout.args,(int *)stateinout.output_args);
      //stateinout.num_prev_line=stateinout.num_next_line;
      if(stateinout.output_args_size%80!=0){stateinout.num_next_line=(stateinout.output_args_size/80)+1;}
      stateinout.function=2;
      hoh_debug("hello: "<<stateinout.num_next_line);
      //stateinout.current_line+=stateinout.num_prev_line;
      stateinout.num_next_line+=1;
      stateinout.current_line+=stateinout.num_next_line;
      //stateinout.num_prev_line+=1;

    }
    else if(stateinout.input_array[stateinout.start_pointer]==0x21 && stateinout.input_array[stateinout.start_pointer+1]==0x17 && stateinout.input_array[stateinout.start_pointer+2]==0x30 &&stateinout.space_pointer-stateinout.start_pointer==3){
      hoh_debug("running Fibonacci...");
      int i=stateinout.space_pointer+1;
      int temp=0;
      while(i<stateinout.end_pointer-1){
        if(scan_code[stateinout.input_array[i]]-'0'>=0 && scan_code[stateinout.input_array[i]]-'0'<=9){
          temp+=(scan_code[stateinout.input_array[i]]-'0')*pow(stateinout.end_pointer-2-i);
        }
        else{
          hoh_debug("Wrong input to Fibonacci..:(");
          break;
        }
        i++;
      }
      stateinout.args=temp;
      stateinout.output_args[0] = fibo(stateinout.args);
      stateinout.output_args_size=1;
      stateinout.num_next_line=2;
      stateinout.current_line+=stateinout.num_next_line;
      stateinout.function=3;
    }
    else if(stateinout.input_array[stateinout.start_pointer]==0x12 && stateinout.input_array[stateinout.start_pointer+1]==0x2e && stateinout.input_array[stateinout.start_pointer+2]==0x23 && stateinout.input_array[stateinout.start_pointer+3]==0x18 && stateinout.space_pointer-stateinout.start_pointer==4){
      hoh_debug("running echo...");
      int i=stateinout.space_pointer+1;
      while(i<stateinout.end_pointer-1){
        stateinout.echo_args[stateinout.echo_end_pointer]=scan_code[stateinout.input_array[i]];
        i++;
        stateinout.echo_end_pointer++;
      }
      stateinout.function=1;
      
      if(stateinout.echo_end_pointer%80!=0){stateinout.num_next_line=stateinout.echo_end_pointer/80 +1;}
      stateinout.num_next_line+=1;
      stateinout.current_line+=stateinout.num_next_line;
    }
    else{
      hoh_debug("Sorry dude!!!, I didn't understand your command... ");
      stateinout.function=-1;
      stateinout.num_next_line=2;
      stateinout.current_line+=stateinout.num_next_line;
    }
    stateinout.end_pointer=0;
    stateinout.start_pointer=0;
    stateinout.space_pointer=0;
    stateinout.current_pointer=0;
    if(stateinout.current_line>=24){
      stateinout.current_line=3;
      stateinout.num_next_line=0;
    }
    for(int i=0;i<1024;i++){
      stateinout.input_array[i]=0;
    }
  }
}

void shell_render(const shellstate_t& shell, renderstate_t& render){
  if(shell.storage[shell.main_end_pointer-1]==0x1c){
    render.output_array[render.end_pointer]=shell.storage[shell.main_end_pointer-1];
    // render.starter=0;
    render.end_pointer++;
  }
  else if(shell.storage[shell.main_end_pointer-1]==0x4b){
     render.output_array[render.end_pointer]=shell.storage[shell.main_end_pointer-1];
     render.end_pointer++;
  }
  else if(shell.storage[shell.main_end_pointer-1]==0x4d){
     render.output_array[render.end_pointer]=shell.storage[shell.main_end_pointer-1];
     render.end_pointer++;
  }
  else if(shell.storage[shell.main_end_pointer-1]==0x0e){
     render.output_array[render.end_pointer]=shell.storage[shell.main_end_pointer-1];
     render.end_pointer++;
  }
  else{
    render.output_array[shell.end_pointer-1]=shell.input_array[shell.end_pointer-1];
    render.end_pointer=shell.end_pointer;
    render.starter=render.end_pointer; 
  }
  render.key_counter=shell.key_counter;
  render.which_fun=shell.function;
  render.echo_end_pointer=shell.echo_end_pointer;
  render.current_pointer=shell.current_pointer; 
  render.result_args_size=shell.output_args_size;
  render.num_next_line=shell.num_next_line;
  render.current_line=shell.current_line;
  for(int i=0;i<render.result_args_size;i++){
    render.result_args[i]=shell.output_args[shell.output_args_size-1-i];
  }
  for(int i=0;i<render.echo_end_pointer;i++){
    render.echo_args[i]=shell.echo_args[i];
  }
}

bool render_eq(const renderstate_t& a, const renderstate_t& b){
  if(a.echo_end_pointer==b.echo_end_pointer && a.result_args_size==b.result_args_size && a.which_fun==b.which_fun){
    if(a.echo_end_pointer==b.echo_end_pointer && a.key_counter==b.key_counter && a.starter==b.starter && a.current_pointer==b.current_pointer){
      return true;
    }
  }
  return false;
}

//
// Given a render state, we need to write it into vgatext buffer
//
void render(const renderstate_t& state, int w, int h, addr_t vgatext_base){

  number_key_pressed(state.key_counter, w, h,vgatext_base); // This prints number of key pressed
  char scan_code[] = {' ',27,
                        '1','2','3','4','5','6','7','8','9','0','-','=',8,'\t',
                        'q','w','e','r','t','y','u','i','o','p','[',']',13,
                        ' ','a','s','d','f','g','h','j','k','l',';','\'','`',
                        ' ','\\','z','x','c','v','b','n','m',',','.','/',
                        ' ',42,' ',' '};
        
  uint8_t last_var = state.output_array[state.end_pointer-1];
  hoh_debug("last var is:  "<<last_var);
  char *p = "$ ";
  for(int loc=0;*p;loc++,p++){
    vgatext::writechar(loc+w*(state.current_line),*p,0,2,vgatext_base);
  }                    
  if(last_var==0x1c){
    if(state.current_line==3 && state.num_next_line==0){
      hoh_debug("yaha to gaya hu..");
      for(int i=2;i<=24;i++){
        for(int j=0;j<=79;j++){
          writecharxy(j,i,scan_code[0x39],0,2,w,h,vgatext_base);
        }
      }
      char *p = "Screen got full, please type your command again...:)";
      for(int loc=0;*p;loc++,p++){
        vgatext::writechar(loc+w*(2),*p,0,3,vgatext_base);
      }
    }
    else if(state.which_fun==2){
      for(int i=0;i<state.result_args_size;i++){
        vgatext::writechar(i+w*(state.current_line-state.num_next_line+1),((char)state.result_args[i])+'0',0,3,vgatext_base);
      }
    }
    else if(state.which_fun==3){
      for(int i=0;i<state.result_args_size;i++){
        vgatext::writechar(i+w*(state.current_line-state.num_next_line+1),((char)state.result_args[i])+'0',0,3,vgatext_base);
      }
    }
    else if(state.which_fun==1){
      for(int loc=0;loc<state.echo_end_pointer;loc++){
        vgatext::writechar(loc+w*(state.current_line-state.num_next_line+1),state.echo_args[loc],0,3,vgatext_base);
      }
    }
    else if(state.which_fun==-1){
      char *p = "command not found...:(";
      for(int loc=0;*p;loc++,p++){
        vgatext::writechar(loc+w*(state.current_line-state.num_next_line+1),*p,0,3,vgatext_base);
      } 
    }
  } // Till here I handle the enter key...!!!
  else if(last_var==0x4b){}
  else if(last_var==0x4d){}
  else if(last_var==0x0e){ // This handles the Backspace
    writecharxy(state.current_pointer%78+2,state.current_line,scan_code[0x39],0,2,w,h,vgatext_base);  
  }
  else{
    if(state.current_pointer%78==0){
      writecharxy(79,state.current_line,scan_code[state.output_array[state.end_pointer-1]],0,2,w,h,vgatext_base);         
    }  
     else{hoh_debug("Current line is: "<<state.current_line);
      writecharxy(state.current_pointer%78-1+2,state.current_line,scan_code[state.output_array[state.end_pointer-1]],0,2,w,h,vgatext_base);
    }
  }
}

// Helper functions...

static void writecharxy(int x, int y, uint8_t c, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  vgatext::writechar(y*w+x,c,bg,fg,vgatext_base);
}

static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int y=y0;y<y1;y++){
    for(int x=x0;x<x1;x++){
      writecharxy(x,y,0,bg,fg,w,h,vgatext_base);
    }
  }
}

static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){

  writecharxy(x0,  y0,  0xc9, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y0,  0xbb, bg,fg, w,h,vgatext_base);
  writecharxy(x0,  y1-1,0xc8, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y1-1,0xbc, bg,fg, w,h,vgatext_base);

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y0, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y1-1, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x0,y, 0xba, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x1-1,y, 0xba, bg,fg, w,h,vgatext_base);
  }
}

static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int i=0;i<maxw;i++){
    writecharxy(x+i,y,str[i],bg,fg,w,h,vgatext_base);
    if(!str[i]){
      break;
    }
  }
}

static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  enum {max=sizeof(uint32_t)*2+1};
  char a[max];
  for(int i=0;i<max-1;i++){
    a[max-1-i-1]=hex2char(number%16);
    number=number/16;
  }
  a[max-1]='\0';

  drawtext(x,y,a,maxw,bg,fg,w,h,vgatext_base);
}

