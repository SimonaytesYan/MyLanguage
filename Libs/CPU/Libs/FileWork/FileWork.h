#ifndef _FILEWORK_SYM_
#define _FILEWORK_SYM_

int    gettext(char *text, size_t size,  FILE *fp);

void   getlines_from_text(const char **lines, char *text);

void   put_text_to_file(const char *text, FILE *fp, size_t N);

size_t get_text_size(const char *file_name);

#endif