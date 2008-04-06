.PHONY : bin clean doc doxy

bin: src
	$(MAKE) -C src

doc: doxy
	$(MAKE) -C doc

clean:
	rm -rf bin/* doxy/* Tesi.pdf
	$(MAKE) clean -C src

doxy: src
	doxygen Doxyfile
	$(MAKE) -C doxy/latex
