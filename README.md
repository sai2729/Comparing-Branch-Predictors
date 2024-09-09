# Comparing Branch Predictors

This project compares branch prediction strategies, Global Address Prediction (GAp) and Per-Address Prediction (PAp), in C++. It analyzes prediction accuracy and memory usage with a 15-bit Branch History Register and 2-bit Pattern History Table, providing insights into their performance.

## Data Structures:
- *Branch History Table (BHT):* std::bitset<15> to track branch outcomes.
- *Pattern History Table (PHT):* std::vector<unsigned char> with 2-bit counters for each branch history.

## Key Features:
- Implemented Global and Per-Address prediction methods.
- Memory and accuracy trade-off analysis for different BHR sizes.
- Output predictions saved in output.txt file.

## Compilation:
Use a C++ compiler like g++ or clang++ with C++11 support.

Command:
```bash
g++ -o branchPredictor CombinedBranchPredictor.cpp
```

## How to Run:
Run the compiled executable, ensuring the trace file (gccSmall.trace) is in the same directory.

```bash
./branchPredictor
```

## Result Analysis
The project evaluates memory usage and prediction accuracy, showing higher accuracy with increased Branch History Register size, at the cost of increased memory consumption.
