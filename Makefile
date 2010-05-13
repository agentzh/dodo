.PHONY: all upload

all:
	perl dodo.pl ~/error.log

upload: all
	rsync -e 'ssh -p 18888' /home/chunlai/lightzone--exclude=.git --exclude=t/servroot -rcv --exclude='*.tar.gz' --exclude='*.dom.res' --exclude='*.swp' -l --exclude='*~' --exclude=blib `pwd`/ agentzh@agentzh.org:~/misc/dodo/

