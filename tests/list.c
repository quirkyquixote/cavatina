/*
 * Test for list
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "../cavatina.h"

struct node {
	struct kv_node node;
	int data;
};

int main(int argc, char *argv[])
{
	struct kv_list list;
	struct node nodes[10];
	struct kv_node *iter;
	int i;

	for (i = 0; i < 10; ++i)
		nodes[i].data = i;

	kv_list_init(&list);
	for (i = 0; i < 10; ++i)
		kv_list_add(&list, &nodes[i].node);
	kv_foreach(iter, &list)
		printf("%d\n", ((struct node *)iter)->data);
	for (i = 1; i < 10; i += 2)
		kv_list_remove(&list, &nodes[i].node);
	kv_foreach(iter, &list)
		printf("%d\n", ((struct node *)iter)->data);

	kv_list_init(&list);
	for (i = 0; i < 10; ++i)
		kv_list_add(&list, &nodes[i].node);
	kv_foreach(iter, &list)
		printf("%d\n", ((struct node *)iter)->data);
	for (i = 0; i < 10; i += 2)
		kv_list_remove(&list, &nodes[i].node);
	kv_foreach(iter, &list)
		printf("%d\n", ((struct node *)iter)->data);

	return 0;
}

