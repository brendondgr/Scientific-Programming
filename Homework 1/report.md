# Homework 1

## Question 1

The program uses `argc` and `argv` to accept input arguments in the following order: `data_directory` and `json_file`. If the arguments are not provided in this specific order, the program will display an error message explaining the correct usage and terminate. When the correct arguments are supplied, the program reads the specified CSV files and processes them based on the instructions in the JSON file, provided the files adhere to the expected format described in the JSON. If any required file is missing or improperly formatted, the program will display an error message and halt execution.

## Question 2

The parameter file uses a JSON format where each top-level key represents a dataset (like "fetal.csv" or "healthcare.csv").

For each dataset, the file contains required keys including "file_name" (the CSV filename), "lines_to_read" (number of data lines to process), and "columns" (an array of column names to consider).

Optional parameters are stored under "other_parameters" and include "do_not_include" (terms that filter out columns containing those terms) and "normalize" (a boolean for data normalization). 

The C++ code parses this structure using the nlohmann/json library, iterating through each dataset and applying the specified filtering and processing rules. Using a parameter file instead of hard-coding values provides significant advantages including the ability to process different datasets without recompilation, easy configuration changes by non-programmers, quick experimentation with different parameters, and the flexibility to deploy the same binary across different environments with different configuration needs.

## Question 3 
I chose fetal health monitoring data as my primary 1-D dataset, which includes measurements like baseline heart rate, accelerations, fetal movement, and uterine contractions. This dataset is more interesting than temperature because it involves multiple interconnected physiological parameters that collectively indicate fetal well-being, making it a complex multi-dimensional healthcare monitoring scenario rather than a simple environmental measurement. The program communicates to the reader the specific file being processed, the number of data lines read, and outputs the calculated mean and standard deviation for each monitored parameter, providing immediate statistical insight into the fetal health indicators.

## Question 4
The formulas I used are:
- Mean: μ = (Σx_i) / n, where x_i are the data points and n is the sample size
- Standard deviation: σ = √[(Σ(x_i - μ)²) / n], using the sample standard deviation formula

I implemented a method using `std::accumulate` for calculating the sum to find the mean, then computed the standard deviation by iterating through the data again to calculate the sum of squared differences from the mean. This two-pass approach ensures numerical stability and accuracy, as it avoids potential precision issues that can occur with single-pass computational formulas when dealing with large datasets or when the mean is significantly different from individual data points.

## Question 5
To print a floating-point value to two decimal places using `<iomanip>`, I use `std::fixed << std::setprecision(2)`. 

`std::fixed` formats numbers in fixed-point notation (like 123.45), while `std::scientific` formats numbers in scientific notation (like 1.23e+02). When used with `std::setprecision(n)`, `std::fixed` specifies exactly n digits after the decimal point, whereas `std::scientific` specifies n significant digits total in the mantissa. Without either manipulator, `std::setprecision` controls the total number of significant digits and the system chooses between fixed or scientific notation automatically.

## Question 6
`std::vector<double>` is appropriate for 1-D data because it provides dynamic sizing and automatic memory management. Unlike raw arrays, vectors prevent memory leaks, offer bounds checking, and provide safe iterators. They support standard algorithms and can grow/shrink as needed without manual memory allocation.

The amortized complexity of `push_back` is O(1) - while individual insertions may take O(n) during reallocation, the average cost remains constant. `reserve` is helpful when you know the approximate data size in advance, as it pre-allocates memory and prevents multiple reallocations during loading.

## Question 7
I would use C++ facilities like `std::chrono::steady_clock` to time the program by capturing time points before and after operations using `std::chrono::steady_clock::now()`, then calculating the duration with `std::chrono::duration_cast`. For example, I would wrap computation sections with timing code to measure end-to-end execution time, I/O operations separately, or just the core statistical calculations.The timing results would be reported in milliseconds or microseconds using appropriate duration types.

However, timing was not implemented in this assignment since it was not specifically requested in the problem statement, though it would be valuable for performance analysis when processing large datasets or comparing different algorithmic approaches for statistical computations.

## Question 8
I automated batch runs by creating a Python script (`parameters.py`) that scans a directory for CSV files and generates a JSON configuration file containing parameters for each dataset. This script automatically discovers all CSV files in the specified directory (excluding previously transformed files to avoid reprocessing), reads each file to determine properties like column count and data lines, and creates appropriate parameter sets.

The automation workflow works as follows:
1. Run `python parameters.py --path data --normalize` to generate `json/parameters.json`
2. Execute `./main data json/parameters.json` to process all datasets in a single run
3. The C++ program iterates through each dataset configuration in the JSON file

For output file naming, I implemented a systematic convention to prevent overwrites:
- Summary statistics files use the pattern: `{original_filename}_summary.csv` 
- Transformed data files use the pattern: `{original_filename}_transformed.csv`
- For example, `fetal.csv` generates `fetal_summary.csv` and `fetal_transformed.csv`

This approach provides several advantages: the Python script eliminates manual parameter file creation, the single C++ execution processes multiple datasets efficiently, and the naming convention ensures each dataset's outputs are clearly identified and separated. The normalization flag can be toggled via command line argument, allowing easy experimentation with different processing modes across all datasets simultaneously.

## Question 9
My `Makefile` is organized with these main targets:

- **`all`**: Default target that builds the executable
- **`o`** (the executable): Links object files to create the final binary
- **`clean`**: Removes generated files (`*.o` and the executable)
- **Pattern rule `%.o: %.cpp`**: Compiles source files to object files

The Makefile uses standard variables: `CXX=g++`, `CXXFLAGS=-std=c++17 -Wall -Iincludes`, `TARGET=o`, and automatically generates object file names from sources.

In VS Code, I configure the build task in `.vscode/tasks.json` to run `make` directly. Program arguments are passed either through the integrated terminal (`./o data json/parameters.json`) or via `.vscode/launch.json` for debugging sessions.

## Question 10
I produce CSV files in a standardized format with comma-separated values and header rows. The exact file formats are:

**Summary files** (`*_summary.csv`): Contains statistical metadata with columns:
- `Statistic`: The type of statistic (e.g., "Mean", "Standard Deviation", "Data Points Read")
- One column for each processed data column containing the computed values

**Transformed data files** (`*_transformed.csv`): Contains normalized data with:
- Original column headers preserved
- Data values linearly transformed to the range [0, 1]
- Same row structure as input files (up to the specified `lines_to_read` limit)

My Python plotting script (`visualize.py`) reads these files using pandas `read_csv()` and creates two types of visualizations:

1. **Line plots** (`*_all_columns.png`): Shows all columns as separate lines plotted against row index, with each column clearly labeled in the legend
2. **Histogram plots** (`*_histograms.png`): Displays distribution of values for each column in a grid layout

The plotting pipeline uses a parameters-driven approach:
- `get_files_to_process()` reads the JSON configuration to determine which files to visualize
- It automatically detects whether to use original or transformed data based on the `normalize` parameter
- Columns are filtered according to `do_not_include` specifications
- The `DataPlotter` class handles file reading and plot generation with configurable sampling intervals

To verify curve correspondence, I implemented several safeguards:
- File names are preserved through the transformation pipeline (e.g., `fetal.csv` → `fetal_transformed.csv` → `fetal_transformed_all_columns.png`)
- Plot titles include the source filename for clear identification
- Legend labels match the original column names from the CSV headers
- The script warns about missing files or columns and skips invalid configurations
- Each plot is saved with a descriptive filename that traces back to the original dataset

This systematic approach ensures that each curve in the legend corresponds exactly to the correct column from the specified dataset and parameter configuration.
