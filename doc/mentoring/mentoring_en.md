## Mentoring 11/05/2024

In the mentoring that has been had with the professor several doubts have been asked, and all of them have been solved:
- **Execv and error control**: It is clarified that exec only returns if some failure has happened when exec is executed, either because the path is incorrect, you do not have the necessary permissions to execute a command or for whatever reason. If execv works correctly, it returns with the status with which the executed command returns.

    Therefore, as we want to control the errors we have to take into account the following: 
    - Check with which error code it returns the commands, usually 0 for Success, and low values for Failure, usually 1. Then we can pick up the status of the executed command.
    - If it is exec that has failed, make an exit with a somewhat special code, to pick up the special status.

    In the case of wanting to nest ifs in the error control with different status values, we can resort to:

    ```
    if (WIFEXEXITED(status)){
        switch(WEXITSTATUS(status)){
            case 0:
                exit(EXIT_SUCCESS);
            case 1:
                /* Command failure, not exec failure*/.
            case SPECIAL_VALUE_ERROR:
                /* exec failure */
        }
    }
    ```

    In some cases it is not necessary to distinguish between the two errors.

- Internal fragmentation**: When internal fragmentation occurs in memory, the piece of memory block that is not complete will never be confused. 
    With the file management control of the operating system itself, the maximum file size is always known, therefore, despite the fact that internal fragmentation exists, it will be known when to stop reading in a block and not to read memory that is not from the file.

- Code advice**: The _idall.c_ file of exercise 3, which at the time of the tutorial had not been delivered, has been reviewed. Some advice has been given even though the code looked good:
    - Reminder to always declare variables at the beginning of functions, to make debugging easier. This is a good programming practice.
    - We can use any of `err(), errx(), warn(), warnx() or fprintf(stderr, “”) with exit()`. Being clear about what they do and when they paint errno. With `err() and errx()` you exit directly, there is no need to call `exit()`.
    - The PACT that has been reached in the course is that the reserved resources must be all released when we do `exit(EXIT_SUCCESS)`, that is, when we exit successfully. This has been commented because when we exit with error, we do not need to release. Although it is well done, it does not penalize if it is not done.
    - Function names should be named for what they do, if we want to calculate a cosine, let the function be called cosine. However, if it is a procedure, it can be `do_something()`.
    - It is also mentioned that constants are named in a way that does not follow the style followed in the subject, but that is not important.
    - It is also good practice to make short functions, for example, to pull the code from the child process and the parent process into separate functions.


Translated with DeepL.com (free version)