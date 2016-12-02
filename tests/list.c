/*
 * Test for list
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "../cavatina.h"

int main(int argc, char *argv[])
{
	struct kv_list list;
	struct kv_node nodes[10];
	struct kv_node *iter;
	int i, j;

	/* Create empty list */
	kv_list_init(&list);
	assert(list.first == NULL);
	assert(list.last == NULL);

	/* Fill list with all nodes */
	for (i = 0; i < 10; ++i) {
		kv_list_add(&list, &nodes[i]);
		/* Check manually */
		assert(list.first == &nodes[0]);
		assert(list.last == &nodes[i]);
		iter = list.first;
		for (j = 0; j <= i; ++j) {
			assert(iter == &nodes[j]);
			iter = iter->next;
		}
		assert(iter == NULL);
		/* Check with kv_foreach */
		j = 0;
		kv_foreach(iter, &list) {
			assert(iter == &nodes[j]);
			++j;
		}
	}

	/* Remove even nodes */
	for (i = 0; i < 10; i += 2)
		kv_list_remove(&list, &nodes[i]);
	/* Check manually */
	assert(list.first == &nodes[1]);
	assert(list.last == &nodes[9]);
	iter = list.first;
	for (i = 1; i < 10; i += 2) {
		assert(iter == &nodes[i]);
		iter = iter->next;
	}
	assert(iter == NULL);
	/* Check with kv_foreach */
	i = 1;
	kv_foreach(iter, &list) {
		assert(iter == &nodes[i]);
		i += 2;
	}

	/* Remove odd nodes */
	for (i = 1; i < 10; i += 2)
		kv_list_remove(&list, &nodes[i]);
	assert(list.first == NULL);
	assert(list.last == NULL);

	/* Fill again the list */
	for (i = 0; i < 10; ++i) {
		kv_list_add(&list, &nodes[i]);
		/* Check manually */
		assert(list.first == &nodes[0]);
		assert(list.last == &nodes[i]);
		iter = list.first;
		for (j = 0; j <= i; ++j) {
			assert(iter == &nodes[j]);
			iter = iter->next;
		}
		assert(iter == NULL);
		/* Check with kv_foreach */
		j = 0;
		kv_foreach(iter, &list) {
			assert(iter == &nodes[j]);
			++j;
		}
	}

	/* Remove even nodes */
	for (i = 1; i < 10; i += 2)
		kv_list_remove(&list, &nodes[i]);
	/* Check manually */
	assert(list.first == &nodes[0]);
	assert(list.last == &nodes[8]);
	iter = list.first;
	for (i = 0; i < 10; i += 2) {
		assert(iter == &nodes[i]);
		iter = iter->next;
	}
	assert(iter == NULL);
	/* Check with kv_foreach */
	i = 0;
	kv_foreach(iter, &list) {
		assert(iter == &nodes[i]);
		i += 2;
	}

	/* Remove odd nodes */
	for (i = 0; i < 10; i += 2)
		kv_list_remove(&list, &nodes[i]);
	assert(list.first == NULL);
	assert(list.last == NULL);
	return 0;
}

