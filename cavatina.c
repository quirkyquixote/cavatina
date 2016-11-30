
#include "cavatina.h"

#include <stdio.h>
#include <stdlib.h>

int kv_parse_group(struct kv_parser *parser, const char **buf);
int kv_parse_comment(struct kv_parser *parser, const char **buf);
int kv_parse_key(struct kv_parser *parser, const char **buf);
int kv_parse_val(struct kv_parser *parser, const char **buf);

void kv_init(struct kv_parser *parser)
{
	memset(parser, 0, sizeof(*parser));
}

void kv_destroy(struct kv_parser *parser)
{
	struct kv_group *group, *next_group;
	struct kv_key *key, *next_key;
	struct kv_val *val, *next_val;

	group = parser->first_group;
	while (group != NULL) {
		next_group = group->next;
		key = group->first_key;
		while (key != NULL) {
			next_key = key->next;
			val = key->first_val;
			while (val != NULL) {
				next_val = val->next;
				free(val);
				val = next_val;
			}
			free(key);
			key = next_key;
		}
		free(group);
		group = next_group;
	}
}

int kv_parse(struct kv_parser *parser, const char *buf)
{
	struct kv_str group;

	for (;;) {
		while (*buf == ' ' || *buf == '\t')
			++buf;
		if (*buf == 0) {
			return 0;
		} else if (*buf == '[') {
			if (kv_parse_group(parser, &buf) != 0)
				return -1;
		} else if (*buf == '#' || *buf == '\n') {
			if (kv_parse_comment(parser, &buf) != 0)
				return -1;
		} else {
			if (kv_parse_key(parser, &buf) != 0)
				return -1;
			if (kv_parse_val(parser, &buf) != 0)
				return -1;
		}
	}
}

int kv_strcmp(struct kv_str s1, struct kv_str s2)
{
	int l1, l2;
	l1 = s1.end - s1.begin;
	l2 = s2.end - s2.begin;
	if (l2 < l1)
		l1 = l2;
	return strncmp(s1.begin, s2.begin, l1);
}

struct kv_group *kv_get_group(struct kv_parser *parser, struct kv_str name)
{
	struct kv_group *group;
	group = parser->first_group;
	while (group != NULL) {
		if (kv_strcmp(group->name, name) == 0)
			return group;
		group = group->next;
	}
	return NULL;
}

struct kv_group *kv_add_group(struct kv_parser *parser, struct kv_str name)
{
	struct kv_group *group;
	group = kv_get_group(parser, name);
	if (group == NULL) {
		group = calloc(1, sizeof(*group));
		group->name = name;
		if (parser->last_group)
			parser->last_group->next = group;
		else
			parser->first_group = group;
		parser->last_group = group;
	}
	return group;
}

struct kv_key *kv_get_key(struct kv_group *group, struct kv_str name)
{
	struct kv_key *key;
	key = group->first_key;
	while (key != NULL) {
		if (kv_strcmp(key->name, name) == 0)
			return key;
		key = key->next;
	}
	return NULL;
}

struct kv_key *kv_add_key(struct kv_group *group, struct kv_str name)
{
	struct kv_key *key;
	key = kv_get_key(group, name);
	if (key == NULL) {
		key = calloc(1, sizeof(*key));
		key->name = name;
		if (group->last_key)
			group->last_key->next = key;
		else
			group->first_key = key;
		group->last_key = key;
	}
	return key;
}

struct kv_val *kv_get_val(struct kv_key *key, struct kv_str name)
{
	struct kv_val *val;
	val = key->first_val;
	while (val != NULL) {
		if (kv_strcmp(val->name, name) == 0)
			return val;
		val = val->next;
	}
	return NULL;
}

struct kv_val *kv_add_val(struct kv_key *key, struct kv_str name)
{
	struct kv_val *val;
	val = kv_get_val(key, name);
	if (val == NULL) {
		val = calloc(1, sizeof(*val));
		val->name = name;
		if (key->last_val)
			key->last_val->next = val;
		else
			key->first_val = val;
		key->last_val = val;
	}
	return val;
}

int kv_parse_group(struct kv_parser *parser, const char **buf)
{
	struct kv_str str;

	str.begin = *buf + 1;
	str.end = strchr(str.begin, ']');
	if (str.end == NULL) {
		fprintf(stderr, "Expected '['");
		return -1;
	}
	*buf = str.end + 1;
	parser->cur_group = kv_add_group(parser, str);
	return 0;
}

int kv_parse_comment(struct kv_parser *parser, const char **buf)
{
	struct kv_str str;

	str.begin = *buf;
	str.end = strchr(str.begin, '\n');
	if (str.end == NULL) {
		str.end = str.begin + strlen(str.begin);
		*buf = str.end;
	} else {
		*buf = str.end + 1;
	}
	return 0;
}

int kv_parse_key(struct kv_parser *parser, const char **buf)
{
	struct kv_str str;

	str.begin = *buf;
	str.end = str.begin;
	while (strchr(" \t\n\r=", *str.end) == NULL)
		++str.end;
	*buf = str.end;
	parser->cur_key = kv_add_key(parser->cur_group, str);
	return 0;
}

int kv_parse_val(struct kv_parser *parser, const char **buf)
{
	struct kv_str str;

	while (**buf == ' ' || **buf == '\t')
		++*buf;
	if (**buf != '=') {
		fprintf(stderr, "Expected '='\n");
		return -1;
	}
	++*buf;
	while (**buf == ' ' || **buf == '\t')
		++*buf;
	str.begin = *buf;
	str.end = strchr(str.begin, '\n');
	*buf = str.end + 1;
	parser->cur_val = kv_add_val(parser->cur_key, str);
	return 0;
}

int kv_dump(struct kv_parser *parser)
{
	struct kv_group *group;
	struct kv_key *key;
	struct kv_val *val;
	const char *ptr;

	for (group = parser->first_group; group != NULL; group = group->next) {
		putchar('\n');
		putchar('[');
		for (ptr = group->name.begin; ptr != group->name.end; ++ptr)
			putchar(*ptr);
		putchar(']');
		putchar('\n');
		for (key = group->first_key; key != NULL; key = key->next) {
			for (val = key->first_val; val != NULL; val = val->next) {
				for (ptr = key->name.begin; ptr != key->name.end; ++ptr)
					putchar(*ptr);
				putchar('=');
				for (ptr = val->name.begin; ptr != val->name.end; ++ptr)
					putchar(*ptr);
				putchar('\n');
			}
		}
	}
}

