## listview ##

**Author: Timothy N Murphy <tnmurphy@gmail.com>**

A way for shell script to show a menu or list of options and then obtain a list of those which were selected by the user. 

## What??? ##


As an example - we want to let the user choose which files to add to a backup archive: 

```
ls * | tar -czf backup.tgz $(./lv "Choose which files to add to the archive")
```

This will show a fullscreen selection a bit like this:

```
Choose which files to search for the word bob
Search file1.py
Search file2.py
Search file3.py

```

The user may select files with the spacebar - a star will appear on the screen next to it. To complete the selection the user presses enter.

The selected files are then written one per line to the stdout and may be captured for use in a script as with the example above.

## -u Option ##
The "unique" option will allow only one option to be selected from the list. This is obviously useful with menus.

```
```

## Different menu text ##
It is possible for a displayed option to have different text from what gets sent to the output.

If there is a comma in the input the text before the comma is displayed in the menu but should that option be selected by the user the text that gets sent to stdout will be what comes after the comma.

e.g. 

```
ls $(echo -e "Option 1 - list all pictures,*.png\nOption 2 - list all C source files,*.c" | lv "Select file types to list")

.... will result in something like:

ls *.png

...or

ls *.c

```

This is a case where what you want to happen is different from the descriptive text that you wish to describe the option. 
