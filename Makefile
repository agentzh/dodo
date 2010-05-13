.PHONY: all upload

all:
	perl dodo.pl ~/error.log

upload: all
	uptree .rsync

