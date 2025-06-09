# .tar-file-reader-with-C
Creating a web app using react+vite and creating a http server and service for reading a .tar file using C

# Starting the program using Docker CLI
Inside the main project directory, there is a Dockerfile and docker-compose.yml file. To start the program, do these steps:
- install the Docker engine and CLI
- using the command prompt or terminal (depending on the OS of the machine) navigate to the project's main directory
- in the terminal run this command: "docker-compose up --build"
- after the container has been created, open a browser window and type in the URL of the locally hosted web app: http://localhost:8080
  (port 8080 on the host is mapped to port 80 of the container)
