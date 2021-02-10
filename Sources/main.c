#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "md4c-html.h"

/* Global options. */
static unsigned parser_flags = 0;
static FILE* out = NULL;
#ifndef MD4C_USE_ASCII
    static unsigned renderer_flags = MD_HTML_FLAG_DEBUG | MD_HTML_FLAG_SKIP_UTF8_BOM;
#else
    static unsigned renderer_flags = MD_HTML_FLAG_DEBUG;
#endif

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

void process_args(int argc, char** argv, Settings_t* opts) {
	opts->fullHtml = 1;
	for (int i = 1; i < argc; i++) {
		if (strstr(argv[i], "-i") != NULL) {
			opts->in = argv[i + 1];
		}
		else if (strstr(argv[i], "-o") != NULL) {
			const char* ext = ".html";
			int len = strlen(argv[i + 1]);
			if (strstr(argv[i + 1], ext) != NULL) {
				opts->out = malloc(len);
				if (opts->out != NULL) {
					memcpy(opts->out, argv[i + 1], len);
				}
			}
			else {
				int extlen = strlen(ext);
				opts->out = malloc(len + extlen);
				if (opts->out != NULL) {
					memcpy(opts->out, argv[i + 1], len);
					memcpy(opts->out+len, ext, extlen);
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

int main(int argc, char** argv) {

	//Parse arguments
	Settings_t options = { 0 };
	process_args(argc, argv, &options);

	//Open and create files
	FILE* f = fopen(options.in,"rb");
	if( f == NULL){
		printf("Could not open file, exiting...");
		return 1;
	}

	out = fopen(options.out, "wb");

	//Get size of file
	size_t size;
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* contents = calloc(sizeof(char),size);
	int value = fread(contents, sizeof(char), size, f);
	if(value < size){
		exit(1);
	}

	struct membuffer buf_out = {0};
	membuf_init(&buf_out, size + size/8 + 64);

	if(md_html(contents,size,process_output,&buf_out,parser_flags,renderer_flags) == -1){
		printf("Failed to parse with md4c");
	}
	char* output = calloc(sizeof(char), buf_out.size);
	memcpy(output, buf_out.data, buf_out.size);
	fwrite(buf_out.data, 1, buf_out.size, out);
	//printf("%s",contents);

	free(contents);
	fclose(f);
	return 0;
}
