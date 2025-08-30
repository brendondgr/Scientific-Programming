import pandas as pd
import glob
import os
import argparse
import json

def get_csv_parameters(directory):
    """
    Generates a dictionary of parameters for each CSV file in a directory.
    """
    csv_files = [
        file for file in glob.glob(os.path.join(directory, '*.csv'))
        if "_transformed" not in os.path.basename(file)
    ]
    parameters = {}

    for file_path in csv_files:
        try:
            df = pd.read_csv(file_path)
            file_name = os.path.basename(file_path)

            # Handle unnamed columns
            df.columns = [
                col if not col.startswith('Unnamed:') else ''
                for col in df.columns
            ]

            parameters[file_name] = {
                'lines_to_read': len(df),
                'columns': list(df.columns),
                'file_name': file_name,
                'other_parameters': {}
            }
        except Exception as e:
            print(f"Could not process {file_path}: {e}")
            
    return parameters

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate parameters for CSV files in a directory.')
    parser.add_argument('--path', type=str, help='The path to the directory containing the CSV files.')
    args = parser.parse_args()

    if not args.path:
        print("Error: --path argument is required.")
        exit(1)

    directory_path = args.path
    csv_parameters = get_csv_parameters(directory_path)

    output_directory = 'json'
    os.makedirs(output_directory, exist_ok=True)
    output_filename = os.path.join(output_directory, 'parameters.json')
    with open(output_filename, 'w') as f:
        json.dump(csv_parameters, f, indent=4)

    print(f"Parameters saved to {output_filename}")
