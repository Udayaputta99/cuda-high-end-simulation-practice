rm -r -f out_base/
mkdir -p out_base/

make base
rm -f particle_base.tar.gz
tar -cvf particle_base.tar.gz *.vtu *.pvd
rm -r *.vtu *.pvd 
rm -r -f out_base/