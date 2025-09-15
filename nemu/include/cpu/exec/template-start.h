#include "cpu/exec/helper.h"

#if DATA_BYTE == 1

#define SUFFIX b
#define DATA_TYPE uint8_t
#define DATA_TYPE_S int8_t

#elif DATA_BYTE == 2

#define SUFFIX w
#define DATA_TYPE uint16_t
#define DATA_TYPE_S int16_t

#elif DATA_BYTE == 4

#define SUFFIX l
#define DATA_TYPE uint32_t
#define DATA_TYPE_S int32_t

#else

#error unknown DATA_BYTE

#endif

#define REG(index) concat(reg_, SUFFIX) (index)
#define REG_NAME(index) concat(regs, SUFFIX) [index]

#define MEM_R(addr, sreg) swaddr_read(addr, DATA_BYTE, sreg)                // Ìí¼ÓÁËsreg
#define MEM_W(addr, data, sreg) swaddr_write(addr, DATA_BYTE, data, sreg)

#define OPERAND_W(op, src) concat(write_operand_, SUFFIX) (op, src)

#define MSB(n) ((DATA_TYPE)(n) >> ((DATA_BYTE << 3) - 1))




#define PUSH(data) do{REG(4) -= DATA_BYTE; MEM_W(REG(4), data);}while(0)
#define PUSH_N(data, n) do{reg_l(4) -= n; swaddr_write(reg_l(4), n, data);}while(0)
#define POP(dst) do{dst = MEM_R(REG(4)); REG(4) += DATA_BYTE;}while(0)
#define POP_N(dst,n) do{dst = swaddr_read(reg_l(4),n); reg_l(4)+= 4;}while(0)