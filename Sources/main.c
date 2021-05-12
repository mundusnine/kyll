#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "md4c-html.h"
#include "tinydir.h"



struct membuffer {
	char* data;
	size_t asize;
	size_t size;
};

typedef struct Settings {
	char* in;
	char* out;
	char* css;
	int fullHtml;
	int codeHighlight;
	char* codeTheme;
} Settings_t;


typedef struct String {
	char* data;
	int length;
} String_t;

//To document later
#define Docs(...)

#define LARGE_STRING_SIZE 2048 * 2048

#define PATH_MAX 1024

#ifdef WIN32 
#define SEPARATOR '\\'
#else
SEPERATOR '/'
#endif

#ifdef WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
MKDIR(path) mkdir(path,0)
#endif


/* Global options. */
static unsigned parser_flags = 0;
static FILE* out = NULL;
#ifndef MD4C_USE_ASCII
    static unsigned renderer_flags = MD_HTML_FLAG_DEBUG | MD_HTML_FLAG_SKIP_UTF8_BOM;
#else
    static unsigned renderer_flags = MD_HTML_FLAG_DEBUG;
#endif


TCHAR temp_string[PATH_MAX];


static void
membuf_init(struct membuffer* buf, MD_SIZE new_asize)
{
    buf->size = 0;
    buf->asize = new_asize;
    buf->data = malloc(buf->asize);
    if(buf->data == NULL) {
        fprintf(stderr, "membuf_init: malloc() failed.\n");
        exit(1);
    }
}

static void
membuf_grow(struct membuffer* buf, size_t new_asize)
{
    buf->data = realloc(buf->data, new_asize);
    if(buf->data == NULL) {
        fprintf(stderr, "membuf_grow: realloc() failed.\n");
        exit(1);
    }
    buf->asize = new_asize;
}

static void
membuf_append(struct membuffer* buf, const char* data, MD_SIZE size)
{
    if(buf->asize < buf->size + size)
        membuf_grow(buf, buf->size + buf->size / 2 + size);
    memcpy(buf->data + buf->size, data, size);
    buf->size += size;
}

void process_output(const MD_CHAR *output, MD_SIZE size, void * userdata){
	membuf_append((struct membuffer*) userdata, output, size);
}

void fixpath(char* dest,size_t len) {
	if (strstr(dest, "/") != NULL) {
		for (int i = 0; i < len; i++) {
			if (dest[i] == '/') {
				dest[i] = '\\';
			}
		}
	}

	///WARNING: HERE BE DRAGONS
	if (dest[0] == '.') {
		char temp[PATH_MAX] = {0};
		getcwd(temp, PATH_MAX);
		int i = 0;
		dest[0] = SEPARATOR;
		if (dest[1] == '.')
		{
			dest[0] = 0;
			dest[1] = SEPARATOR;
			i = 1;
		}
		strcat(temp,&dest[i]);
		memcpy(dest,temp,strlen(temp));
	}
}
void process_args(int argc, char** argv, Settings_t* opts) {
	opts->fullHtml = 1;
	for (int i = 1; i < argc; i++) {
		if (strstr(argv[i], "-i") != NULL) {
			int len = strlen(argv[i + 1]);
			opts->in = malloc(PATH_MAX);
			if (opts->in != NULL) {
				memcpy(opts->in, argv[i + 1], len);
				fixpath(opts->in, len);
				opts->in[len] = 0;
			}

		}
		else if (strstr(argv[i], "-o") != NULL) {
			const char* ext = opts->in != NULL && strstr(opts->in, ".md") != NULL ? ".html" : "";
			int len = strlen(argv[i + 1]);
			opts->out = malloc(PATH_MAX);
			if (strstr(argv[i + 1], ext) != NULL) {
				if (opts->out != NULL) {
					memcpy(opts->out, argv[i + 1], len);
					fixpath(opts->out, len);
					opts->out[len] = 0;
				}
			}
			else {
				int extlen = strlen(ext);
				if (opts->out != NULL) {
					memcpy(opts->out, argv[i + 1], len);
					memcpy(opts->out+len, ext, extlen);
					fixpath(opts->in, len);
					opts->out[len + extlen] = 0;
				}
			}
			if (opts->out == NULL) {
				fprintf(stderr, "Error: not enough memory available,uninstall your electron apps or chrome");
			}	
		}
		else if (strstr(argv[i], "-t") != NULL) {

		}
	}
}


