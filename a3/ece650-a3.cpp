#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <rgen-executable-path>" << std::endl;
        return 1;
    }

    // Command-line arguments for rgen
    const char* rgenPath = argv[1];
    char* rgenArgs[] = {const_cast<char*>(rgenPath), (char*)"-s", (char*)"5", (char*)"-n", (char*)"4", (char*)"-l", (char*)"5", nullptr};

    // Create pipes for IPC
    int pipeFromRgen[2];
    int pipeToPython[2];
    int pipeToShortestPath[2];

    if (pipe(pipeFromRgen) == -1 || pipe(pipeToPython) == -1 || pipe(pipeToShortestPath) == -1) {
        std::cerr << "Error creating pipes" << std::endl;
        return 1;
    }

    // Fork the first child process for rgen
    pid_t rgenPid = fork();

    if (rgenPid == -1) {
        std::cerr << "Error forking rgen process" << std::endl;
        return 1;
    }

    if (rgenPid == 0) {
        // Child process for rgen
        close(pipeFromRgen[0]);  // Close the read end
        dup2(pipeFromRgen[1], STDOUT_FILENO);  // Redirect stdout to the pipe
        close(pipeFromRgen[1]);

        execvp(rgenPath, rgenArgs);
        std::cerr << "Error executing rgen" << std::endl;
        return 1;
    } else {
        // Parent process
        close(pipeFromRgen[1]);  // Close the write end

        // Fork the second child process for Python
        pid_t pythonPid = fork();

        if (pythonPid == -1) {
            std::cerr << "Error forking Python process" << std::endl;
            return 1;
        }

        if (pythonPid == 0) {
            // Child process for Python
            close(pipeToPython[0]);  // Close the read end
            dup2(pipeToPython[1], STDOUT_FILENO);  // Redirect stdout to the pipe
            close(pipeToPython[1]);

            // Execute the Python script
            const char* pythonPath = "python";
            char* pythonArgs[] = {const_cast<char*>(pythonPath), (char*)"ece650-a1.py", nullptr};
            execvp(pythonPath, pythonArgs);
            std::cerr << "Error executing Python script" << std::endl;
            return 1;
        } else {
            // Parent process
            close(pipeToPython[1]);  // Close the write end

            // Create a vector to store the child process PIDs
            std::vector<pid_t> childProcesses = {rgenPid, pythonPid};

            // Fork the third child process for the shortest path program
            pid_t shortestPathPid = fork();

            if (shortestPathPid == -1) {
                std::cerr << "Error forking the shortest path program" << std::endl;
                return 1;
            }

            if (shortestPathPid == 0) {
                // Child process for the shortest path program
                close(pipeToShortestPath[1]);  // Close the write end
                dup2(pipeToShortestPath[0], STDIN_FILENO);  // Redirect stdin to the pipe
                close(pipeToShortestPath[0]);

                // Execute the shortest path program (provide the path to your a2-ece650 here)
                const char* shortestPathPath = "./a2-ece650";
                char* shortestPathArgs[] = {const_cast<char*>(shortestPathPath), nullptr};
                execvp(shortestPathPath, shortestPathArgs);
                std::cerr << "Error executing the shortest path program" << std::endl;
                return 1;
            } else {
                // Parent process
                close(pipeToShortestPath[0]);  // Close the read end

                // Read output from rgen and forward it to the Python process
                char buffer[256];
                ssize_t bytesRead;

                while ((bytesRead = read(pipeFromRgen[0], buffer, sizeof(buffer))) > 0) {
                    write(pipeToPython[0], buffer, bytesRead);
                }

                close(pipeFromRgen[0]);
                close(pipeToPython[0]);

                // Read shortest path queries from stdin and forward them to the shortest path program
                std::string line;
                while (std::getline(std::cin, line)) {
                    write(pipeToShortestPath[1], line.c_str(), line.length());
                    write(pipeToShortestPath[1], "\n", 1);
                }

                close(pipeToShortestPath[1]);

                // Wait for all child processes to finish
                int status;
                for (pid_t childPid : childProcesses) {
                    waitpid(childPid, &status, 0);
                }
            }
        }
    }

    return 0;
}





