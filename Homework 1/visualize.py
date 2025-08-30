
import pandas as pd
import matplotlib.pyplot as plt
import os

class DataPlotter:
    def __init__(self, data_dir, images_dir):
        self.data_dir = data_dir
        self.images_dir = images_dir
        if not os.path.exists(self.images_dir):
            os.makedirs(self.images_dir)

    def find_transformed_files(self):
        transformed_files = []
        for file_name in os.listdir(self.data_dir):
            if file_name.endswith('_transformed.csv'):
                transformed_files.append(os.path.join(self.data_dir, file_name))
        return transformed_files

    def read_data(self, file_path):
        try:
            df = pd.read_csv(file_path)
            return df
        except Exception as e:
            print(f"Error reading {file_path}: {e}")
            return None

    def plot_and_save(self, df, file_path):
        if df is None:
            return

        plt.figure(figsize=(12, 8))
        for column in df.columns:
            plt.plot(df[column], label=column)

        file_name = os.path.basename(file_path)
        plt.title(f'All Columns from {file_name}')
        plt.xlabel('Index')
        plt.ylabel('Normalized Value')
        plt.legend(loc='best')
        plt.grid(True)
        
        output_path = os.path.join(self.images_dir, f'{file_name}_all_columns.png')
        plt.savefig(output_path)
        plt.close()
        print(f"Saved plot for all columns to {output_path}")

if __name__ == "__main__":
    data_directory = 'data'
    images_directory = 'images'
    plotter = DataPlotter(data_directory, images_directory)
    transformed_files = plotter.find_transformed_files()

    if not transformed_files:
        print(f"No *_transformed.csv files found in {data_directory}")
    else:
        for file_path in transformed_files:
            print(f"Processing {file_path}...")
            df = plotter.read_data(file_path)
            if df is not None:
                plotter.plot_and_save(df, file_path)
