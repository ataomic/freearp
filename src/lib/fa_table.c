#include <fa_table.h>

__inline u32 fa_addr_hcode(const struct fa_addr* addr, u32 id)
{
	return 0;
}

int fa_addr_cmp(const struct fa_addr* a, const struct fa_addr* b)
{
	int ret = fa_int_cmp(a->type, b->type);
	if(ret != 0)
		return ret;
	switch(a->type) {
		case FA_ADDR_HW:
			return memcmp(a->u.hw, b->u.hw);
		case FA_ADDR_IP:
			return fa_int_cmp(a->u.ip, b->u.ip);
		case FA_ADDR_HOST:
			return stricmp(a->u.host, b->u.host);
	}

	return -1;
}

void fa_addr_copy(struct fa_addr* a, const struct fa_addr* b)
{
	a->type = b->type;
	a->owner = b->owner;
	switch(a->type) {
		case FA_ADDR_HW:
			memcpy(a->u.hw, b->u.hw, sizeof(a->u.hw));
			break;
		case FA_ADDR_IP:
			a->u.ip = b->u.ip;
			break;
		case FA_ADDR_HOST:
			a->u.host = strdup(b->u.host);
			break;
	}
}

void fa_addr_cleanup(struct fa_addr* a)
{
	switch(a->type) {
		case FA_ADDR_HOST:
			free(a->u.host);
			a->u.host = NULL;
			break;
	}
}

void fa_addr_replace(struct fa_addr* a, struct fa_addr* b)
{
	list_replace(&a->list, &b->list);
}

struct fa_addr* fa_table_find_addr(struct fa_table* table, u32 id,
	const struct fa_addr* addr)
{
	fa_addr* fa;
	u32 hindex = fa_addr_hcode(addr, id) % fa_table_size(table);
	list_t *bucket = &table->buckets[hindex], *pos;

	list_for_each(pos, bucket) {
		fa = fa_addr_entry(pos);
		if(fa_addr_equals(fa, addr))
			return fa;
	}

	return NULL;
}

void fa_table_insert_addr(struct fa_table* table, u32 id,
	struct fa_addr* addr)
{
	u32 hindex = fa_addr_hcode(addr, id) % fa_table_size(table);
	list_t* bucket = &table->buckets[hindex];
	
	list_add(&addr->list, bucket);
	table->count ++;
}

void fa_table_erase_addr(struct fa_table* table, u32 id,
	struct fa_addr* addr)
{
	list_del_init(&addr->list);
	table->count --;
}

int fa_table_insert_node(struct fa_table* table, struct fa_node* node)
{
	int i;

	for(i = 0; i < node->count; i ++) {
		addr = node->addr+i;
		if(!list_empty(&addr->list) &&
			fa_table_find_node(table, node->id, addr) != node) {
			return FA_DUP;
		}
	}

	for(i = 0; i < node->count; i ++) {
		if(!list_empty(&addr->list))
			fa_table_insert_addr(table, node->id, addr);
	}

	return FA_OK;
}

void fa_table_erase_node(struct fa_table* table, struct fa_node* node)
{
	int i;

	for(i = 0; i < node->count; i ++) {
		addr = node->addr+i;
		if(!list_empty(&addr->list))
			fa_table_erase_addr(table, addr);
	}
}

struct fa_addr* fa_node_find_addr(struct fa_node* node,
	const struct fa_addr* addr)
{
	int i;

	for(i = 0; i < node->count; i ++) {
		if(fa_addr_equals(node->addr+i, addr))
			return node->addr+i;
	}

	return NULL;
}

int fa_node_add_addr(struct fa_node* node, const struct fa_addr* addr)
{
	if(fa_node_find_addr(node, addr))
		return FA_DUP;
	if(node->count+1 > FA_ADDR_MAX)
		return FA_FAIL;
	
	fa_addr_copy(node->addr+node->count, addr);
	node->count ++;
	
	return FA_OK;
}

void fa_node_del_addr(struct fa_node* node, struct fa_addr* addr)
{
	int index;
	struct fa_addr* fb;

	index = fa_addr_index(node, addr);
	if(index < 0)
		return;
	
	fa_addr_cleanup(addr);
	if(index < node->count-1) {
		fb = node->addr+node->count-1;
		fa_addr_copy(addr, fb);
		fa_addr_replace(addr, fb);
		fa_addr_cleanup(fb);
	}
	node->count --;
}

