PREFIX=./build

all: dummy main

dummy: dummy_func.c
	clang dummy_func.c -c -o ${PREFIX}/lib/libdummy.a

main: dummy main.c
	clang -flto -fuse-ld=lld  \
		-Wl,-mllvm=-load=../pass/build/libDummyPassLTO.so \
		-Wl,--save-temps \
		main.c -o ${PREFIX}/bin/main ${PREFIX}/lib/libdummy.a

test: all
	llvm-dis ${PREFIX}/bin/main.0.5.precodegen.bc 
	@echo "dummy_func calls in bitcode before assembly generation: "
	@cat ${PREFIX}/bin/main.0.5.precodegen.ll | grep dummy_func

	llc ${PREFIX}/bin/main.0.5.precodegen.bc 
	@echo "dummy_func calls after assembly generation: "
	@cat ${PREFIX}/bin/main.0.5.precodegen.s | grep dummy_func | true

	@echo "dummy_func being called when executing: "
	@${PREFIX}/bin/main | grep dummy_func | true

clean:
	rm build/lib/* build/bin/* -rf