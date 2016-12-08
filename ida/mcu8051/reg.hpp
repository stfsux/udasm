
#ifndef __REG_HPP__
#define __REG_HPP__

#define REG(x) i8051_arch.regname[x]

extern const char* const mcu8051_regname[];

enum
{
  I8051_REG_VCS=I8051_REG_AB+1,
  I8051_REG_VDS
};
 
#endif


