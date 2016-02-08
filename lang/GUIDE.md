# Translation Guide

### VARIABLES

Expressions, such as `%1`, `%2`, `%3`, etc. *stand for a certain variable*.

For example, `Error starting %1` will be automatically converted to:
- `Error starting Firefox`
- `Error starting Apktool`
- `Error starting GIMP`
- `Error starting application`
- ...

Note that these expressions:
- contain **NO** spaces (`% 1` is wrong);
- should **NOT** be written back to front (`1%` is wrong).

The only valid notations are `%1`, `%2` and so on.

--------------------

### SHORTCUTS

Sometimes you will meet the `&` character.
This character converts the following letter to a shortcut key, for example:

- `&File`     – <kbd>F</kbd> keyboard key will open the "File" menu;
- `&Settings` – <kbd>S</kbd> keyboard key will open the "Settings" dialog;
- `E&xit`     – <kbd>X</kbd> keyboard key will exit the application.

*Defining a shortcut key is up to you*, though there are some standards for any language.

**If a shortcut key is hard to decide, leave the string translation without a "&" sign at all.**
