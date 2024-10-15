###########################################################################
#  A utility to convert the em definition table to generated code.
#  This is more portable than the shell scripts that used to exist.
#
#  This tool should only be called if the em_table is changed, it creates
#  the following files:
#   em_spec.h
#   em_pseu.h
#   em_pseu.c
#   em_mnem.h
#   em_mnem.c
#   em_flag.c
###########################################################################
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
	printf("char em_pseu[][4] = {\n")>"em_pseu.c";
	printf("char em_mnem[][4] = {\n")>"em_mnem.c";
	printf("#include \"em_flag.h\"\n")>"em_flag.c";
	printf("char em_flag[] = {\n")>"em_flag.c";
} 
{
	if (length($0) == 0)
	{
		section++;
	} 
	
	if (length($0) != 0)
	{
		# Replace all spaces by tabs as field separator is a tab character.
		gsub(/ /,"\t",$0);
		# process the indexes.
		if (section == SECTION_INDEXES)
		{
			fieldCount = split($0, records);
			if (fieldCount != 2)
			{
				print "Error reading indexes section, expected 2 fields.\n";
				errno = 1;
				exit(errno);
			}
			if (records[1] == "fpseu")
			{
				fpseuvalue = records[2] + 0;
			}
			if (records[1] == "fmnem")
			{
				fmnemvalue = records[2] + 0;
			}			
			printf("#define sp_%s\t%s\n",records[1],records[2]) >"em_spec.h";
		} else
		# Process the mnemonics section.
		if (section == SECTION_MNEMONICS)
		{
			fieldCount = split($0, records);
			printf("#define ps_%s\t%d\n",records[1],records[2]+0+fpseuvalue)>"em_pseu.h"
			printf("  \"%s\",\n",records[1])>"em_pseu.c"
			pseudoindex++;
		} else
		if (section == SECTION_OPCODES)
		{
			fieldCount = split($0, records);
			if (fieldCount != 3)
			{
				print "Error reading opcodes section, expected 3 fields.\n";
				errno = 1;
				exit(errno);
			}			
			if (length(records[2])!=2)
			{
				printf("Error opcode type characterstic should be on 2 characters.\n");
				errno = 1;
				exit(errno);
			}
		 	printf("#define op_%s\t%d\n",records[1],opcodeindex+fmnemvalue)>"em_mnem.h";
	 		printf("  \"%s\",\n",records[1])>"em_mnem.c";	
	 		
	 		str = toupper(records[2]);
	 		str1 = substr(str,1,1);
	 		str2 = substr(str,2,1);
	 		if (str1 == "-")
	 		{
	 			str1 = "NO";
	 		}
	 		if (str2 == "-")
	 		{
	 			str2 = "NO";
	 		}
   	 		printf("PAR_%s | FLO_%s,\n",str1,str2)>"em_flag.c";	 		
	 		
	 		opcodeindex++;
		} 
	}
	
}
END { 
  # Only continue output of data if no error.
  if (errno == 0)
  {
  	printf("#define sp_lpseu\t%d\n",fpseuvalue+pseudoindex - 1) >"em_spec.h";
  	printf("#define sp_lmnem\t%d\n",fmnemvalue+opcodeindex - 1 ) >"em_spec.h";
  
  	printf("};\n")>"em_pseu.c"
  	printf("};\n")>"em_mnem.c"
  	printf("};\n")>"em_flag.c"
  }

}