/*
 ============================================================================
 Name        : cjson.c
 Author      : Carlos Alvarez
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "crimson.h"

#define _skip_spaces(x)		while(isspace(*x) && *x != '\0') x++

/* Private methods */
static JSON_ARRAY *crimson_parse_array(char **str);



JSON_VALUE *crimson_new_value(JSON_VALUE_TYPE vtype, void *value)
{
	JSON_VALUE *new_value;
	new_value = calloc(1, sizeof(JSON_VALUE));

	switch (vtype)
	{
	case JSON_TYPE_NULL:
	case JSON_TYPE_FALSE:
	case JSON_TYPE_TRUE:
		new_value->val = NULL;
		break;
	case JSON_TYPE_STRING:
		new_value->val = strdup((char *) value);
		break;
	case JSON_TYPE_NUMBER_INT:
		new_value->val = calloc(1, sizeof(int));
		memcpy(new_value->val, value, sizeof(int));
		break;
	case JSON_TYPE_NUMBER_LONG:
		new_value->val = calloc(1, sizeof(long));
		memcpy(new_value->val, value, sizeof(long));
		break;
	case JSON_TYPE_NUMBER_FLOAT:
		new_value->val = calloc(1, sizeof(float));
		memcpy(new_value->val, value, sizeof(float));
		break;
	case JSON_TYPE_NUMBER_DOUBLE:
		new_value->val = calloc(1, sizeof(double));
		memcpy(new_value->val, value, sizeof(double));
		break;
	case JSON_TYPE_OBJECT:
	case JSON_TYPE_ARRAY:
		new_value->val = value;
		break;
	default:
		new_value->val = value;
	}
	new_value->type = vtype;

	return new_value;
}

JSON_PAIR *crimson_new_pair(const char *key, JSON_VALUE *value)
{
	if(key == NULL || strlen(key) < 1) return NULL;

	JSON_PAIR *new_pair;
	new_pair = calloc(1, sizeof(JSON_PAIR));
	new_pair->key = strdup(key);
	new_pair->value = value;

	return new_pair;
}

JSON_ARRAY *crimson_new_array(JSON_VALUE *value)
{
	JSON_ARRAY *new_array;
	new_array = calloc(1, sizeof(JSON_ARRAY));
	new_array->first_value = value;

	return new_array;
}

JSON_OBJECT *crimson_new_object(JSON_PAIR *pair)
{
	JSON_OBJECT *new_object;
	new_object = calloc(1, sizeof(JSON_OBJECT));
	new_object->first_pair = pair;

	return new_object;
}

int crimson_append_value(JSON_ARRAY *array, JSON_VALUE *value)
{
	if (array == NULL || value == NULL) return -1;

	if (array->first_value == NULL)
	{
		array->first_value = value;
		return 0;
	}

	JSON_VALUE *val = array->first_value;
	while (val->next != NULL) val = val->next;
	val->next = value;

	return 0;
}

int crimson_add_pair(JSON_OBJECT *object, JSON_PAIR *pair)
{
	if (object == NULL || pair == NULL) return -1;

	if (object->first_pair == NULL)
	{
		object->first_pair = pair;
		return 0;
	}

	JSON_PAIR *pr = object->first_pair;
	JSON_PAIR *bf = NULL;
	do
	{
		if (strcmp(pr->key, pair->key) == 0)
		{
			pair->next = pr->next;

			if (bf == NULL) object->first_pair = pair;
			else bf->next = pair;

			crimson_delete_pair(pr);
			return 1;
		}

		if (pr->next == NULL)
		{
			pr->next = pair;
			pair->next = NULL;
			break;
		}
		bf = pr;
		pr = pr->next;
	} while (pr != NULL);

	return 0;
}

int crimson_append_pair(JSON_OBJECT *object, JSON_PAIR *pair)
{
	if (object == NULL || pair == NULL) return -1;

	if (object->first_pair == NULL)
	{
		object->first_pair = pair;
		return 0;
	}

	JSON_PAIR *pr = object->first_pair;
	do
	{
		if (strcmp(pr->key, pair->key) == 0) return -2;

		if (pr->next == NULL)
		{
			pr->next = pair;
			pair->next = NULL;
			break;
		}
		pr = pr->next;
	} while (pr != NULL);

	return 0;
}

