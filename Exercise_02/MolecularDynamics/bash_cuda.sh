rm -r -f out_cuda/
mkdir -p out_cuda/

make cuda
rm -f particle_cuda.tar.gz
tar -cvf particle_cuda.tar.gz *.vtu *.pvd
rm -r *.vtu *.pvd 
rm -r -f out_cuda/