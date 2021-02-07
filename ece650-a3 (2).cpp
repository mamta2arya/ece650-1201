#include <unistd.h>
#include <signal.h>
#include <vector>
#include <cstddef>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <bits/stdc++.h>

std::vector<pid_t> pids;

int process_a2(void)
{
    // Process B writing to C
    while (!std::cin.eof())
    {
        // reading a line of input until End of line and store in a string
        std::string line1;
        std::getline(std::cin, line1);
        std::cout << line1 << std::endl;
    }
    std::cout << "[B] saw EOF" << std::endl;
    return 0;
}

void kill_all_childs() {
    for (pid_t k : pids)
    {

        kill(k, SIGTERM);
    }
}

void clean_up_child_process(int signal_number)
{
    /* Clean up the child process. */
    int status;
    wait(&status);
    kill_all_childs();
    exit(status);
}

int main(int argc, char **argv)
{
    /* Handle SIGCHLD by calling clean_up_child_process. */
    struct sigaction sigchld_action;
    memset(&sigchld_action, 0, sizeof(sigchld_action));
    sigchld_action.sa_handler = &clean_up_child_process;
    sigaction(SIGCHLD, &sigchld_action, NULL);

    pid_t child_pid;

    // create file descriptors for pipe
    // 0 - read
    // 1 - write
    int rgen_to_a1_fd[2];
    int a1_to_a2_fd[2];

    if (pipe(rgen_to_a1_fd) == -1)
    {
        std::cerr << "Error: Pipe failed" << std::endl;
        return 1;
    }
    if (pipe(a1_to_a2_fd) == -1)
    {
        std::cerr << "Error: Pipe failed" << std::endl;
        return 1;
    }

    // fork a child process to run rgen program
    child_pid = fork();
    if (child_pid == 0)
    {
        // connect rgen stdout to file descriptor
        dup2(rgen_to_a1_fd[1], STDOUT_FILENO);
        close(rgen_to_a1_fd[0]);
        close(rgen_to_a1_fd[1]);

        int rgen = execv("./rgen", argv);
        std::cerr << "Error: Fail to run rgen program" << std::endl;
        return rgen;
    }
    pids.push_back(child_pid);

    // fork a child process to run assignment 1 program
    child_pid = fork();
    if (child_pid == 0)
    {
        // connect assignment 1 stdin to file descriptor
        dup2(rgen_to_a1_fd[0], STDIN_FILENO);
        close(rgen_to_a1_fd[0]);
        close(rgen_to_a1_fd[1]);

        // connect assignment 1 stdout to file descriptor
        dup2(a1_to_a2_fd[1], STDOUT_FILENO);
        close(a1_to_a2_fd[0]);
        close(a1_to_a2_fd[1]);

        char *const a1_args[3] = {"python3", "./ece650-a1.py", NULL};
        int a1 = execvp(a1_args[0], a1_args);
        std::cerr << "Error: Fail to run assignment 1 program" << std::endl;
        return a1;
    }
    pids.push_back(child_pid);

    // fork a child process to run assignment 2 program
    child_pid = fork();
    if (child_pid == 0)
    {
        // connect assignment 2 stdin to file descriptor
        dup2(a1_to_a2_fd[0], STDIN_FILENO);
        close(a1_to_a2_fd[0]);
        close(a1_to_a2_fd[1]);

        char *const a2_args[2] = {"./a2ece650", NULL};
        int a2 = execv(a2_args[0], a2_args);
        std::cerr << "Error: Fail to run assignment 2 program" << std::endl;
        return a2;
    }
    pids.push_back(child_pid);

    dup2(a1_to_a2_fd[1], STDOUT_FILENO);
    close(a1_to_a2_fd[0]);
    close(a1_to_a2_fd[1]);

    int X = process_a2();

    // kill all the process
    kill_all_childs();
    
    return X;
}