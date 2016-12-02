
#include <assert.h>

#include "../cavatina.h"

int main(int argc, char *argv[])
{
	struct kv_parser *p;
	struct kv_group *g1, *g2;
	struct kv_key *k1, *k2;
	struct kv_val *v1, *v2;

	p = kv_new();
	assert(p->groups.first == NULL);
	assert(p->groups.last == NULL);
	assert(p->cur_group == NULL);
	assert(p->cur_key == NULL);
	assert(p->cur_val == NULL);

	g1 = kv_add_group(p, kv_str("g.1"));
	assert(p->groups.first == &g1->node);
	assert(p->groups.last == &g1->node);
	assert(g1->node.next == NULL);
	assert(kv_strcmp(g1->name, kv_str("g.1")) == 0);

	g2 = kv_add_group(p, kv_str("g.2"));
	assert(p->groups.first == &g1->node);
	assert(p->groups.last == &g2->node);
	assert(g1->node.next == &g2->node);
	assert(g2->node.next == NULL);
	assert(kv_strcmp(g2->name, kv_str("g.2")) == 0);

	k1 = kv_add_key(g1, kv_str("key.1.1"));
	assert(g1->keys.first == &k1->node);
	assert(g1->keys.last == &k1->node);
	assert(k1->node.next == NULL);
	assert(kv_strcmp(k1->name, kv_str("key.1.1")) == 0);

	k2 = kv_add_key(g1, kv_str("key.1.2"));
	assert(g1->keys.first == &k1->node);
	assert(g1->keys.last == &k2->node);
	assert(k1->node.next == &k2->node);
	assert(k2->node.next == NULL);
	assert(kv_strcmp(k2->name, kv_str("key.1.2")) == 0);

	v1 = kv_add_val(k1, kv_str("val.1.1.1"));
	assert(k1->vals.first == &v1->node);
	assert(k1->vals.last == &v1->node);
	assert(v1->node.next == NULL);
	assert(kv_strcmp(v1->name, kv_str("val.1.1.1")) == 0);

	v2 = kv_add_val(k1, kv_str("val.1.1.2"));
	assert(k1->vals.first == &v1->node);
	assert(k1->vals.last == &v2->node);
	assert(v1->node.next == &v2->node);
	assert(v2->node.next == NULL);
	assert(kv_strcmp(v2->name, kv_str("val.1.1.2")) == 0);

	kv_destroy(p);
	p = kv_new();
	kv_add(p, kv_str("foo"), kv_str("bar"), kv_str("baz"));
	g1 = (void *)p->groups.first;
	assert(g1 != NULL);
	assert(g1 == (void *)p->groups.last);
	assert(kv_strcmp(g1->name, kv_str("foo")) == 0);
	k1 = (void *)g1->keys.first;
	assert(k1 != NULL);
	assert(k1 == (void *)g1->keys.last);
	assert(kv_strcmp(k1->name, kv_str("bar")) == 0);
	v1 = (void *)k1->vals.first;
	assert(v1 != NULL);
	assert(v1 == (void *)k1->vals.last);
	assert(kv_strcmp(v1->name, kv_str("baz")) == 0);
	return 0;
}
