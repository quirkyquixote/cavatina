
include ../config.mk

OBJS += parse.o
OBJS += list.o

PROGRAMS = $(OBJS:.o:)
DEPS = $(OBJS:.o=.d)
LOGS = $(OBJS:.o=.log)

.PHONY: all
all: $(LOGS)
	@for i in $(LOGS); do echo TEST $$i; diff -q $$i $$i.gold; done

.PHONY: clean
clean:
	$(RM) $(PROGRAMS)
	$(RM) $(OBJS)
	$(RM) $(DEPS)
	$(RM) $(LOGS)

parse: parse.o ../cavatina.o
list: list.o ../cavatina.o

%.log: %
	$(QUIET_GEN)./$< > $@ 2>&1

