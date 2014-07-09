all:
	scons

clean:
	scons --clean

coverage:
	@echo "You need to be building with the coverage flags enabled in SConstruct for this to work"
	find src tests -name "*.gcda" -or -name "*.gcno" -delete
	make clean
	make
	rm *.gcov || true
	./run_tests || true
	bash -c 'for x in $$( find src tests -name *.c | grep -v clar.c ) ; do gcov -p $$x ; done'

