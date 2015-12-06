NVCC      = nvcc
CUBLASLIB = -lcublas


all: test_dgemm_cublas.x test_sgemm_cublas.x test_dsyrk_cublas.x test_dtrsm_cublas.x test_daxpy_cublas.x test_saxpy_cublas.x test_sgemm_magma.x test_sgemm_magma2.x

#test_dgemm_cublas.x: test_dgemm_cublas.cu 
#	nvcc test_dgemm_cublas.cu -o test_dgemm_cublas.x -lcublas
#
#test_sgemm_cublas.x: test_sgemm_cublas.cu
#	nvcc test_sgemm_cublas.cu -o test_sgemm_cublas.x -lcublas
#
#test_dsyrk_cublas.x: test_dsyrk_cublas.cu 
#	nvcc test_dsyrk_cublas.cu -o test_dsyrk_cublas.x -lcublas
#
#test_dtrsm_cublas.x: test_dtrsm_cublas.cu 
#	nvcc test_dtrsm_cublas.cu -o test_dtrsm_cublas.x -lcublas
#
#test_daxpy_cublas.x: test_daxpy_cublas.cu 
#	nvcc test_daxpy_cublas.cu -o test_daxpy_cublas.x -lcublas

%.x: %.cu
	$(NVCC) $< -o $@ $(CUBLASLIB)

clean:
	rm -f *.o *~ *.x
	#rm gpu/*.o gpu/*~ gpu/*.x
	#rm mkl/*.o mkl/*~ mkl/*.x
