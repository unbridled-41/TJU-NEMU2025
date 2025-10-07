
#define S (1 << s)
#define B (1 << b)
#define t (27 - s -b)

cache_line **cache;
void concat(init_, cache)(){

	memset(all_mask, 1, 2 * 64);
	cache = (cache_line **)malloc(S * sizeof(cache_line *));
	int i, j;
	for (i = 0; i < S; i++){
		cache[i] = (cache_line *)malloc(E * sizeof(cache_line));
		for (j = 0; j < E; j++){
			cache[i][j].buf = (uint8_t *)malloc(B * sizeof(uint8_t));
			cache[i][j].valid = 0;
			cache[i][j].dirty = 0;
		}
	}
}

void concat(cache, _read)(hwaddr_t addr, uint8_t *buf){
	hwaddr_t addr_temp = addr;
	addr_temp >>= b;
	uint32_t set_index = addr_temp & (S - 1);
	uint32_t tag = (addr_temp >> s) &((1 << t) - 1);
	int i;
	for (i = 0; i < E; i++){
		if (cache[set_index][i].tag == tag && cache[set_index][i].valid){
			// hit:
			memcpy(buf, cache[set_index][i].buf, B);
			return;
		}
	}
	// miss:
//	printf("miss: %x %x\n",tag,set_index);
	for (i = 0; i < E; i++){
		if (!cache[set_index][i].valid){
			break;
		}
	}
	if (i == E){
		srand(0);
		i = rand() % E;
		if(use_dirty && cache[set_index][i].dirty){
			miss_write((set_index << b) + (cache[set_index][i].tag << (s + b)), cache[set_index][i].buf, all_mask);
		}
	}
	
	cache[set_index][i].valid = 1;
	cache[set_index][i].tag = tag;
	cache[set_index][i].dirty = 0;
	(miss_read)(addr & ~(B - 1),  cache[set_index][i].buf);
	memcpy(buf, cache[set_index][i].buf, B);
}

void concat(cache, _write)(hwaddr_t addr, uint8_t *buf, uint8_t *mask){
	hwaddr_t addr_temp = addr;
	addr_temp >>= b;
	uint32_t set_index = addr_temp & (S - 1);
	uint32_t tag = addr_temp >> s &((1 << t) - 1);
	int i;
	for (i = 0; i < E; i++){
		if (cache[set_index][i].tag == tag && cache[set_index][i].valid){
			// hit:
			memcpy_with_mask(cache[set_index][i].buf, buf, B, mask);
			if(use_dirty){
				cache[set_index][i].dirty = 1;
			}
			else{
				(miss_write)(addr & ~(B - 1),  buf, mask);
			}
			return;
		}
	}
	// miss:
	if(use_dirty){
		for (i = 0; i < E; i++){
			if (!cache[set_index][i].valid){
				break;
			}
		}
		if (i == E){
			srand(0);
			i = rand() % E;
			if(cache[set_index][i].dirty){
				(miss_write)((set_index << b) + (cache[set_index][i].tag << (s + b)), cache[set_index][i].buf, all_mask);
			}
		}

		cache[set_index][i].valid = 1;
		cache[set_index][i].tag = tag;
		(miss_read)(addr, cache[set_index][i].buf);
		memcpy_with_mask(cache[set_index][i].buf, buf, B, mask);
		cache[set_index][i].dirty = 1;
	}
	else{
		(miss_write)(addr & ~(B - 1),  buf, mask);
	}

}
#undef S
#undef B
#undef t
