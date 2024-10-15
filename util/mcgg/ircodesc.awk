function char_to_flags(c) {
	if (c == "S") return "IRF_SIZED"
		return "0"
}

function char_to_type(c) {
	if (c ~ /[A-Za-z]/) return "'"c"'"
	if (c == "?")       return "'?'"
	if (c == ".")       return "0"
}



BEGIN {
		print "#include \"ircodes.h\""
		print "const struct ir_data ir_data[IR__COUNT] = {"
}


/^ *[^# ]+/  {
		printf("\t{ \"%s\", ", $3)
		printf("%s, ", char_to_flags(substr($1, 1, 1)))
		printf("%s, ", char_to_type(substr($2, 1, 1)))
		printf("%s, ", char_to_type(substr($2, 3, 1)))
		printf("%s, ", char_to_type(substr($2, 4, 1)))
		printf(" },\n")
}
END  {
	print "};"
}
