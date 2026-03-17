import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

# 1. Load the 5-column data
# Format: iteration, n, edge_count, max_cc_size, cc_count
cols = ['iteration', 'n', 'm', 'max_cc', 'cc_count']
df = pd.read_csv('data.csv', names=cols)

# We know n = 100,000
N = 100000

# 2. Accurate Aggregation
# Group by m to get the mean size and the percentage of runs that are connected
stats = df.groupby('m').agg({
    'max_cc': 'mean',
    'cc_count': lambda x: (x == 1).mean() # Probability that the graph is connected
}).reset_index()
stats.rename(columns={'cc_count': 'p_connected'}, inplace=True)

# 3. Find Exact Milestones (No approximations)
# First m where the AVERAGE size hits exactly N
# This only happens when EVERY iteration at that m is connected.
m_full_connectivity = stats[stats['max_cc'] == N]['m'].min()

# Threshold where the Giant Component is born (Theoretical m = n/2)
m_critical = N / 2

# 4. Accurate Visualization (Log-Log)
fig, ax1 = plt.subplots(figsize=(12, 8))
sns.set_style("whitegrid")

# Plot 1: Mean Size of Largest Component
ax1.plot(stats['m'], stats['max_cc'], color='royalblue', linewidth=2.5, label='Mean Largest CC Size')
ax1.set_xscale('log')
ax1.set_yscale('log')
ax1.set_ylabel('Size of Largest Component (Log Scale)', fontsize=13)
ax1.set_xlabel('Number of Edges (m) [Log Scale]', fontsize=13)

# Plot 2: Connectivity Probability (The "S-Curve")
# This shows exactly how many of your 10k runs are connected at each stage
ax2 = ax1.twinx()
ax2.plot(stats['m'], stats['p_connected'], color='orange', alpha=0.6, linestyle='--', label='P(Full Connectivity)')
ax2.set_ylabel('Probability Graph is Fully Connected', color='orange', fontsize=13)

# 5. Reference Lines
plt.axvline(x=m_critical, color='red', linestyle='--', label='Critical Point ($m=n/2$)')

# Only plot the saturation line if it was actually reached in your data
if not np.isnan(m_full_connectivity):
    plt.axvline(x=m_full_connectivity, color='green', linestyle=':', label=f'Full Saturation ($m={int(m_full_connectivity):,}$)')

# Labels
plt.title('Accurate Phase Transition Analysis ($n=10^5$)', fontsize=16)
fig.legend(loc='upper left', bbox_to_anchor=(0.1, 0.9))

plt.tight_layout()
plt.savefig('accurate_analysis.png', dpi=300)
plt.show()

# Precise Reporting
print(f"--- Accurate Results ---")
print(f"Number of nodes (N): {N}")
print(f"Theoretical Transition (Giant born): m = {int(m_critical):,}")
if not np.isnan(m_full_connectivity):
    print(f"Full Saturation (100% of runs connected): m = {int(m_full_connectivity):,}")
else:
    print("Full Saturation: Not reached in recorded data.")