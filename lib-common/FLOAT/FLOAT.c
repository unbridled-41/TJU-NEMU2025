#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	//	nemu_assert(0);
	return((long long)1ll * a * b) >> 16ll;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	/* Dividing two 64-bit integers needs the support of another library
	 * `libgcc', other than newlib. It is a dirty work to port `libgcc'
	 * to NEMU. In fact, it is unnecessary to perform a "64/64" division
	 * here. A "64/32" division is enough.
	 *
	 * To perform a "64/32" division, you can use the x86 instruction
	 * `div' or `idiv' by inline assembly. We provide a template for you
	 * to prevent you from uncessary details.
	 *
	 *     asm volatile ("??? %2" : "=a"(???), "=d"(???) : "r"(???), "a"(???), "d"(???));
	 *
	 * If you want to use the template above, you should fill the "???"
	 * correctly. For more information, please read the i386 manual for
	 * division instructions, and search the Internet about "inline assembly".
	 * It is OK not to use the template above, but you should figure
	 * out another way to perform the division.
	 */
	// a * (1 << 16) / b
	// idiv : "64/32" division (or "32/16", "16/8")
	FLOAT q, r;
	asm volatile ("idiv %2" : "=a"(q), "=d"(r) : "r"(b), "a"(a << 16), "d"(a >> 16));

	/*another way:
	
		// the FLOAT quo of (a << 16) / b just is the max number q such that
		// q * b <= (a << 16)
		int sgn = (a > 0) ^ (b > 0);
		a = a > 0 ? a : -a;
		b = b > 0 ? b : -b;
		int q = 0;
		long long _a = (long long)a << 16ll;
		long long _b = (long long)b;
		for(int k = 30; k >= 0; k--){
			if((long long)q * b + (b << k) <= a)
				q |= (1 << k);
		}
		if(sgn)q = -q;
	*/
	//	nemu_assert(0);
	return q;
}
union {
	float f;
	unsigned u;
}f2u;
FLOAT f2F(float a) {
	/* You should figure out how to convert `a' into FLOAT without
	 * introducing x87 floating point instructions. Else you can
	 * not run this code in NEMU before implementing x87 floating
	 * point instructions, which is contrary to our expectation.
	 *
	 * Hint: The bit representation of `a' is already on the
	 * stack. How do you retrieve it to another variable without
	 * performing arithmetic operations on it directly?
	 */
	f2u.f = a;
	unsigned x = f2u.u;
	FLOAT b = 0;
	int frac = x & ((1u << 23) - 1);
	int exp = (x>>23) & ((1u << 8) - 1);
	if(exp == 0){
		b = 0;
	}else if(exp == 255){
		//panic("NAN or INF!");
	}else{
		exp -= 127 + 7;
		frac += (1 << 23);
		if(exp < -23)
		      b = 0;
		else if(exp > 7);
		      //panic("float too big!");
		else if(exp > 0)
		      b = frac << exp;
		else 
		      b = frac >> (-exp);
	}
	if(x >> 31)b = -b;
	//	nemu_assert(0);
	return b;
}

FLOAT Fabs(FLOAT a) {
	//	nemu_assert(0);
	return a > 0 ? a : -a;
}

/* Functions below are already implemented */

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

