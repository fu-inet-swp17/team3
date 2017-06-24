bgp_rtr: bgp_val.c
	gcc -o bgp_val bgp_val.c -lpthread -lbgpstream -lrtr -Wall

.PHONY: clean demo

demo: bgp_val.c demo/term_if.c
	gcc -o bgp_val bgp_val.c -lpthread -lbgpstream -lrtr
	gcc -o term_if demo/term_if.c -lncurses -lcurl
	./bgp_val | ./term_if

clean:
	rm -f asn.txt && rm -f bgp_val && rm -f term_if
