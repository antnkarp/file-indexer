# file-indexer
This is a simple application that traverses directories and shows details on certain files. It is designed for Linux operating systems.
This procedure of gathering information is referred to as indexing.

The program stores information about:
* directories
* JPEG images
* PNG images
* gzip archives
* zip files (including any files based on zip format like .docx, .odt, ...)

File type recognition is based on a file signature (so called magic number).</br>
Following information is collected by the program:
* file name
* absolute path to a file
* size
* owner's uid
* type (one of the above)

The data structure that contains all of the collected information is referred to as index.

## Build
`make` command produces one executable: `file_indexer`

## Invocation
The program has following command line arguments:
* `-d path`<br/>
   The path to a directory that will be recursively traversed.
   
* `-f path`<br/>
   The path to a file where index is stored.

* `-t n`<br/>
   n denotes the time between subsequent rebuilds of index. This parameter is optional.

## Available commands
* `exit`<br/>
   Starts the termination procedure. If indexing is in progress, the program waits for it to finish.
   
* `exit!`<br/>
   Quick termination. If indexing is in progress, it is cancelled.
   
* `index`<br/>
   Issues indexing if there is no ongoing indexing.

* `count`<br/>
   Calculates and displays the number of each file type present in the index.
   
* `largerthan x`<br/>
   Prints information in the index for files that have size larger than x (in bytes)/
   
* `namepart y`<br/>
   y is a part of a filename, it can contian spaces. Prints information about all files in the index for which y is a substring of the name.
   
* `owner z`<br/>
   z is an uid. Prints information about all files in the index whose owner has uid equal to z.
   
