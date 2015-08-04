nasm -f elf64 triad_sse_asm.asm
nasm -f elf64 triad_avx_asm.asm
nasm -f elf64 triad_fma_asm.asm
g++ -m64 -lrt -O3 -mfma  tests.cpp triad_fma_asm.o -o tests_fma
g++ -m64 -lrt -O3 -mavx  tests.cpp triad_avx_asm.o -o tests_avx
g++ -m64 -lrt -O3 -msse2 tests.cpp triad_sse_asm.o -o tests_sse
