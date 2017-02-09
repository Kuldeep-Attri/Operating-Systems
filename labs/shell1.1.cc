#include "labs/shell.h"
#include "labs/vgatext.h"

//
// initialize shellstate
//

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

int fact(int n){
  if(n<0){return -1;}
  if(n==0){return 1;}
  int c=1; 
  while(n>0){c*=n;n--;}
  return c;
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


static void writecharxy(int x, int y, uint8_t c, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);


void shell_init(shellstate_t& state){
  state.key_counter=0; // It keep tracks of number of key pressed
  state.end_pointer=0; // It keep tracks of end position
  state.space_pointer=0; // it keep track of the position of space in the line 
  state.args=-1; // This is the argument which will in the fact(_) and fibo(_) functions
  state.output=0; // Setting output equal to zero
  state.start_pointer=0;
  state.function=0;
  state.number_of_enter=0;
  state.main_end_pointer=0;
  state.echo_end_pointer=0;
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

    if(stateinout.end_pointer==0 && scankey==0x39){
      stateinout.key_counter+=1;
      stateinout.start_pointer+=1;
      stateinout.input_array[stateinout.end_pointer]=scankey;
      stateinout.end_pointer+=1;
    }
    else if(stateinout.input_array[stateinout.end_pointer-1]!=0x39 && scankey==0x39){
      stateinout.input_array[stateinout.end_pointer]=scankey;
      stateinout.storage[stateinout.main_end_pointer]=scankey;
      stateinout.main_end_pointer++;
      stateinout.end_pointer+=1;
      stateinout.key_counter+=1;
      if(scankey==0x39){stateinout.space_pointer=stateinout.end_pointer-1;}
    }
    else if(stateinout.input_array[stateinout.end_pointer-1]==0x39 && scankey!=0x39){
      stateinout.input_array[stateinout.end_pointer]=scankey;
      stateinout.storage[stateinout.main_end_pointer]=scankey;
      stateinout.main_end_pointer++;
      stateinout.end_pointer+=1;
      stateinout.key_counter+=1;
    }
    else if(stateinout.input_array[stateinout.end_pointer-1]!=0x39 && scankey!=0x39){
      stateinout.input_array[stateinout.end_pointer]=scankey;
      stateinout.storage[stateinout.main_end_pointer]=scankey;
      stateinout.main_end_pointer++;
      stateinout.end_pointer+=1;
      stateinout.key_counter+=1;
    }
    else{
      stateinout.key_counter+=1;
    }
    if(scankey==0x1c){stateinout.number_of_enter+=1;}

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
  if(stateinout.input_array[stateinout.end_pointer-1]==0x1c){
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
      stateinout.output = fact(stateinout.args);
      hoh_debug("Args is: "<<stateinout.args);
      hoh_debug("result is: "<<stateinout.output);
      stateinout.function=2;

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
      stateinout.output = fibo(stateinout.args);
      stateinout.function=3;
    }
    else if(stateinout.input_array[stateinout.start_pointer]==0x12 && stateinout.input_array[stateinout.start_pointer+1]==0x2e && stateinout.input_array[stateinout.start_pointer+2]==0x23 && stateinout.input_array[stateinout.start_pointer+3]==0x18 && stateinout.space_pointer-stateinout.start_pointer==4){
      hoh_debug("running echo...");
      int i=stateinout.space_pointer+1;
      while(i<stateinout.end_pointer-1){
        stateinout.echo_args[stateinout.echo_end_pointer]=scan_code[stateinout.input_array[i]];
        hoh_debug("checking echo: "<<scan_code[stateinout.input_array[i]]);
        i++;
        stateinout.echo_end_pointer++;
      }
      stateinout.function=1;
    }
    else{
      hoh_debug("Sorry dude!!!, I didn't understand your command");
      stateinout.function=-1;
    }
    stateinout.end_pointer=0;
    stateinout.start_pointer=stateinout.end_pointer;
    stateinout.space_pointer=0;
    for(int i=0;i<1024;i++){
      stateinout.input_array[i]=0;
    }
  }
}
//
// shellstate --> renderstate
//
void shell_render(const shellstate_t& shell, renderstate_t& render){
  if(shell.storage[shell.main_end_pointer-1]==0x1c){
    render.output_array[render.end_pointer]=shell.storage[shell.main_end_pointer-1];
    render.starter=0;
    render.end_pointer++;
  }
  else{
    render.output_array[shell.end_pointer-1]=shell.input_array[shell.end_pointer-1];
    render.end_pointer=shell.end_pointer;
    render.starter=render.end_pointer; 
  }
  render.result=shell.output;
  render.key_counter=shell.key_counter;
  render.which_fun=shell.function;
  render.number_of_enter=shell.number_of_enter;
  render.echo_end_pointer=shell.echo_end_pointer;
  for(int i=0;i<render.echo_end_pointer;i++){
    render.echo_args[i]=shell.echo_args[i];
  }
}

bool render_eq(const renderstate_t& a, const renderstate_t& b){
  if(a.echo_end_pointer==b.echo_end_pointer && a.result==b.result && a.which_fun==b.which_fun){
    if(a.echo_end_pointer==b.echo_end_pointer && a.key_counter==b.key_counter && a.number_of_enter==b.number_of_enter && a.starter==b.starter){
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
        
  char *p = "$ ";
  for(int loc=0;*p;loc++,p++){
    vgatext::writechar(loc+w*(state.starter/80+3+2*state.number_of_enter),*p,0,2,vgatext_base);
  }                    
  if(state.output_array[state.end_pointer-1]!=0x1c){
    writecharxy(state.starter%80-1+2,state.starter/80+3+2*state.number_of_enter,scan_code[state.output_array[state.end_pointer-1]],0,2,w,h,vgatext_base);
  }

  if(state.output_array[state.end_pointer-1]==0x1c && state.which_fun==2){
    char key_counter[100];
    int temp=state.result;
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
    for(int i=0;i<size_key_counter;i++){
      vgatext::writechar(i+w*(2+2*state.number_of_enter),key_counter[i],0,3,vgatext_base);
    }

  }
  else if(state.output_array[state.end_pointer-1]==0x1c && state.which_fun==3){
    char key_counter[100];
    int temp=state.result;
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
    for(int i=0;i<size_key_counter;i++){
      vgatext::writechar(i+w*(2+2*state.number_of_enter),key_counter[i],0,3,vgatext_base);
    }
  }
  else if(state.output_array[state.end_pointer-1]==0x1c && state.which_fun==1){
    for(int loc=0;loc<state.echo_end_pointer;loc++){
        vgatext::writechar(loc+w*(2*(state.end_pointer/80)+2+2*state.number_of_enter),state.echo_args[loc],0,3,vgatext_base);
    }
  }
  else if(state.output_array[state.end_pointer-1]==0x1c && state.which_fun==-1){
    char *p = "command not found...:(";
    for(int loc=0;*p;loc++,p++){
      vgatext::writechar(loc+w*(2*(state.end_pointer/80)+2+2*state.number_of_enter),*p,0,3,vgatext_base);
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