void crimson_delete_value(JSON_VALUE *value)
{
	if (value == NULL) return;

	switch (value->type) {
		case JSON_TYPE_NULL:
		case JSON_TYPE_FALSE:
		case JSON_TYPE_TRUE:
			break;
	case JSON_TYPE_STRING:
	case JSON_TYPE_NUMBER_INT:
	case JSON_TYPE_NUMBER_LONG:
	case JSON_TYPE_NUMBER_FLOAT:
	case JSON_TYPE_NUMBER_DOUBLE:
		if (value->val != NULL) free(value->val);
		break;
	case JSON_TYPE_OBJECT:
		crimson_delete_object(value->val);
		break;
	case JSON_TYPE_ARRAY:
		crimson_delete_array(value->val);
		break;
	default:
		break;
	}
	free(value);
	value = NULL;
}

void crimson_delete_array(JSON_ARRAY *array)
{
	if (array == NULL) return;

	JSON_VALUE *val = array->first_value;
	while (val != NULL)
	{
		JSON_VALUE *temp = val->next;
		crimson_delete_value(val);
		val = temp;
	}
	free(array);
	array = NULL;
}

void crimson_delete_pair(JSON_PAIR *pair)
{
	if (pair == NULL) return;

	if (pair->key != NULL) free(pair->key);
	if (pair->value != NULL) crimson_delete_value(pair->value);
	free(pair);
	pair = NULL;
}

void crimson_delete_object(JSON_OBJECT *object)
{
	if (object == NULL) return;

	JSON_PAIR *pair = object->first_pair;
	while (pair != NULL)
	{
		JSON_PAIR *temp = pair->next;
		crimson_delete_pair(pair);
		pair = temp;
	}
	free(object);
	object = NULL;
}

JSON_PAIR *crimson_get_pair(JSON_OBJECT *obj, const char *key)
{
	if (obj == NULL || key == NULL || strlen(key) < 1) return NULL;

	JSON_PAIR *pair = obj->first_pair;

	while (pair != NULL)
	{
		if (strcmp(key, pair->key) == 0) return pair;
		pair = pair->next;
	}

	return NULL;
}

int crimson_tostr_value(JSON_VALUE *value, FILE *stream, char **str)
{
	if (value == NULL || str == NULL) return -1;

	FILE *strm;
	size_t sz;

	if (stream == NULL) strm = open_memstream(str, &sz);
	else strm = stream;

	switch (value->type)
	{
	case JSON_TYPE_NULL:
		fprintf(strm, "null");
		break;
	case JSON_TYPE_FALSE:
		fprintf(strm, "false");
		break;
	case JSON_TYPE_TRUE:
		fprintf(strm, "true");
		break;
	case JSON_TYPE_STRING:
		if (value->val == NULL) fprintf(strm, "null");
		fprintf(strm, "\"%s\"", (char*)value->val);
		break;
	case JSON_TYPE_NUMBER_INT:
		if (value->val == NULL) fprintf(strm, "null");
		fprintf(strm, "%d", *((int*)value->val));
		break;
	case JSON_TYPE_NUMBER_LONG:
		if (value->val == NULL) fprintf(strm, "null");
		fprintf(strm, "%ld", *((long*)value->val));
		break;
	case JSON_TYPE_NUMBER_FLOAT:
		if (value->val == NULL) fprintf(strm, "null");
		fprintf(strm, "%f", *((float*)value->val));
		break;
	case JSON_TYPE_NUMBER_DOUBLE:
		if (value->val == NULL) fprintf(strm, "null");
		fprintf(strm, "%f", *((double*)value->val));
		break;
	case JSON_TYPE_OBJECT:
		if (value->val == NULL) fprintf(strm, "null");
		crimson_tostr_object((JSON_OBJECT*)value->val, strm, str);
		break;
	case JSON_TYPE_ARRAY:
		if (value->val == NULL) fprintf(strm, "null");
		crimson_tostr_array((JSON_ARRAY*) value->val, strm, str);
		break;
	}

	if (stream == NULL) fclose(strm);
	return 0;
}

int crimson_tostr_pair(JSON_PAIR *pair, FILE *stream, char **str)
{
	if (pair == NULL || pair->key == NULL || strlen(pair->key) < 1 || str == NULL) return -1;

	FILE *strm;
	size_t sz;

	if (stream == NULL) strm = open_memstream(str, &sz);
	else strm = stream;

	fprintf(strm, "\"%s\":", pair->key);
	crimson_tostr_value(pair->value, strm, str);

	if (stream == NULL) fclose(strm);

	return 0;
}

