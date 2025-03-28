## listview ##

**Author: Timothy N Murphy <tnmurphy@gmail.com>**

A way for shell script to show a menu or list of options and then execute a command with arguments based on those options.

## What??? ##

{ for i in *.py; do echo "Search $i, $i"; done } | ./lv "Choose which files to search for the word bob" grep bob 

This will show a fullscreen selection a bit like this

```
Choose which files to search for the word bob
Search file1.py
Search file2.py
Search file3.py

```

you may select files with the spacebar and pressing enter will cause "grep" to be run with the selected options.

Options are piped into the standard input in the form "Display text, argument that will be passed to grep"

for example if all three files were selected then this would be the resulting command: 

```
grep bob file1.py file2.py file3.py
```


