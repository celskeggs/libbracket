include config.mk

CC_BUILTIN_ROOT=$(shell $(CC) -print-search-dirs | sed -n -e "s/install: \(.*\)/\1/p" | tr -s "\n" "/")
CC_BUILTIN_INC=$(CC_BUILTIN_ROOT)include
CFLAGS=-O3 -m$(BITS) -Wall
ALL_CFLAGS=-nostdlib -nostdinc -fno-asynchronous-unwind-tables -ffreestanding -I$(BUILDDIR) -I$(CC_BUILTIN_INC) $(CFLAGS)
DESTDIR=

OBJS_RAW=math.o memory.o e_pow.o e_sqrt.o s_scalbn.o s_floor.o e_fmod.o floatscan.o strtod.o
OBJS=$(addprefix $(BUILDDIR)/, $(OBJS_RAW))

$(BUILDDIR)/libbracket.a: $(OBJS)
	ar rcs $@ $^

$(BUILDDIR)/%.o: src/%.c src/libbracket.h
# $(BUILDDIR)/math.o: src/math.c src/libbracket.h
	mkdir -p $(BUILDDIR)
	$(CC) $(ALL_CFLAGS) $< -c -o $@
# $(BUILDDIR)/memory.o: src/memory.c src/libbracket.h
#	mkdir -p $(BUILDDIR)
#	$(CC) $(ALL_CFLAGS) $< -c -o $@
# $(BUILDDIR)/e_pow.o: src/e_pow.c src/libbracket.h
#	mkdir -p $(BUILDDIR)
#	$(CC) $(ALL_CFLAGS) $< -c -o $@

.PHONY: main install test clean
	
install: $(BUILDDIR)/libbracket.a src/libbracket.h
	install -m 755 -d $(DESTDIR)/$(LIBDIR)
	install -m 755 -d $(DESTDIR)/$(INCLUDEDIR)
	install -m 644 $(BUILDDIR)/libbracket.a $(DESTDIR)/$(LIBDIR)/libbracket.a
	install -m 644 src/libbracket.h $(DESTDIR)/$(INCLUDEDIR)/libbracket.h

test:
	echo "Not implemented yet."

clean:
	rm -f $(OBJS) $(BUILDDIR)/libbracket.a

distclean: clean
	rm -f config.mk config.status
