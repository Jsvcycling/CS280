# Program 01 Design Document

The program parses by paragraph. This means that once it reaches the end of the
paragraph, it calculates the output for it before continuing on to the next
paragraph.

The main loop first works by tokenizing the entire paragraph into a queue
consisting of individual tokens (A.K.A. words incl. punctuation). Upon paragraph
generation, these tokens are popped into an output array until the sum of the
token lengths is more than or equal to the defined line length. If the line
length is more, the final word is removed from the array and stored to be used
for the next line in the current paragraph. If the line length is equal to the
defined line length, then the program moves onto the next output line in the
paragraph.

If after the final word in the output array is removed, and the length of the
output line is less than the defined line length, N-spaces will be distributed
throughout the output line upon output printing.

In the case that the output array has only one item, but the current line length
is more than the defined line length, the "defined line length minus two"
character and all characters following are removed and stored to be used in the
next line in the current paragraph. The character at position "defined line
length minus one" is set to a dash ("-") symbol to indicate the continuation of
the word on the next line.

Upon encountering a like which contains only ".ll N" where N is an positive
integer between 10 and 120, the line length of all following paragraphs is
changed to the value of N (until the value is changed again).
