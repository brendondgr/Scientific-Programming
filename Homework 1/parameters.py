import pandas as pd
import glob
import os
import argparse
import json

def get_csv_parameters(directory, normalize=False):
    """
    Generates a dictionary of parameters for each CSV file in a directory.

    This function scans a specified directory for CSV files, reads each file to
    determine its properties (like number of lines and column names), and
    compiles these into a dictionary. It can also set a normalization flag
    for the data processing step. Specific columns can be excluded for
    certain files.

    Args:
        directory (str): The path to the directory containing the CSV files.
        normalize (bool, optional): If True, sets a flag indicating that the
            data should be normalized. Defaults to False.

    Returns:
        dict: A dictionary where each key is a CSV filename and the value is
              another dictionary containing parameters like 'lines_to_read',
              'columns', 'file_name', and 'other_parameters'.
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

            other_params = {}
            if file_name == 'fetal.csv':
                other_params['do_not_include'] = ['histogram', 'mean', 'percent']
            
            other_params['normalize'] = normalize

            parameters[file_name] = {
                'lines_to_read': len(df),
                'columns': list(df.columns),
                'file_name': file_name,
                'other_parameters': other_params
            }
        except Exception as e:
            print(f"Could not process {file_path}: {e}")
            
    return parameters

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Generate parameters for CSV files in a directory.'
    )
    parser.add_argument(
        '--path',
        type=str,
        help='The path to the directory containing the CSV files.'
    )
    parser.add_argument(
        '--normalize',
        action='store_true',
        help='Set normalize parameter to True.'
    )
    args = parser.parse_args()

    if not args.path:
        print("Error: --path argument is required.")
        exit(1)

    directory_path = args.path
    csv_parameters = get_csv_parameters(directory_path, normalize=args.normalize)

    output_directory = 'json'
    os.makedirs(output_directory, exist_ok=True)
    output_filename = os.path.join(output_directory, 'parameters.json')
    with open(output_filename, 'w') as f:
        json.dump(csv_parameters, f, indent=4)

    print(f"Parameters saved to {output_filename}")
