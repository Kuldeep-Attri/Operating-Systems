#pragma once

#include "generated/lpc_kbd.dev.h"

namespace lpc_kbd{

  static inline bool has_key(lpc_kbd_t& dev){

    //insert your code here
	lpc_kbd_status_t var_8=lpc_kbd_status_rd(&dev);
	return (lpc_kbd_status_obf_extract(var_8));

  }

  static inline uint8_t get_key(lpc_kbd_t& dev){

    //insert your code here
	return lpc_kbd_input_rd(&dev);
  }

}// namespace lpc_kbd

