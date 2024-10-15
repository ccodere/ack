# From the warning definitions, generates a code file
# warn_msg with the structure for each warning message
# and code.
#

function basename(file) 
{
    sub(".*/", "", file)
    return file
}

BEGIN {
	print("/* This file is auto generated from '" basename(ARGV[1]) "', do not edit */")>"warn_msg";	
}
/^\.Wn/ { 
	warncode = match($0,/[0-9]+$/)
	warncodestr = substr($0, RSTART, RLENGTH);
	gsub(/^\.Wn[\t ]+/,"",$0);
	gsub(/[\t ]+[A-Z]+[\t ]+[0-9]+$/,"",$0)
	gsub(/\\-/,"-",$0);
	print("{" $0 ", " warncodestr "},")>"warn_msg";
	}
END {    
  print "">"warn_msg";
}