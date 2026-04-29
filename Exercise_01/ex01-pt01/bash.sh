cd src/stream

make

cd ../../

rm -rf *.csv
# rm -rf *.png

sizes=()
value=2
warmup=2

while [ $value -le 100000000 ]; do #less than 10M
    sizes+=($value)
    value=$((value * 2))
    # echo "value is $value"
done

echo "nx,run,serial_bw" > serial_results.csv
for nx in "${sizes[@]}"; do
    for run in {1..10}; do
        serial_bw=$(./build/stream/stream-base $nx $warmup 10 | grep "bandwidth" | awk '{print $2}')
        echo "$nx,$run,$serial_bw" >> serial_results.csv
    done
done
echo "nx,run,omp_bw" > omp_results.csv
for nx in "${sizes[@]}"; do
    for run in {1..10}; do
        omp_bw=$(./build/stream/stream-omp-host $nx $warmup 10 | grep "bandwidth" | awk '{print $2}')
        echo "$nx,$run,$omp_bw" >> omp_results.csv
    done
done

echo "nx,run,cuda_bw" > cuda_results.csv

for nx in "${sizes[@]}"; do
    for run in {1..10}; do
        cuda_bw=$(./build/stream/stream-cuda $nx $warmup 10 | grep "bandwidth" | awk '{print $2}')
        echo "$nx,$run,$cuda_bw" >> cuda_results.csv
    done
done

python plots.py