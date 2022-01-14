BUILDDIR=build
CC=gcc -no-pie
CFLAGS=--std=c17
SRCDIR=src
ASM=nasm
ASMFLAGS=-felf64 -g

all: $(BUILDDIR)/sepia_sse.o $(BUILDDIR)/image_manage.o $(BUILDDIR)/bmpfile_handler.o $(BUILDDIR)/image_sepia.o $(BUILDDIR)/main.o
	$(CC) -o $(BUILDDIR)/main $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.asm build
	$(ASM) $(ASMFLAGS) -o $@ $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.c build
	$(CC) -c $(CFLAGS) $< -o $@

build:
	mkdir -p $(BUILDDIR)

.PHONY: clean

clean:
	rm -rf $(BUILDDIR)
