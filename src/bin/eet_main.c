#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <Eet.h>

static void
do_eet_list(const char *file)
{
   int i, num;
   char **list;
   Eet_File *ef;
   
   ef = eet_open(file, EET_FILE_MODE_READ);
   if (!ef)
     {
	printf("cannot open for reading: %s\n", file);
	exit(-1);
     }
   list = eet_list(ef, "*", &num);
   if (list)
     {
	for (i = 0; i < num; i++)
	  printf("%s\n",list[i]);
	free(list);
     }
   eet_close(ef);
}

static void
do_eet_extract(const char *file, const char *key, const char *out)
{
   Eet_File *ef;
   void *data;
   int size = 0;
   FILE *f;
   
   ef = eet_open(file, EET_FILE_MODE_READ);
   if (!ef)
     {
	printf("cannot open for reading: %s\n", file);
	exit(-1);
     }
   data = eet_read(ef, key, &size);
   if (!data)
     {
	printf("cannot read key %s\n", key);
	exit(-1);
     }
   f = fopen(out, "w");
   if (!f)
     {
	printf("cannot open %s\n", out);
	exit(-1);
     }
   if (fwrite(data, size, 1, f) != 1)
     {
	printf("cannot write to %s\n", out);
	exit(-1);
     }
   fclose(f);
   free(data);
   eet_close(ef);
}

static void
do_eet_decode_dump(void *data, const char *str)
{
   fputs(str, (FILE *)data);
}

static void
do_eet_decode(const char *file, const char *key, const char *out)
{
   Eet_File *ef;
   void *data;
   int size = 0;
   FILE *f;
   
   ef = eet_open(file, EET_FILE_MODE_READ);
   if (!ef)
     {
	printf("cannot open for reading: %s\n", file);
	exit(-1);
     }
   data = eet_read(ef, key, &size);
   if (!data)
     {
	printf("cannot read key %s\n", key);
	exit(-1);
     }
   f = fopen(out, "w");
   if (!f)
     {
	printf("cannot open %s\n", out);
	exit(-1);
     }
   if (!eet_data_text_dump(data, size, do_eet_decode_dump, f))
     {
	printf("cannot write to %s\n", out);
	exit(-1);
     }
   fclose(f);
   free(data);
   eet_close(ef);
}

static void
do_eet_insert(const char *file, const char *key, const char *out, int compress)
{
   Eet_File *ef;
   void *data;
   int size = 0;
   FILE *f;
   
   ef = eet_open(file, EET_FILE_MODE_READ_WRITE);
   if (!ef)
     ef = eet_open(file, EET_FILE_MODE_WRITE);
   if (!ef)
     {
	printf("cannot open for read+write: %s\n", file);
	exit(-1);
     }
   f = fopen(out, "r");
   if (!f)
     {
	printf("cannot open %s\n", out);
	exit(-1);
     }
   fseek(f, 0, SEEK_END);
   size = ftell(f);
   rewind(f);
   data = malloc(size);
   if (!data)
     {
	printf("cannot allocate %i bytes\n", size);
	exit(-1);
     }
   if (fread(data, size, 1, f) != 1)
     {
	printf("cannot read file %s\n", out);
	exit(-1);
     }
   fclose(f);
   eet_write(ef, key, data, size, compress);
   free(data);
   eet_close(ef);
}

static void
do_eet_encode(const char *file, const char *key, const char *out, int compress)
{
   Eet_File *ef;
   char *text;
   int textlen = 0;
   void *data;
   int size = 0;
   FILE *f;
   
   ef = eet_open(file, EET_FILE_MODE_READ_WRITE);
   if (!ef)
     ef = eet_open(file, EET_FILE_MODE_WRITE);
   if (!ef)
     {
	printf("cannot open for read+write: %s\n", file);
	exit(-1);
     }
   f = fopen(out, "r");
   if (!f)
     {
	printf("cannot open %s\n", out);
	exit(-1);
     }
   fseek(f, 0, SEEK_END);
   textlen = ftell(f);
   rewind(f);
   text = malloc(textlen);
   if (!text)
     {
	printf("cannot allocate %i bytes\n", size);
	exit(-1);
     }
   if (fread(text, textlen, 1, f) != 1)
     {
	printf("cannot read file %s\n", out);
	exit(-1);
     }
   fclose(f);
   data = eet_data_text_undump(text, textlen, &size);
   if (!data)
     {
        printf("cannot parse %s\n", out);
	exit(-1);
     }
   eet_write(ef, key, data, size, compress);
   free(text);
   free(data);
   eet_close(ef);
}

static void
do_eet_remove(const char *file, const char *key)
{
   Eet_File *ef;
   
   ef = eet_open(file, EET_FILE_MODE_READ_WRITE);
   if (!ef)
     {
	printf("cannot open for read+write: %s\n", file);
	exit(-1);
     }
   eet_delete(ef, key);
   eet_close(ef);
}

int
main(int argc, char **argv)
{
   eet_init();
   if (argc < 2)
     {
	printf("Usage:\n"
	       "  eet -l FILE.EET                      list all keys in FILE.EET\n"
	       "  eet -x FILE.EET KEY OUT-FILE         extract data stored in KEY in FILE.EET and write to OUT-FILE\n"
	       "  eet -d FILE.EET KEY OUT-FILE         extract and decode data stored in KEY in FILE.EET and write to OUT-FILE\n"
	       "  eet -i FILE.EET KEY IN-FILE COMPRESS insert data to KEY in FILE.EET from IN-FILE and if COMPRESS is 1, compress it\n"
	       "  eet -e FILE.EET KEY IN-FILE COMPRESS insert and encode to KEY in FILE.EET from IN-FILE and if COMPRESS is 1, compress it\n"
	       "  eet -r FILE.EET KEY                  remove KEY in FILE.EET\n"
	       );
	eet_shutdown();
	return 0;
     }
   if ((!strcmp(argv[1], "-l")) && (argc > 2))
     {
	do_eet_list(argv[2]);
     }
   else if ((!strcmp(argv[1], "-x")) && (argc > 4))
     {
	do_eet_extract(argv[2], argv[3], argv[4]);
     }
   else if ((!strcmp(argv[1], "-d")) && (argc > 4))
     {
	do_eet_decode(argv[2], argv[3], argv[4]);
     }
   else if ((!strcmp(argv[1], "-i")) && (argc > 5))
     {
	do_eet_insert(argv[2], argv[3], argv[4], atoi(argv[5]));
     }
   else if ((!strcmp(argv[1], "-e")) && (argc > 5))
     {
	do_eet_encode(argv[2], argv[3], argv[4], atoi(argv[5]));
     }
   else if ((!strcmp(argv[1], "-r")) && (argc > 3))
     {
	do_eet_remove(argv[2], argv[3]);
     }
   eet_shutdown();
   return 0;
}