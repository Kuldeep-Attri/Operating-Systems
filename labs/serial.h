#pragma once

#include "util/io.h"

namespace serial{

  static inline bool is_transmitter_ready(io_t baseport){
        uint16_t var16 = io::read16(baseport,5);
	return (var16>>5 & 1);
}

  static inline void writechar(uint8_t c, io_t baseport){
        io::write16(baseport,0,(uint16_t)c); 
  }

} //end serial
