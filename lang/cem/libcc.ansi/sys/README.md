The functions here all use POSIX system calls to do the actual work or are
compliant with the POSIX definitions, and so usually require `unistd.h` 
(at the minimum). Typically each group of functions will be protected by an 
`ACKCONF` variable so the plat can turn them on and off as
necessary.

Usually some of these functions may wish to be overriden by the different
platforms. 

Some extra information here on how to port:

- errno codes in errno.h should be modified accordingly as well as the values in errlist.c
  to be mapped correctly.

