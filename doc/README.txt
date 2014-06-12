Team 26 - Rain Dartt, Dharmin Shah, and Rob Wagner

--Building/Running--

	To build and run the OS, make sure you are in the ProjectE directory,
	then type $./compileOS.sh
	and the shell will come up.

--Features to Remember--

	The following shell commands are available to the user:
	
	>type filename
		- prints the contents of the file specified, line by line, until a null character is read from the file.
	>execute filename
		- runs the filename as a program (compiled, assembled, and linked machine code)
	>delete filename
		- allows the memory of a filename to be overwritten and prevents it from being displayed by >type.
	>copy filename1 filename2
		- copies the contents of filename1 into filename2; can create filename2 if it does not exist already.
	>dir
		also (>ls ) - prints out the non-deleted file names as listed in the directory.
	>create filename
		- prompts the user to enter lines of characters to input into a file; ends and saves the file when the user inputs an empty line.
	>kill processID
		- kills a process the has been executed and sets its entry in the process table to inactive
	>help
		(also >helplt) - prints out the helplt.txt that shows all available commands.
	>clear
		- clears the text displayed by the shell and leaves only a single prompt
	>chclr[0-F]
		- changes the color of text displayed by the shell to one of sixteen specified colors.
	
--Three Questions--

	How can we fix the loading sequence to avoid having to load shell twice?
	How can we add code to implement more features without breaking bochs?
	How can we transfer our code to an actual floppy disk and run it?


