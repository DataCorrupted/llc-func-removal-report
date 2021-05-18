PREFIX=./build

all: clean dummy main main-pm

dummy: dummy_func.c
	clang dummy_func.c -c -o ${PREFIX}/lib/libdummy.a

main: dummy main.c
	clang -flto -fuse-ld=lld  \
		-Wl,-mllvm=-load=../pass/build/libDummyPassLTO.so \
		-Wl,--save-temps \
		main.c -o ${PREFIX}/bin/main ${PREFIX}/lib/libdummy.a

main-pm: dummy main.c
	clang -Xclang -load -Xclang ../pass/build/libDummyPass.so \
	main.c -o ${PREFIX}/bin/main-pm ${PREFIX}/lib/libdummy.a 

test: main main-pm
	@echo 
	llvm-dis ${PREFIX}/bin/main.0.5.precodegen.bc 
	@echo "dummy_func calls in bitcode before assembly generation: "
	@cat ${PREFIX}/bin/main.0.5.precodegen.ll | grep dummy_func

	@echo 
	llc ${PREFIX}/bin/main.0.5.precodegen.bc 
	@echo "dummy_func calls after assembly generation: "
	@cat ${PREFIX}/bin/main.0.5.precodegen.s | grep dummy_func 

	@echo 
	@echo "When executing main: "
	@echo "123" | ${PREFIX}/bin/main | grep dummy_func

	@echo 
	@echo "When executing main-pm: "
	@echo "123" | ${PREFIX}/bin/main | grep dummy_func

clean:
	mkdir -p ${PREFIX}/lib
	mkdir -p ${PREFIX}/bin
	rm build/lib/* build/bin/* -rf
