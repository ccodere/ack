# Creates a switch case with all opcodes
# that have no arguments
#

BEGIN  { 
	FS = "\t";
	section = 0; 
	fpseuvalue = 0;
	fmnemvalue = 0;
	SECTION_INDEXES = 0;
	SECTION_MNEMONICS = 1;
	SECTION_OPCODES = 2;
	# Error status
	errno = 0;
	opcodeindex = 0;
	pseudoindex = 0;
	printf("switch(p->em_opcode) {\n");
} 
{
    /* byte value constants */
	if (NF == 2)
	{
	/*	printf("instruction\n");*/
	} else
	if (NF == 3)
	{
      /* pseudo instructions */
	  if (match($2,/[0-9][0-9]?/)!=0)
	  {
/*		printf("pseudo\n"); */
	  } else
	  {
	    if (match($2,/w/)!=0)
	    {
	    	printf("\tcase op_%s:\n",$1);
	    	printf("\t\tC_%s_narg();\n",$1);
	    	printf("\t\tbreak;\n");
	    }
	  }
	}
}
END { 
  # Only continue output of data if no error.
  if (errno == 0)
  {
  	printf("\tdefault: EM_error = \"Illegal mnemonic\";\n");
  	printf("}\n");
  }

}