typedef char* va_list;

#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (size_t) - 1) / sizeof (size_t)) * sizeof (size_t))

#define va_start(AP, LASTARG)  \
  ( AP = ((char *) &(LASTARG) + __va_rounded_size (LASTARG)))

#define va_end(AP)

#define va_arg(AP, TYPE)  \
 (AP += __va_rounded_size (TYPE),    \
  *((TYPE *) (AP - ((sizeof(TYPE) < 4)  \
		    ? sizeof(TYPE)  \
		    : __va_rounded_size (TYPE)))))
