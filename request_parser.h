#ifndef ebb_request_parser_h
#define ebb_request_parser_h

#include <sys/types.h> 
#ifndef TRUE
# define TRUE 1
# define FALSE 0
#endif

typedef struct ebb_request_info ebb_request_info;
typedef struct ebb_element ebb_element;
typedef struct ebb_request_parser  ebb_request_parser;
typedef void (*element_cb)(void *data, ebb_element *);

#define EBB_IDENTITY 0
#define EBB_CHUNKED  1

struct ebb_request_info {
  size_t content_length;
  int transfer_encoding;
  size_t body_read;
  int eating_body;
  int expect_continue;

  unsigned int version_major;
  unsigned int version_minor;

  void (*free) (ebb_request_info*);
};

struct ebb_element {
  const char *base;
  size_t len; 
  ebb_element *next;  

  void (*free) (ebb_element*);
}; 

struct ebb_request_parser {

/* PUBLIC */
  void *data;

  /* allocates a new element */
  ebb_element* (*new_element)(void *);

  ebb_request_info* (*new_request_info)(void*);
  void (*request_complete)(void*);

  void (*body_handler)(void *data, const char *at, size_t length);
  void (*header_handler)(void *data, ebb_element *field, ebb_element *value);
  element_cb request_method;
  element_cb request_uri;
  element_cb fragment;
  element_cb request_path;
  element_cb query_string;

/* PRIVATE */
  int cs;
#define PARSER_STACK_SIZE 10
  int stack[PARSER_STACK_SIZE];
  int top;

  size_t chunk_size;
  unsigned eating:1;

  /* element in progress stack 
   * grammar doesn't have more than 3 nested elements
   */
  ebb_element *eip_stack[3]; 
  ebb_element *header_field_element;
  ebb_request_info *current_request;
};

void ebb_request_parser_init
  ( ebb_request_parser *parser
  );

size_t ebb_request_parser_execute
  ( ebb_request_parser *parser
  , const char *data
  , size_t len
  );

int ebb_request_parser_has_error
  ( ebb_request_parser *parser
  );

int ebb_request_parser_is_finished
  ( ebb_request_parser *parser
  );

void ebb_request_info_init
  ( ebb_request_info *
  );

void ebb_element_init
  ( ebb_element *element
  );

ebb_element* ebb_element_last
  ( ebb_element *element
  );

size_t ebb_element_len
  ( ebb_element *element
  );

void ebb_element_strcpy
  ( ebb_element *element
  , char *dest
  );

void ebb_element_printf
  ( ebb_element *element
  , const char *format
  );

#endif
