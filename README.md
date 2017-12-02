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

The comparisons between runtimes were not fair. Project 2 had to merge all the sorted tables and then output a single CSV file, while Project 1 outputted separate CSV files for each sorted table. The overall graphs of both programs follow a similar trend because they both use mergesort. As the # of files increase, the time gap also increase with project 2 being on the slower end. This is because project 2 has to merge all the files together before proceeding to the printing, whereas project 1 directly goes to printing after the sorting has completed. If there were no differences, project 2 would be faster because project 1's memory space gets copied for each spawned child. Mergesort is a great option for a multithreaded sorting program because you can spawn threads each time you split the data before merging. In this project, I didn't exactly do this due to the limit of CPU cores in the iLab machines, but essentially I kind of did if you look at eat CSV file as a portion of a bigger data that eventually gets merged.