void resize_string(String_t* last, size_t size) {
	if(last->length < size){
		char* newdata =  realloc(last->data,sizeof(char*) * size);
		if(newdata == NULL){
			fprintf(stderr,"Error not enough memory available, uninstall all electron apps or chrome");
			exit(1);
		}
		char* old = last->data;
		last->data = newdata;
		last->length = size;
		if(old != NULL)
			free(old);
	}
	last->data[size] = 0;
	
	
}

String_t* create_string(size_t size) {
	String_t* last = malloc(sizeof(String_t));
	if (last != NULL) {
		last->data = NULL;
		last->length = 0;
		if (size != 0) {
			resize_string(last, size);
		}
		return last;
	}
	fprintf(stderr, "Error could not allocate string of size %i", (int)size);
	return NULL;
	
}
String_t* file_contents = NULL;
void process_file(Settings_t* options) {

	//Open and create files
	FILE* f = fopen(options->in, "rb");
	if (f == NULL) {
		printf("Could not open file, exiting...");
		return 1;
	}

	out = fopen(options->out, "wb");

	//Get size of file
	size_t size;
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	resize_string(file_contents,size);
	int value = fread(file_contents->data, sizeof(char), size, f);
	if (value < size) {
		exit(1);
	}

	struct membuffer buf_out = { 0 };
	membuf_init(&buf_out, size + size / 8 + 64);

	if (md_html(file_contents->data, size, process_output, &buf_out, parser_flags, renderer_flags) == -1) {
		printf("Failed to parse file %s with md4c",options->in);
	}
	fwrite(buf_out.data, 1, buf_out.size, out);

	fclose(f);
	fclose(out);
}
void help() {
	printf("Use it wisely my son");
}

Docs("Convert to char on Windows. On unix we just memcpy.")
void to_char(char* dest, TCHAR* src) {
#ifdef WIN32
	size_t length = wcslen(src);
	wcstombs(dest, src, length);
#else
	size_t length = strlen(src);
	memcpy(dest, src, length);
#endif
	dest[length] = 0;
}

Docs("Convert to wchar on Windows. On unix we just memcpy.")
void to_wchar(TCHAR* dest, char* src) {
	size_t length = strlen(src);
#ifdef WIN32
	mbstowcs(dest, src, length);
#else
	memcpy(dest, src, length);
#endif
	dest[length] = 0;
}

int main(int argc, char** argv) {

	if (argc == 1) {
		help();
		exit(0);
	}
	//Parse arguments
	Settings_t options = { 0 };
	file_contents = create_string(LARGE_STRING_SIZE);
	process_args(argc, argv, &options);
	if (options.in != 0 && options.out != 0 && strstr(options.in, ".md") != NULL && strstr(options.out, ".html") != NULL) {
		process_file(&options);
	}
	else {
		tinydir_dir dir;
		
		to_wchar(temp_string, options.out);
		if (tinydir_open(&dir, temp_string) == -1)
		{
			MKDIR(options.out);
			if (tinydir_open(&dir, temp_string) == -1) {
				fprintf(stderr, "Error could not create path %s", options.out);
				goto bail;
			}
		}

		to_wchar(temp_string,options.in);
		if (tinydir_open(&dir, temp_string) == -1)
		{
			fprintf(stderr,"Error path %s not found",options.in);
			goto bail;
		}

		while (dir.has_next)
		{
			tinydir_file file;
			if (tinydir_readfile(&dir, &file) == -1)
			{
				perror("Error getting file");
				goto bail;
			}

			to_char(options.in, file.path);
			if (!file.is_dir && strstr(options.in,".md") != NULL )
			{
				to_wchar(temp_string, options.out);
				size_t len = strlen(options.out);
				options.out[len] = SEPARATOR;
				options.out[len + 1] = 0;
				strcat(options.out,options.in);
				for (int i = strlen(options.out); i > 0; i--) {
					if (options.out[i] == '.') {
						options.out[i] = 0;
						goto fixed;
					}
					options.out[i] = 0;
				}
			fixed:
				strcat(options.out,".html");

				process_file(&options);

				to_char(options.out, temp_string);
			}
			printf("\n");

			if (tinydir_next(&dir) == -1)
			{
				perror("Error getting next file");
				goto bail;
			}
			
		}

		bail:
			tinydir_close(&dir);
	}
	
	free(file_contents->data);
	free(file_contents);
	return 0;
}
