SRC_DOCS = \
    LLgen_NCER.n \
	LLgen.n

HTML_DOCS := $(SRC_DOCS:.n=.html)

PS_DOCS	= $(SRC_DOCS:.n=.ps)

PDF_DOCS = $(PS_DOCS:.ps=.pdf)
	
docs: $(PS_DOCS) $(PDF_DOCS)

.SUFFIXES: .n .ps .html .pdf
.n.ps:
	groff -m ms -R -G -e -g -p -t  -Tps $< >$@
.n.html:
	groff -m ms -R -G -e -g -p -t -Txhtml $< >$@
.ps.pdf:
	ps2pdf $< $@

clean:
	-rm ${HTML_DOCS}
	-rm ${PS_DOCS}
	-rm ${PDF_DOCS}
