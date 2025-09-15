#ifndef __JCC_H__
#define __JCC_H__

make_helper(jbe_si_b);
make_helper(jnz_si_b);
make_helper(jbe_si_v);
make_helper(jle_si_b);
make_helper(jle_si_v);

make_helper(jg_si_b);
make_helper(jg_i_v);
make_helper(jl_si_b);
make_helper(jge_si_b);
make_helper(jge_si_v);
make_helper(ja_si_b);
make_helper(ja_si_v);
make_helper(jne_si_v);
make_helper(js_si_b);
make_helper(js_si_v);
make_helper(jns_si_b);

#endif
