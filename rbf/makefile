include ../makefile.inc


all: pre-build librbf.a rbftest1 rbftest2 rbftest3 rbftest4 rbftest5 rbftest6 rbftest7 rbftest8 rbftest8b rbftest9 rbftest10 rbftest11 rbftest12 tests/pfm-test rbftest_p0 rbftest_p1 rbftest_p1b rbftest_p1c rbftest_p2 rbftest_p2b rbftest_p3 rbftest_p4

# Compile in debug mode for debug output
debug: CPPFLAGS += -DDEBUG
debug: all

# My tests
test: pre-build tests/page-test tests/utils-test

tests/page-test: librbf.a tests/page-test.o
tests/page-test.o: pfm.h rbfm.h

tests/utils-test: librbf.a tests/utils-test.o
tests/utils-test.o: pfm.h rbfm.h utils.h

# Force re-compile because of hack
pre-build:
	rm -f pfm.o rbfm.o



# c file dependencies
pfm.o: pfm.h
rbfm.o: rbfm.h

# lib file dependencies
librbf.a: librbf.a(pfm.o)  # and possibly other .o files
librbf.a: librbf.a(rbfm.o)

# Tests
rbftest1.o: pfm.h rbfm.h
rbftest2.o: pfm.h rbfm.h
rbftest3.o: pfm.h rbfm.h
rbftest4.o: pfm.h rbfm.h
rbftest5.o: pfm.h rbfm.h
rbftest6.o: pfm.h rbfm.h
rbftest7.o: pfm.h rbfm.h
rbftest8.o: pfm.h rbfm.h
rbftest8b.o: pfm.h rbfm.h
rbftest9.o: pfm.h rbfm.h
rbftest10.o: pfm.h rbfm.h
rbftest11.o: pfm.h rbfm.h
rbftest12.o: pfm.h rbfm.h
rbftest_p0.o: pfm.h rbfm.h
rbftest_p1.o: pfm.h rbfm.h
rbftest_p1b.o: pfm.h rbfm.h
rbftest_p1c.o: pfm.h rbfm.h
rbftest_p2.o: pfm.h rbfm.h
rbftest_p2b.o: pfm.h rbfm.h
rbftest_p3.o: pfm.h rbfm.h
rbftest_p4.o: pfm.h rbfm.h
tests/pfm-test.o: pfm.h

# binary dependencies
rbftest1: rbftest1.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest2: rbftest2.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest3: rbftest3.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest4: rbftest4.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest5: rbftest5.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest6: rbftest6.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest7: rbftest7.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest8: rbftest8.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest8b: rbftest8b.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest9: rbftest9.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest10: rbftest10.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest11: rbftest11.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest12: rbftest12.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest_p0: rbftest_p0.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest_p1: rbftest_p1.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest_p1b: rbftest_p1b.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest_p1c: rbftest_p1c.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest_p2: rbftest_p2.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest_p2b: rbftest_p2b.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest_p3: rbftest_p3.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest_p4: rbftest_p4.o librbf.a $(CODEROOT)/rbf/librbf.a
tests/pfm-test: tests/pfm-test.o librbf.a $(CODEROOT)/rbf/librbf.a

# dependencies to compile used libraries
.PHONY: $(CODEROOT)/rbf/librbf.a
$(CODEROOT)/rbf/librbf.a:
	$(MAKE) -C $(CODEROOT)/rbf librbf.a

.PHONY: clean
clean:
	-rm -rf rbftest1 rbftest2 rbftest3 rbftest4 rbftest5 rbftest6 rbftest7 rbftest8 rbftest8b rbftest9 rbftest10 rbftest11 rbftest12 pfm-test page-test utils-test *.a *.o *~ rbftest_p0 rbftest_p1 rbftest_p1b rbftest_p1c rbftest_p2 rbftest_p2b rbftest_p3 rbftest_p4
