###########################################################################
#  A utility to convert the keywords to a c file
#  This is more portable than the shell scripts that used to exist.
#
###########################################################################
BEGIN  { 
	FS = "\t";
	
	printf("#include \"param.h\"\n")
    printf("#include \"lookup.h\"\n")
    printf("#include \"varinfo.h\"\n")
    printf("#include \"instruct.h\"\n")
    printf("#include \"set.h\"\n")
    printf("#include \"expr.h\"\n")
    printf("#include \"iocc.h\"\n")
    printf("#include \"y.tab.h\"\n")
    printf("\n")
    printf("void enterkeyw(void) {\n")
    printf("  register symbol *sy_p;\n")
    printf("\n")
    
} 
{
	if (NF==2)
	{
	   printf("  sy_p=lookup(\"%s\",symkeyw,newsymbol);sy_p->sy_value.syv_keywno=%s;\n",$1,$2)
	}
}
END { 
  # Only continue output of data if no error.
  printf("}\n")
}
