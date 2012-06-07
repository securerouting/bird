source=bgpsec.c validate.c
root-rel=../../../
dir-name=proto/bgp/bgpsec

include ../../../Rules

tests: $(objs) tests.o
	gcc -o $@ $(objs) tests.o $(LDFLAGS) $(LIBS)

genrtrkey: $(objs) genrtrkey.o
	gcc -o $@ $(objs) genrtrkey.o $(LDFLAGS) $(LIBS)
