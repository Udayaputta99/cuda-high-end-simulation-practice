import pandas as pd
import matplotlib.pyplot as plt
import io

path_serial = "serial_results.csv"
path_omp_host = "omp_results.csv"
path_cuda = "cuda_results.csv"
# 1. Load data
df_serial = pd.read_csv(path_serial)
df_omp_host = pd.read_csv(path_omp_host)
df_cuda = pd.read_csv(path_cuda)

# 2. Calculate average of runs for each nx
avg_df_serial = df_serial.groupby('nx')['serial_bw'].mean().reset_index()
avg_df_omp_host = df_omp_host.groupby('nx')['omp_bw'].mean().reset_index()
avg_df_cuda = df_cuda.groupby('nx')['cuda_bw'].mean().reset_index()

# 3. Plotting

plt.figure(figsize=(8, 5))
plt.plot(avg_df_serial['nx'], avg_df_serial['serial_bw'], color='blue', label='serial')
plt.plot(avg_df_omp_host['nx'], avg_df_omp_host['omp_bw'], color='red', label='omp-host')
plt.plot(avg_df_cuda['nx'], avg_df_cuda['cuda_bw'], color='green', label='cuda')

plt.xlabel('log scale: nx')
plt.xscale('log')
plt.ylabel('Average Bandwidth(GB/s)')
plt.title('Average Bandwidth by nx')
plt.legend()
plt.grid(axis='y', linestyle='--', alpha=0.7)

# plt.show()
plt.savefig('STREAM_Bandwidth_comparison',dpi=300)
