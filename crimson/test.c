/*
 * main.c
 *
 *  Created on: Jun 5, 2015
 *      Author: carlos
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "crimson.h"

int main(void) {

	FILE *tst;
	tst = fopen("JSON", "r");
	char json[40000];
	memset(json, 0, 40000);
	fgets(json, 40000, tst);
	fclose(tst);
	//puts(json);

	float start = clock();
	int i;
	for (i=0; i < 10000; i++)
	{
		char *parser = json;
		JSON_OBJECT *obj = crimson_parse_object(&parser);

		/*if (obj == NULL)
		{
			printf("Error in char #%d, near:\n%s\n          ^", (int)(parser-json), parser-10);
			return 0;
		}*/

		char *tostr;
		crimson_tostr_object(obj, NULL, &tostr);
		//puts(tostr);
		free(tostr);
		crimson_delete_object(obj);
	}
	float end = clock();


	printf("Time: %.2f s\n", (float)((end - start)/1000000));


	return EXIT_SUCCESS;
}

