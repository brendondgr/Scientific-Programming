# Data Processing and Visualization Pipeline

This project provides a set of tools to process and visualize data from CSV files. It includes a C++ application for numerical processing and two Python scripts for generating configuration files and plotting the results.

## Setup and Execution

The following steps outline the process of compiling and running the different components of this project.

### 1. Generate Parameters

First, run the `parameters.py` script to scan the `data/` directory and create a `parameters.json` file. This file configures the C++ application.

**Usage:**

```bash
python3 parameters.py --path <directory_containing_csvs> [--normalize]
```

- `--path`: (Required) Specifies the directory where your CSV files are located.
- `--normalize`: (Optional) If included, the data will be normalized to a [0, 1] range during processing.

**Example:**

```bash
python3 parameters.py --path data/ --normalize
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
python3 visualize.py [--interval <sampling_interval>] [--histograms] [--params-file <path_to_json>]
```

- `--interval`: (Optional) An integer for the sampling interval (e.g., `25` plots every 25th row). Defaults to `25`.
- `--histograms`: (Optional) If included, generates histograms for each data column.
- `--params-file`: (Optional) Path to the JSON parameters file. Defaults to `json/parameters.json`.

**Example:**

```bash
python3 visualize.py --interval 50 --histograms
```

This will save the generated plots as PNG images in the `images/` directory.

### Cleaning Up

To remove the compiled C++ object files and the executable, run:

```bash
make clean
```
