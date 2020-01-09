Simple wallpaper tagging system.
Using program 'feh' for wallpapers is recommended.

# How to use

Run
```
./build.sh
```
to compile 'tags'.
I then recommend moving the tags binary into some $PATH location.

## Help message
Run
```
tags
```
to get a full help message.

## Init
Run
```
tags init
```
in your wallpaper directory to create a .tags file, which is where all tag information will be kept.

#### Adding tags
To add tags to a file, run
```
tags add 'your_file' tag1 tag2 tag3 ...
```

## Removing tags
To remove tags from a file, run
```
tags remove 'your_file' tag1 tag2 tag3 ...
```
which will get rid of the given tags if they exist.

## Forgetting files
To forget tagged files, run
```
tags clear 'file1' 'file2' 'file3' ...
```
which will get rid of the given files from the record.

## Displaying tags
To see the tags of some files, run
```
tags display 'file1' 'file2' 'file3' ...
```
You can run
```
tags display *
```
to display the tags of all files in your current working directory.

## Querying tags
To query for files with some given tags, run
```
tags query tag1 tag2 tag3 ...
```
which will display all files with any of the given tags. If you only want files
which have ALL of the given tags, run query with the special keyword AND
```
tags query AND tag1 tag2 tag3 ...
```

## Counting tags
To count the number of occurences of some tags, run
```
tags count tag1 tag2 tag3
```
which will display the number of occurences of each tag in numerical order. If
you want to get the occurences of every single tag in every single file run
```
tags count
```

## Updating .tags file
If you've deleted some files or manually editted the .tags file, its recommended to run
```
tags update
```
which will get rid of deleted file records and make tags lowercase.

## Setting new wallpaper
If you use the program 'feh' for displaying your wallpaper, and you want to set a new wallpaper
from your collection, you can run
```
tags pape {'AND'} tag1 tag2 tag3 ...
```
The arguments to 'pape' are the same as for 'query'. This option will find all wallpapers
specified by your search requirements and show them in sxiv. In sxiv left-click to view image
in full, right-click to mark an image and press 'q' to quit (default bindings). If you've
marked an image and you quit sxiv, it will be made your new wallpaper.
