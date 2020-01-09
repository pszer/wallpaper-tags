Simple wallpaper tagging system.

# How to use

Run
```
tags
```
to get a full help message.

Run
```
tags init
```
in your wallpaper directory to create a .tags file, which is where all tag information will be kept.

To add tags to a file, run
```
tags add 'your_file' tag1 tag2 tag3 ...
```

To remove tags from a file, run
```
tags remove 'your_file' tag1 tag2 tag3 ...
```
which will get rid of the given tags if they exist.

To forget tagged files, run
```
tags clear 'file1' 'file2' 'file3' ...
```
which will get rid of the given files from the record.

To see the tags of some files, run
```
tags display 'file1' 'file2' 'file3' ...
```

To query for files with some given tags, run
```
tags query tag1 tag2 tag3 ...
```
which will display all files with any of the given tags. If you only want files
which have ALL of the given tags, run query with the special keyword AND
```
tags query AND tag1 tag2 tag3 ...
```

To count the number of occurences of some tags, run
```
tags count tag1 tag2 tag3
```
which will display the number of occurences of each tag in numerical order. If
you want to get the occurences of every single tag in every single file run
```
tags count
```

If you've deleted some files or manually editted the .tags file, its recommended to run
```
tags update
```
which will get rid of deleted file records and make tags lowercase.

If you use the program 'feh' for displaying your wallpaper, and you want to set a new wallpaper
from your collection, you can run
```
tags pape {'AND'} tag1 tag2 tag3 ...
```
The arguments to 'pape' are the same as for 'query'. This option will find all wallpapers
specified by your search requirements and show them in a feh slideshow in a new window. In
feh, press LEFT/RIGHT ARROWS to switch between images in the slideshow and press RETURN on an
image to make it your wallpaper.
