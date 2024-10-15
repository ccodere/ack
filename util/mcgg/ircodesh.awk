BEGIN {
	print "enum ir_opcode {";
}
/^ *[^# ]+/ {
	print "\tIR_" $3 ","
}
END {
	print "\tIR__COUNT"
	print "};"
}
