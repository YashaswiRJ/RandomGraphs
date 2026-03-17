import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

# 1. Load the 2-column data
# Format: iteration_number, edges_to_connect
try:
    # We specify names since the CSV might not have a header row
    cols = ['iteration', 'm_to_connect']
    df = pd.read_csv('distribution_1e4.csv', names=cols)
except FileNotFoundError:
    print("Error: 'j.csv' not found. Please ensure the file is in the same folder.")
    exit()

# 2. Basic Data Cleaning
# Ensure there are no null values that would crash the plot
df = df.dropna()

if df.empty:
    print("Error: The data file is empty.")
else:
    # 3. Calculate Statistics
    mean_m = df['m_to_connect'].mean()
    median_m = df['m_to_connect'].median()
    std_m = df['m_to_connect'].std()

    # 4. Create the Distribution Plot
    plt.figure(figsize=(12, 7))
    sns.set_style("white")

    # Draw the Histogram + Kernel Density Estimate (KDE)
    # Using 'stat="probability"' or "density" makes it a proper distribution curve
    sns.histplot(df['m_to_connect'], kde=True, color='royalblue', bins=40, stat="density", alpha=0.6)

    # 5. Add Reference Lines for Mean and Median
    plt.axvline(mean_m, color='red', linestyle='--', linewidth=2, 
                label=f'Mean m: {int(mean_m):,}')
    plt.axvline(median_m, color='green', linestyle='-', linewidth=2, 
                label=f'Median m: {int(median_m):,}')

    # 6. Formatting the Plot (Normal Linear Scale)
    plt.title('Distribution of Edges Required for Full Connectivity ($n=10^4$)', fontsize=16)
    plt.xlabel('Number of Edges Added to reach $cc\_count=1$', fontsize=13)
    plt.ylabel('Probability Density', fontsize=13)
    
    # Add a text box with summary stats
    stats_text = f"Total Runs: {len(df)}\nStd Dev: {std_m:.2f}"
    plt.text(0.95, 0.95, stats_text, transform=plt.gca().transAxes, 
             verticalalignment='top', horizontalalignment='right', 
             bbox=dict(boxstyle='round', facecolor='white', alpha=0.5))

    plt.legend()
    sns.despine() # Makes the graph look cleaner
    plt.tight_layout()
    
    # Save and show
    plt.savefig('connectivity_distribution.png', dpi=300)
    plt.show()

    print(f"Plot generated successfully using {len(df)} data points.")