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
	    if (match($2,/[cdflnorswz]/)==1)
	    {
	    	printf("\tcase op_%s:\n",$1);
	    	printf("\t\tC_%s(p->em_cst);\n",$1);
	    	printf("\t\tbreak;\n");
	    } else
	    if (match($2,"-")==1)
	    {
#	        opcodes_narg[op_narg_index++] = $1;
	    	printf("\tcase op_%s:\n",$1);
	    	printf("\t\tC_%s();\n",$1);
	    	printf("\t\tbreak;\n");
	    } else
	    if (match($2,"p")==1)
	    {
	    	printf("\tcase op_%s:\n",$1);
	    	printf("\t\tC_%s(p->em_pnam);\n",$1);
	    	printf("\t\tbreak;\n");
	    } else
	    if (match($2,"b")==1)
	    {
	    	printf("\tcase op_%s:\n",$1);
	    	printf("\t\tC_%s((label) (p->em_cst));\n",$1);
	    	printf("\t\tbreak;\n");
	    } else
	    if (match($2,"g")==1)
	    {
	    	opcodes[opcodeindex] = $1;
	    	opcodeindex++;
	    }
	  }
	}
}
END { 
  # The "g" values are added here
  
  printf("/* a \"g\" argument */\n");
  printf("\tdefault: \n");
  printf("\t\tif (p->em_argtype == nof_ptyp) {\n");
  printf("\t\t\tswitch (p->em_opcode) {\n");
  printf("\t\t\t\tdefault:\n");
  printf("\t\t\t\t\tEM_error = \"Illegal mnemonic\";\n");
  printf("\t\t\t\t\tbreak;\n");
  
  for (var in opcodes)
  {
	    	printf("\t\t\t\tcase op_%s:\n",opcodes[var]);
	    	printf("\t\t\t\t\tC_%s_dlb(p->em_dlb, p->em_off);\n",opcodes[var]);
	    	printf("\t\t\t\t\tbreak;\n");
  }
  printf("\t\t\t}\n");
  printf("\t\t}\n");
  
  printf("\t\telse if (p->em_argtype == sof_ptyp) {\n");
  printf("\t\t\tswitch (p->em_opcode) {\n");
  printf("\t\t\t\tdefault:\n");
  printf("\t\t\t\t\tEM_error = \"Illegal mnemonic\";\n");
  printf("\t\t\t\t\tbreak;\n");
  
  for (var in opcodes)
  {
	    	printf("\t\t\t\tcase op_%s:\n",opcodes[var]);
	    	printf("\t\t\t\t\tC_%s_dnam(p->em_dnam, p->em_off);\n",opcodes[var]);
	    	printf("\t\t\t\t\tbreak;\n");
  }
  printf("\t\t\t}\n");
  printf("\t\t}\n");
  
  printf("\t\telse /*argtype == cst_ptyp */ {\n");
  printf("\t\t\tswitch (p->em_opcode) {\n");
  printf("\t\t\t\tdefault:\n");
  printf("\t\t\t\t\tEM_error = \"Illegal mnemonic\";\n");
  printf("\t\t\t\t\tbreak;\n");
  
  for (var in opcodes)
  {
	    	printf("\t\t\t\tcase op_%s:\n",opcodes[var]);
	    	printf("\t\t\t\t\tC_%s(p->em_cst);\n",opcodes[var]);
	    	printf("\t\t\t\t\tbreak;\n");
  }
  printf("\t\t\t}\n");
  printf("\t\t}\n");
  printf("\n}\n");
  

}