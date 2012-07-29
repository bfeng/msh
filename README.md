msh -- A minix's shell program
===

Introduction
------------

msh which is short of Minix's Shell is initialized for the project assigned from the course CS551 at IIT.

In this project, you will write your own shell with some minimum requirements from us:

1. Your shell shall be invoked from the Ash shell provided with Minix.
2. Your shell shall first execute a PROFILE file which defines the “prompt sign” and allows you to access programs provided in /bin and /usr/bin. Once the PROFILE file is executed, you will be in a HOME directory specified by you in the PROFILE file.
3. In a command line of your shell you will be able to exercise utilities provided in /bin and /usr/bin. You will need to show that you can do the following: 
  * a) A command with or without argument such as: cat, ls
  * b) A command with or without argument whose output is redirected to a file such as:<code>ls –l > filename</code>
  * c) A command with or without argument whose output is piped to the input of another command such as:<code>cat filename | wc</code>

4. In a command line, your shell can execute a simple application program that will spawn a new process. The application will echo the process id of the new process then exits. 
5. In a command line, you can define integer variables (such as i with its value indicated by $i) and evaluate integer expressions. Cannot use environmental variables.
6. Your shell shall provide a facility that  allows a first command to be run and its output to be pasted back as input to another command. The first command is enclosed within $(…).
  E.g. <code>$ wc $(fgrep –l malloc *.c)</code> will count the number of  statements containing the string “malloc” in all the .c files under the current directory
  This problem must be implemented by using system call.
7. (Bonus): Can you nest the $(…) facility? Or other interesting feature.
8. Your shell shall be terminated by exit or ctl-c. If it is terminated by ctl-c, it first asks the question “Are you sure?”, and wait for confirmation before terminating. 
9. Submit the following in a VMplayer readable package:
  1. An executable of your shell
  2. Source code of your shell programs
  3. Any other executable and shell scripts to test your programs
  4. A readme on how to use and test your program
10. You will be graded on the robustness of your program.
11. Submit a design document of no more than 3 pages (minimum 10 points font) including figures, which must discuss exception handling. The project is due February 2, 2012; February 3 for remote and India students. Self evaluations are due 24 hours afterwards and are private.

Development
-----------
### Prerequisites
* git
* make
* gcc

### Steps

>####1. fork this project

>>     git clone git://github.com/bfeng/msh

>####2. write you code

>####3. commit

>>     git commit -a "Your comments"

>####4. push commits

>>     git push origin master

>####5. pull requests
>> [howto](http://help.github.com/send-pull-requests/)

License
-------
[GPL v3](http://www.gnu.org/licenses/gpl-3.0.txt)
