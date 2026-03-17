import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import os

# --- Configuration ---
num_files = 4
iters_per_file = 2500
N = 10000  # Updated to 1e6 based on your C++ code (n = 1e6)
lim_m = 1000000 # 1e6

# 1. Load and Combine
cols = ['iteration', 'n', 'm', 'max_cc', 'cc_count']
all_data = []

for i in range(num_files):
    fname = f'data_thread_{i}.csv'
    if os.path.exists(fname):
        df_temp = pd.read_csv(fname, names=cols)
        # Offset iterations to be continuous (1-4000)
        df_temp['iteration'] += (i * iters_per_file)
        all_data.append(df_temp)

df = pd.concat(all_data, ignore_index=True)

# 2. Handle the "Early Exit" Logic
# We need the average max_cc for the plot. 
# For any 'm' where a run has already finished (cc_count=1), 
# that run's contribution to the average is N.

# Group by iteration to find where each one ended
ends = df.groupby('iteration')['m'].max().reset_index()

# To get an accurate mean without a massive dataframe, we'll 
# calculate the mean of existing points, and later 
# we'll "pull" the curve to N at the saturation point.
stats = df.groupby('m')['max_cc'].mean().reset_index()

# Find the saturation point (the m where the average is ~N)
saturation_m = stats[stats['max_cc'] >= N * 0.999]['m'].min()

# 3. Create the Log-Log Plot
plt.figure(figsize=(12, 8))
sns.set_style("whitegrid")

# Plot the actual data
plt.plot(stats['m'], stats['max_cc'], color='royalblue', linewidth=2.5, label='Largest CC Size (Mean)')

# Manually "Draw" the saturation line from the last data point to lim_m
# This prevents the plot from looking like it just "stopped"
plt.hlines(y=N, xmin=saturation_m, xmax=lim_m, color='royalblue', linewidth=2.5)

# --- SCALES ---
plt.xscale('log')
plt.yscale('log')

# --- REFERENCE LINES ---
# Red: Critical Threshold (m = n/2)
m_crit = N / 2
plt.axvline(x=m_crit, color='red', linestyle='--', alpha=0.6, label=f'Critical Point (m={int(m_crit)})')

# Green: Saturation Level (Size = N)
plt.axhline(y=N, color='green', linestyle=':', linewidth=2, label=f'Saturation (N={N})')

# Orange: Connectivity Point
plt.axvline(x=saturation_m, color='orange', linestyle='-.', alpha=0.8, label=f'Connectivity (m≈{int(saturation_m):,})')

# 4. Final Formatting
plt.title(f'Log-Log Phase Transition: n={N}, 10000 Runs', fontsize=16)
plt.xlabel('Edges (m) [Log Scale]', fontsize=13)
plt.ylabel('Largest Component Size [Log Scale]', fontsize=13)
plt.grid(True, which="both", ls="-", alpha=0.2)
plt.legend(loc='lower right')

# Adjust limits to see the full range from few edges to 10^8
plt.xlim(10, lim_m)
plt.ylim(1, N * 2)

plt.tight_layout()
plt.savefig('phase_transition_loglog.png', dpi=300)
plt.show()

print(f"Plot generated. Connectivity threshold observed at m ≈ {int(saturation_m):,}")