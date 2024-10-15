# From the warning definitions, generate a header file
# warn.h with all the values of the different trap values.
#
# The function declarations should not be here, should
# be cleaned up eventually.
#

function basename(file) 
{
    sub(".*/", "", file)
    return file
}

BEGIN {
	print("/* This file is auto generated from '" basename(ARGV[1]) "', do not edit */")>"warn.h";	
}
/^\.Wn/ { 
    gsub(/^\.Wn ".*"/,"#define",$0);
	print $0>"warn.h";	 }
END {    
  print "">"warn.h";
  print("#define	warning(n)	do_warn((n), __LINE__, __FILE__)")>"warn.h"; 
  print("void do_warn(int, int, const char *);")>"warn.h";
  print("void set_wmask(int);")>"warn.h";
  print("void init_wmsg(void);")>"warn.h";
  print("#ifdef LOGGING")>"warn.h";
  print("void warningcont(int);")>"warn.h";
  print("#endif /* LOGGING */")>"warn.h";  
}