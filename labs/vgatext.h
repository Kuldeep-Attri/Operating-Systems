#include "util/io.h"

namespace vgatext{

   static inline void writechar(int loc, uint8_t c, uint8_t bg, uint8_t fg, addr_t base){
     //your code goes here
	int var_16 =((uint16_t)c)|((uint16_t)fg<<8)|((uint16_t)bg<<12); 
	mmio::write16(base,loc*2,var_16);  
   }

}//namespace vgatext
