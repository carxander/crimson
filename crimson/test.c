/*
 * main.c
 *
 *  Created on: Jun 5, 2015
 *      Author: carlos
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crimson.h"

int main(void) {

	JSON_VALUE *val;
	JSON_PAIR *pair;
	JSON_OBJECT *obj;
	JSON_ARRAY *array;

	val = crimson_new_value(JSON_TYPE_STRING, "Carlos");
	pair = crimson_new_pair("Name", val);
	obj = crimson_new_object(pair);

	val = crimson_new_value(JSON_TYPE_STRING, "Alvarez");
	pair = crimson_new_pair("Last_Name", val);

	crimson_add_pair(obj, pair);

	array = crimson_new_array(NULL);
	val = crimson_new_value(JSON_TYPE_STRING, "809-555-5555");
	crimson_append_value(array, val);
	val = crimson_new_value(JSON_TYPE_STRING, "809-333-3333");
	crimson_append_value(array, val);
	val = crimson_new_value(JSON_TYPE_STRING, "809-777-7777");
	crimson_append_value(array, val);
	val = crimson_new_value(JSON_TYPE_ARRAY, array);
	pair = crimson_new_pair("Phones", val);

	crimson_add_pair(obj, pair);

	val = crimson_new_value(JSON_TYPE_STRING, "Something avenue #1632, Bronx, NY, USA.");
	pair = crimson_new_pair("Address", val);

	crimson_add_pair(obj, pair);

	val = crimson_new_value(JSON_TYPE_TRUE, NULL);
	pair = crimson_new_pair("Married", val);

	crimson_add_pair(obj, pair);

	val = crimson_new_value(JSON_TYPE_NULL, NULL);
	pair = crimson_new_pair("Social_Security", val);

	crimson_add_pair(obj, pair);

	char *str = NULL;

	crimson_tostr_object(obj, NULL, &str);

	printf("%s", str);
	free(str);
	str = NULL;

	crimson_delete_object(obj);

	char* json = "        {\"Married\":false, \"Age\":24.8, \"Wife\":\"Sahudy\", \"Marriage_Date\":\"\\/Date(1433783698645-400)\\/\"}\n";

	char* parser = json;

	obj = crimson_parse_object(&parser);

	if (obj == NULL) {printf("\nError near:\n\t\t"); puts(parser); return 0;}
	else printf("\nDone\n");

	crimson_add_pair(obj, crimson_new_pair("Wife", crimson_new_value(JSON_TYPE_STRING, "Sharolyn")));
	crimson_append_string(obj, "BirthDate", "24/06/1990");

	char *res;
	crimson_tostr_object(obj, NULL, &res);
	puts(res);
	free(res);

	char *search = crimson_get_value_str(obj, "Age");
	puts(search);
	free(search);

	crimson_delete_object(obj);

	puts("END");
	return EXIT_SUCCESS;
}

