TESI = Tesi.lyx
DIA_FILES = imgs/DiagrammaER.dia  imgs/Scanner.dia  imgs/SchemaLinguaggio.dia imgs/Rete.dia imgs/Architettura.dia

doc: ../Doxyfile $(DIA_FILES:.dia=.eps)
	ln -s doxy/{struct,sqlite,inotify,utils,commons,fuse__utils,musicmeshfsd,musicmeshfsc,table__view__fuse,constants}*pdf . ; true
	lyx -e pdf2 $(TESI) # pdf2 = pdf generato con pdflatex
	mv $(TESI:.lyx=.pdf) ..

$(DIA_FILES:.dia=.eps):
	dia -t eps-pango $(@:.eps=.dia) -e $(@)

clean:
	rm -f $(DIA_FILES:.dia=.eps) imgs/*~ *~ *pdf
