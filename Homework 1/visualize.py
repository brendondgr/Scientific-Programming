
import pandas as pd
import matplotlib.pyplot as plt
import os
import argparse
import json
import glob
import random
import numpy as np

def format_column_name(column_name):
    """
    Formats a column name for display in plots.
    Replaces underscores with spaces and capitalizes each word.

    Args:
        column_name (str): The original column name.

    Returns:
        str: The formatted column name.
    """
    return column_name.replace('_', ' ').title()

def create_comparison_plot(data_dir, images_dir, ascending=False, descending=False):
    """
    Loads transformed CSV files, selects a random column from each, optionally sorts them,
    and plots them on a single line graph.

    Args:
        data_dir (str): The directory where the data files are located.
        images_dir (str): The directory where the generated plot will be saved.
        ascending (bool): If True, sort data in ascending order.
        descending (bool): If True, sort data in descending order.
    """
    transformed_files = glob.glob(os.path.join(data_dir, '*_transformed.csv'))
    if len(transformed_files) < 2:
        print("Error: Less than two transformed CSV files found. Cannot create comparison plot.")
        return

    plt.figure(figsize=(12, 8))
    
    plot_labels = []

    for file_path in transformed_files:
        try:
            df = pd.read_csv(file_path)
            if df.empty or len(df.columns) == 0:
                print(f"Warning: {os.path.basename(file_path)} is empty or has no columns. Skipping.")
                continue

            random_column = random.choice(df.columns)
            
            column_data = df[random_column].dropna()

            if ascending:
                # Sort the data from least to greatest
                processed_data = np.sort(column_data)
            elif descending:
                # Sort the data from greatest to least
                processed_data = np.sort(column_data)[::-1]
            else:
                # Use original order
                processed_data = column_data.values
            
            # Plot the sorted data
            plt.plot(processed_data, label=format_column_name(random_column))
            plot_labels.append(format_column_name(random_column))

        except Exception as e:
            print(f"Error processing {file_path}: {e}")
            continue

    if not plot_labels:
        print("No data was plotted. Skipping plot generation.")
        plt.close()
        return

    plt.title(f'Comparison of Random Columns: {plot_labels[0]} vs {plot_labels[1]}')
    
    xlabel = 'Index'
    if ascending:
        xlabel = 'Sorted Index (Ascending)'
        plt.title(f'Ascending Comparison of Random Columns: {plot_labels[0]} vs {plot_labels[1]}')
    elif descending:
        xlabel = 'Sorted Index (Descending)'
        plt.title(f'Descending Comparison of Random Columns: {plot_labels[0]} vs {plot_labels[1]}')
        
    plt.xlabel(xlabel)
    plt.ylabel('Normalized Value')
    plt.legend()
    plt.grid(True, which='both', axis='y', linestyle='--', linewidth=0.5)
    plt.yticks(np.arange(0, 1.1, 0.1))

    if not os.path.exists(images_dir):
        os.makedirs(images_dir)
        
    output_path = os.path.join(images_dir, 'random_column_comparison.png')
    plt.savefig(output_path, bbox_inches='tight')
    plt.close()
    print(f"Saved comparison plot to {output_path}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot a comparison of random columns from transformed CSV files.")
    parser.add_argument('--data-dir', type=str, default='data',
                        help='Directory where the data files are located.')
    parser.add_argument('--images-dir', type=str, default='images',
                        help='Directory where the generated plots will be saved.')
    
    sort_group = parser.add_mutually_exclusive_group()
    sort_group.add_argument('--ascending', action='store_true',
                        help='Sort the data in ascending order.')
    sort_group.add_argument('--descending', action='store_true',
                        help='Sort the data in descending order.')

    args = parser.parse_args()

    create_comparison_plot(args.data_dir, args.images_dir, args.ascending, args.descending)
