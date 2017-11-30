# CS214 Project 2: Basic Data Sorter - multithreaded

## Usage

### Colmpilation

    make sorter

### CLI

    ./sorter -c ColumnHeader -d Directory -o Output

*ColumnHeader* is the column header to sort on. **Required**.
*Directory* is the directory to search for CSV files. Searches current directory if not specified.
*Output* is the directory to output the sorted CSV file. Outputs in the current directory if not specified.

The flags can be placed in any order.

## Assumptions

CSV files are proper movie_metadata files specified in project 0, although my program does fail gracefully if otherwise.

## Design

My program processes a directory and spawns threads to process every subdirectory or sort every file found in the directory. When sorting a file, if it does not have a ".csv" extension or is not a proper movie_metadata CSV file, the thread exits. The thread validates if the CSV file is "proper" while tokenizing the first row. If all goes well, and the file sorts, the thread then merges with another thread resulting in a new merged table from both threads. The merged thread then tries to merge with another thread, and this process continues until there is only one thread left with a "proper" CSV file's table. This thread then outputs the merged/sorted table to the specified directory as a CSV file.

## Analysis