int crimson_tostr_array(JSON_ARRAY *array, FILE *stream, char **str)
{
	if (array == NULL || str == NULL) return -1;

	FILE *strm;
	size_t sz;

	if (stream == NULL) strm = open_memstream(str, &sz);
	else strm = stream;

	JSON_VALUE *val = array->first_value;

	fputc('[', strm);
	while (val != NULL)
	{
		crimson_tostr_value(val, strm, str);
		val = val->next;
		if(val != NULL) fputc(',', strm);
	}
	fputc(']', strm);

	if(stream == NULL) fclose(strm);

	return 0;
}

int crimson_tostr_object(JSON_OBJECT *object, FILE *stream, char **str)
{
	if (object == NULL || str == NULL) return -1;

	FILE *strm;
	size_t sz;

	if (stream == NULL) strm = open_memstream(str, &sz);
	else strm = stream;

	JSON_PAIR *pair = object->first_pair;

	fputc('{', strm);
	while (pair != NULL)
	{
		crimson_tostr_pair(pair, strm, str);
		pair = pair->next;
		if (pair != NULL) fputc(',', strm);
	}
	fputc('}', strm);

	if(stream == NULL) fclose(strm);

	return 0;
}

static JSON_VALUE *crimson_parse_null(char **str)
{
	if (str == NULL || *str == NULL) return NULL;

	if (strncmp(*str, "null", 4) == 0)
	{
		*str = (*str+4);
		JSON_VALUE *val = crimson_new_value(JSON_TYPE_NULL, NULL);
		return (val != NULL) ? val : NULL;
	}

	return NULL;
}

static JSON_VALUE *crimson_parse_false(char **str)
{
	if (str == NULL || *str == NULL) return NULL;

	if (strncmp(*str, "false", 5) == 0)
	{
		*str = (*str+5);
		JSON_VALUE *val = crimson_new_value(JSON_TYPE_FALSE, NULL);
		return (val != NULL) ? val : NULL;
	}

	return NULL;
}

static JSON_VALUE *crimson_parse_true(char **str)
{
	if (str == NULL || *str == NULL) return NULL;

	if (strncmp(*str, "true", 4) == 0)
	{
		*str = (*str+4);
		JSON_VALUE *val = crimson_new_value(JSON_TYPE_TRUE, NULL);
		return (val != NULL) ? val : NULL;
	}

	return NULL;
}

static JSON_VALUE *crimson_parse_string(char **str)
{
	if (str == NULL || *str == NULL) return NULL;

	if (**str != '"') return NULL;
	char *start = ++(*str);

	int i;
	for (i = 1; **str != '\0'; i++)
	{
		if (**str == '"') break;
		else if (iscntrl(**str)) return NULL;
		(*str)++;
	}
	if (**str == '\0') return NULL;

	(*str)++;
	char *s = strndup(start, i-1);
	if (s == NULL) return NULL;
	JSON_VALUE *val = crimson_new_value(JSON_TYPE_STRING, s);
	free(s);

	return val;
}

static JSON_VALUE *crimson_parse_number(char **str)
{
	if (str == NULL || *str == NULL) return NULL;

	char *tail;
	long l_val;

	JSON_VALUE *val;

	l_val = strtol(*str, &tail, 0);
	if (tail == *str) return NULL;
	if(*tail != '.')
	{
		val = crimson_new_value(JSON_TYPE_NUMBER_LONG, &l_val);
		*str = tail;
		return val;
	}

	double d_val;
	d_val = strtod(*str, &tail);
	if (tail == *str) return NULL;

	val = crimson_new_value(JSON_TYPE_NUMBER_DOUBLE, &d_val);
	*str = tail;
	return val;
}

static JSON_VALUE *crimson_parse_value(char **str)
{
	if (str == NULL || *str == NULL) return NULL;

	_skip_spaces((*str));
	if (**str == '\0') return NULL;

		 if (**str == 'n') return crimson_parse_null(str);
	else if (**str == 'f') return crimson_parse_false(str);
	else if (**str == 't') return crimson_parse_true(str);
	else if (**str == '"') return crimson_parse_string(str);
	else if (isdigit(**str) || **str == '-' || **str == '+') return crimson_parse_number(str);
	else if (**str == '{')
	{
		JSON_OBJECT *obj = crimson_parse_object(str);
		if (obj == NULL) return NULL;

		JSON_VALUE *val = crimson_new_value(JSON_TYPE_OBJECT, obj);
		return val;
	}
	else if (**str == '[')
	{
		JSON_ARRAY *array = crimson_parse_array(str);
		if (array == NULL) return NULL;

		JSON_VALUE *val = crimson_new_value(JSON_TYPE_OBJECT, array);
		return val;
	}

	return NULL;
}

