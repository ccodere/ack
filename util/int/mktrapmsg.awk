# From the trap messages, generates a code file
# trap_msg with the textual description of each
# trap message.
#

function basename(file) 
{
    sub(".*/", "", file)
    return file
}

BEGIN {
	print("/* This file is auto generated from '" basename(ARGV[1]) "', do not edit */")>"trap_msg";	
}
{
	gsub(/^../,"",$0);
	print("\"" $0 "\",")>"trap_msg";
}
END {    
  print "">"trap_msg";
}