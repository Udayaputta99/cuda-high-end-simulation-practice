# rm -rf vtufiles/
# cd src_cuda
# rm *.vtu *.pvd 
# rm main
# nvcc -O3 --std=c++20 -o main main.cu

# ./main

# tar -cvf particle.tar.gz *.vtu *.pvd
# mkdir ../vtufiles
# mv *.vtu *.pvd ../vtufiles/
# mv *.tar.gz ../