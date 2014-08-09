# lookup: Search for a file in parent directories, up to / .

For example, `lookup .git` will search for `./.git`, `../.git`,
`../../.git`, and so on until `/` is reached. If the file is found, it
will print its absolute path and exit, otherwise it will return 1.

## Usage

    lookup [-p] [-r] FILENAME
    
## Options 

+ -p: only print the path for the parent directory

+ -r: print relative path (e.g. "../../../foo") not the absolute path
