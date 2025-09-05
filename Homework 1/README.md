# Data Processing and Visualization Pipeline

This project provides a set of tools to process and visualize data from CSV files. It includes a C++ application for numerical processing and two Python scripts for generating configuration files and plotting the results.

## Full Pipeline Execution (Commands ONLY)
Here is the full Pipeline if you want to save some time.
### Linux (UV)
```bash
uv init
uv sync
uv pip install -r requirements.txt
uv run parameters.py --path data/ --normalize
make
./o data/ json/parameters.json
uv run visualize.py --ascending
make clean
```

### Windows (UV)
```bash
uv init
uv sync
uv pip install -r requirements.txt
uv run parameters.py --path data/ --normalize
make
o.exe data/ json/parameters.json
uv run visualize.py --ascending
make clean
```




## Setup and Execution

The following steps outline the process of compiling and running the different components of this project.

### 1. Generate Parameters

First, run the `parameters.py` script to scan the `data/` directory and create a `parameters.json` file. This file configures the C++ application.

**Usage:**

```bash
python parameters.py --path <directory_containing_csvs> [--normalize]
```

- `--path`: (Required) Specifies the directory where your CSV files are located.
- `--normalize`: (Optional) If included, the data will be normalized to a [0, 1] range during processing.

**Example:**

```bash
python parameters.py --path data/ --normalize
```

### 2. Compile and Run the C++ Application

After generating the parameters, compile the C++ application using the provided `Makefile`.

**Compilation:**

```bash
make
```

This will create an executable file named `o` in the root directory.

**Execution:**

Run the compiled program, providing the data directory and the path to the `parameters.json` file.

**Usage:**

```bash
./o <data_directory> <json_file>
```

**Example:**

```bash
./o data/ json/parameters.json
```

The application will process the data as specified in the JSON file, creating summary CSVs and, if normalization is enabled, transformed data CSVs in the `data/` directory.

### 3. Visualize the Results

Finally, use the `visualize.py` script to generate plots from the processed data.

**Usage:**

```bash
python visualize.py [--data-dir <directory_where_data_is>] [--images-dir <directory_to_save_images>] [--ascending | --descending]
```

- `--data-dir`: (Optional) Specifies the directory where your data files are located. Defaults to `data`.
- `--images-dir`: (Optional) Specifies the directory where the generated plots will be saved. Defaults to `images`.
- `--ascending`: (Optional) Sorts the data in ascending order.
- `--descending`: (Optional) Sorts the data in descending order.

**Example:**

```bash
python visualize.py --ascending
```

This will save the generated plot as a PNG image in the `images/` directory.

### Cleaning Up

To remove the compiled C++ object files and the executable, run:

```bash
make clean
```
