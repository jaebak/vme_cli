# A very simple Makefile
vme_cli : build/vme_cli.o build/VMEController.o
	g++ -o $@ $^

build/%.o: %.cc VMEController.h JTAG_constants.h VMEUtils.h
	g++ -c -o $@ $< -I.

.PHONY: clean

clean:
	rm -f build/*.o vme_cli