static JSON_PAIR *crimson_parse_pair(char **str)
{
	if (str == NULL || *str == NULL) return NULL;

	_skip_spaces((*str));
	if (**str != '"') return NULL;
	(*str)++;

	int len = 0;
	char *endk = *str;
	while (*endk != '"')
	{
		if (*endk == '\\' || *endk == '\0') {*str = endk; return NULL;}
		len++;
		endk++;
	}

	char *key = strndup(*str, len);
	*str = endk;
	(*str)++;

	_skip_spaces((*str));

	if (**str != ':')
	{
		free(key);
		return NULL;
	}

	(*str)++;
	JSON_VALUE *val = crimson_parse_value(str);

	if (val == NULL)
	{
		free(key);
		return NULL;
	}

	JSON_PAIR *pair = crimson_new_pair(key, val);
	free(key);

	return pair;
}

static JSON_ARRAY *crimson_parse_array(char **str)
{
	if (str == NULL || *str == NULL) return NULL;

	_skip_spaces((*str));

	if (**str != '[') return NULL;
	(*str)++;


	JSON_ARRAY *array = crimson_new_array(NULL);

	_skip_spaces((*str));
	while (**str != ']' && **str != '\0')
	{
		JSON_VALUE *val = crimson_parse_value(str);
		if (val != NULL && crimson_append_value(array, val) == 0) {}
		else {crimson_delete_array(array); return NULL;}
		_skip_spaces((*str));
		if (**str != ',' && **str != ']') {crimson_delete_array(array); return NULL;}
	}
	if (**str != '\0') (*str)++;
	else {crimson_delete_array(array); return NULL;}

	return array;
}

JSON_OBJECT *crimson_parse_object(char **str)
{
	if (str == NULL || *str == NULL) return NULL;

	_skip_spaces((*str));

	if (**str != '{') return NULL;
	(*str)++;

	JSON_OBJECT *object = crimson_new_object(NULL);

	while (**str != '}' && **str != '\0')
	{
		JSON_PAIR *pair = crimson_parse_pair(str);
		if (pair != NULL && crimson_add_pair(object, pair) == 0) {}
		else {crimson_delete_object(object); return NULL;}
		_skip_spaces((*str));

		if (**str == ',') {(*str)++;  continue;}
		if (**str == '}') break;

		return NULL;
	}
	if (**str != '}') {crimson_delete_object(object); return NULL;}

	return object;
}

JSON_VALUE *crimson_get_value(JSON_OBJECT *obj, const char *key)
{
	JSON_PAIR *pair;
	if ((pair =crimson_get_pair(obj, key)) == NULL) return NULL;
	return pair->value;
}

char *crimson_get_value_str(JSON_OBJECT *obj, const char *key)
{
	JSON_PAIR *pair;
	char *res;

	if ((pair =crimson_get_pair(obj, key)) == NULL || pair->value == NULL) res = strdup("null");

	switch (pair->value->type)
	{
	case JSON_TYPE_NULL:
		res = strdup("null");
		break;
	case JSON_TYPE_FALSE:
		res = strdup("false");
		break;
	case JSON_TYPE_TRUE:
		res = strdup("true");
		break;
	case JSON_TYPE_STRING:
		res =strdup(pair->value->val);
		break;
	case JSON_TYPE_NUMBER_INT:
		asprintf(&res, "%d", *((int*)pair->value->val));
		break;
	case JSON_TYPE_NUMBER_LONG:
		asprintf(&res, "%ld", *((long*)pair->value->val));
		break;
	case JSON_TYPE_NUMBER_FLOAT:
		asprintf(&res, "%f", *((float*)pair->value->val));
		break;
	case JSON_TYPE_NUMBER_DOUBLE:
		asprintf(&res, "%f", *((double*)pair->value->val));
		break;
	default:
		res = strdup("null");
	}

	return res;
}











