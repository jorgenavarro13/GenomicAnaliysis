Hello, I hope you're reading this

## Quick summary of how to setup the environment
1.- Setup the web
Clone the repo
cd GenomicAnalysis
cd web

We're going to create an http server with python so:
python3 -m http.server 8000

2.- Setup the c++ server
cd ../servers
g++ -std=c++17 -pthread -o server server.cpp coincidence.cpp && echo "Compiled" && ./server

The page should be displaying the webpage on port 8000
And the c++ server should be available on port 8080