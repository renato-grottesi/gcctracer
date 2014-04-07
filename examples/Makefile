# Make all examples

DIRS = 01-basic 02-simple-thread

.PHONY: examples clean

examples:
	for d in $(DIRS); do (cd $$d; $(MAKE) ); done

clean:
	for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done
	
