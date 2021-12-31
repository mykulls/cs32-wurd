# cs32-wurd
Wurd text editor made in C++ for UCLA's COM SCI 32 course.

You can run it yourself by running the Wurd.exe file under x64/Debug. It will automatically load the dictionary.txt file inside that folder as its dictionary. Highlighted words in red indicate words unknown to the currently loaded dictionary.

Available commands:
- Up, Down, Left, Right arrows - Move the cursor
- Home (Fn+Left on a Mac) - Move to start of the current line
- End (Fn+Right on a Mac)- End of the current line
- PgUp (Fn+Up on a Mac) - Up one page
- PgDown (Fn+Down on a Mac) = Down one page
- Delete (Fn+Delete on a Mac) - Delete the character under the cursor
- Backspace (Delete on a Mac) - Backspace over the previous character
- Ctrl-Z - Undo last change
- Ctrl-S - Save current the file
- Ctrl-L - Load a new file (on success, discard any changes to the current file)
- Ctrl-D - Load a new dictionary (on success, discard the entire old dictionary)
- Ctrl-X - Exit the editor (discard any changes to the current file)

An excerpt of War and Peace in the Wurd editor:
![Wurd screenshot](https://user-images.githubusercontent.com/69874869/147801008-9e41fe56-3935-42ba-9735-8a4fefd50712.png)
