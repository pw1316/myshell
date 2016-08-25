#myshell

This is a simple shell for linux with several simple internal commands.

#PS1 is in the format: [myshell:<your current dir>]>

	It simply shows which directory you are in and recommend you inputing.

#What can I input?

1)	simple internal commands.
		Usage: cd [<dir_name>]		change the directory to the one you input.
									or it will print current directory on the screen if you didn't input dir_name.
		Usage: dir [<dir_name>]		show the content of the directory.
									or it will show the content of the current directory.
		Usage: environ				show the environment variables.
		Usage: echo [<string-list>]	print what you input,just.
		Usage: clr					clear the screen.
		Usage: quit					exit myshell.
		Usage: help					open this readme file.

2)	output redirecting.
		Usage: cmd >(or >>) <filename>
		generally, command will output streams on the screen,which is called STDOUT.
		If you just input these simple commands they will output something to STDOUT so you can see them on the screen.
		However,you can change the place where to output.
		The way to do that is using "> filename" to guide the command to output in the file named filename.
		In that case you will see nothing but next PS1 on the screen.But you will find a new file in the place you have given.
		If you open that file,you will find the output in it.
		There are two modes,as listed before,">" and ">>".
			> means clear the file and write(new)
			>> means write streams behind the file tail(append)
			both will create a new file if the file does not exists.

3)	background commanding.
		in case 1) and 2) , myshell will do the command itself,which means if the command is not yet done,you can't do anything more.
		so we need background commanding.
		Usage: cmd &
		the charactor '&' means that this command is to be run in background.
		in that case,myshell will fork a new process,different from current process.
		the command will be run in the new process,so you can do next job in the myshell.
		however,those simple commands are so fast that it makes no sence using '&'

4)	input from file.
		Usage: myshell <file name>
		if you donn't want to input commands one by one each time,you can write them down in a file and input this command.
		myshell will open the file and read the contents inside. if one line makes a command,myshell will fork new process to do it.
		
5)	external commands.
		there are many commands stored in /bin/ ,if you use them,myshell will also fork new process to do it.
		you can also make external commands in the background.

#Written by : Joker Yough
#Last update: Aug. 9th 2015
