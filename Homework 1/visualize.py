
import pandas as pd
import matplotlib.pyplot as plt
import os
import argparse
import json

def get_files_to_process(data_dir, params_data):
    """
    Identifies which CSV files should be processed for visualization based on a parameters file.

    This function iterates through the parameters data, determines the correct file path
    (original or transformed), and filters columns based on 'do_not_include' lists.
    It prepares a list of files that are ready for plotting.

    Args:
        data_dir (str): The directory where the data files are located.
        params_data (dict): A dictionary loaded from a JSON parameters file, containing
                            details about each file to process.

    Returns:
        list: A list of dictionaries, where each dictionary contains the file 'path'
              and a list of 'columns' to be plotted for that file.
    """
    files_to_plot_info = []
    for file_key, params in params_data.items():
        base_filename = params['file_name']

        if base_filename.endswith('_summary.csv'):
            continue

        if 'columns' not in params:
            print(f"Warning: 'columns' not specified for {base_filename} in parameters file, skipping.")
            continue
        
        original_columns = params.get('columns', [])
        do_not_include = params.get('other_parameters', {}).get('do_not_include', [])

        columns_to_plot = [
            col for col in original_columns 
            if not any(term in col for term in do_not_include if term)
        ]

        if not columns_to_plot:
            print(f"Warning: No columns to plot for {base_filename} after filtering.")
            continue

        normalize = params.get('other_parameters', {}).get('normalize', False)

        if normalize:
            file_name = f"{os.path.splitext(base_filename)[0]}_transformed.csv"
        else:
            file_name = base_filename
        
        file_path = os.path.join(data_dir, file_name)
        if os.path.exists(file_path):
            files_to_plot_info.append({'path': file_path, 'columns': columns_to_plot})
        else:
            print(f"Warning: File not found, skipping: {file_path}")
            
    return files_to_plot_info

class DataPlotter:
    """
    A class to handle reading CSV data and generating plots.

    This class provides methods to read data from CSV files and create two types
    of visualizations: a line plot showing all columns over their index, and
    individual histograms for each column. It handles file I/O for saving the
    plots as images.

    Attributes:
        data_dir (str): The directory where data files are stored.
        images_dir (str): The directory where generated plots will be saved.
        row_sample_interval (int): The interval to sample rows for plotting,
                                   e.g., a value of 10 means every 10th row is plotted.
    """
    def __init__(self, data_dir, images_dir, row_sample_interval=1):
        """
        Initializes the DataPlotter with directories and a sampling interval.

        Args:
            data_dir (str): The path to the directory containing data files.
            images_dir (str): The path to the directory where plots will be saved.
            row_sample_interval (int, optional): The interval for sampling data rows.
                                                 Defaults to 1 (no sampling).
        """
        self.data_dir = data_dir
        self.images_dir = images_dir
        self.row_sample_interval = row_sample_interval
        if not os.path.exists(self.images_dir):
            os.makedirs(self.images_dir)

    def read_data(self, file_path):
        """
        Reads a CSV file into a pandas DataFrame.

        Args:
            file_path (str): The full path to the CSV file to be read.

        Returns:
            pandas.DataFrame or None: A DataFrame containing the CSV data, or None if
                                      an error occurs during reading.
        """
        try:
            df = pd.read_csv(file_path)
            return df
        except Exception as e:
            print(f"Error reading {file_path}: {e}")
            return None

    def plot_and_save(self, df, file_path):
        """
        Generates and saves a line plot of all columns in a DataFrame.

        The plot shows each column as a separate line, plotted against the DataFrame index.
        The resulting image is saved to the directory specified during initialization.

        Args:
            df (pandas.DataFrame): The DataFrame containing the data to plot.
            file_path (str): The path of the source CSV file, used to generate the
                             output plot's filename.
        """
        if df is None:
            return

        # Sample data based on the interval
        df_sampled = df.iloc[::self.row_sample_interval]

        plt.figure(figsize=(12, 8))
        for column in df_sampled.columns:
            plt.plot(df_sampled[column], label=column)

        file_name = os.path.basename(file_path)
        base_name = os.path.splitext(file_name)[0]
        plt.title(f'All Columns from {base_name}')
        plt.xlabel('Index')
        plt.ylabel('Normalized Value')
        plt.legend(loc='center left', bbox_to_anchor=(1, 0.5))
        plt.grid(True)
        
        output_path = os.path.join(self.images_dir, f'{base_name}_all_columns.png')
        plt.savefig(output_path, bbox_inches='tight')
        plt.close()
        print(f"Saved plot for all columns to {output_path}")

    def plot_histograms_and_save(self, df, file_path):
        """
        Generates and saves histograms for each column in a DataFrame.

        This method creates a grid of histograms, one for each column in the DataFrame,
        to show the distribution of its values. The resulting image is saved to the
        directory specified during initialization.

        Args:
            df (pandas.DataFrame): The DataFrame containing the data for which
                                   histograms will be generated.
            file_path (str): The path of the source CSV file, used to generate the
                             output plot's filename.
        """
        if df is None:
            return

        file_name = os.path.basename(file_path)
        base_name = os.path.splitext(file_name)[0]
        
        num_columns = len(df.columns)
        num_rows = (num_columns + 2) // 3
        plt.figure(figsize=(15, 5 * num_rows))
        
        for i, column in enumerate(df.columns):
            plt.subplot(num_rows, 3, i + 1)
            df[column].hist(bins=50)
            plt.title(f'Histogram of {column}')
            plt.xlabel('Value')
            plt.ylabel('Frequency')
        
        plt.tight_layout()
        output_path = os.path.join(self.images_dir, f'{base_name}_histograms.png')
        plt.savefig(output_path)
        plt.close()
        print(f"Saved histograms for {base_name} to {output_path}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot data from transformed CSV files.")
    parser.add_argument('--interval', type=int, default=25,
                        help='Sampling interval for rows (e.g., 25 for every 25th row).')
    parser.add_argument('--histograms', action='store_true',
                        help='Generate histograms for all columns.')
    parser.add_argument('--params-file', type=str, default='json/parameters.json',
                        help='Path to the parameters JSON file.')
    args = parser.parse_args()

    data_directory = 'data'
    images_directory = 'images'
    plotter = DataPlotter(data_directory, images_directory, row_sample_interval=args.interval)
    
    try:
        with open(args.params_file, 'r') as f:
            params_data = json.load(f)
    except FileNotFoundError:
        print(f"Error: Parameters file not found at {args.params_file}")
        exit(1)
    except json.JSONDecodeError:
        print(f"Error: Could not decode JSON from {args.params_file}")
        exit(1)

    files_to_process_info = get_files_to_process(data_directory, params_data)

    if not files_to_process_info:
        print(f"No files to process based on parameters. Check {args.params_file} and {data_directory}.")
    else:
        for file_info in files_to_process_info:
            file_path = file_info['path']
            columns_to_plot = file_info['columns']
            
            print(f"Processing {file_path}...")
            df = plotter.read_data(file_path)
            if df is not None:
                # Ensure that columns to be plotted exist in the dataframe
                available_columns = [col for col in columns_to_plot if col in df.columns]
                missing = set(columns_to_plot) - set(available_columns)
                if missing:
                    print(f"Warning: Columns not found in {os.path.basename(file_path)} and will be skipped: {list(missing)}")
                
                if not available_columns:
                    print(f"No available columns to plot for {os.path.basename(file_path)}. Skipping.")
                    continue
                
                df_to_plot = df[available_columns]
                plotter.plot_and_save(df_to_plot, file_path)
                if args.histograms:
                    plotter.plot_histograms_and_save(df_to_plot, file_path)
