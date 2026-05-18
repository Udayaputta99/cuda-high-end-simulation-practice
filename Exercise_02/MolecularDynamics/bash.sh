rm -rf vtufiles/
cd src
rm *.vtu *.pvd 
rm main
g++ --std=c++20 -o particleSim particleSim.cpp

./particleSim > out.txt

tar -cvf particle.tar.gz *.vtu *.pvd
mkdir ../vtufiles
mv *.vtu *.pvd ../vtufiles/
mv *.tar.gz ../