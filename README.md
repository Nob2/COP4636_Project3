Compilation instructions:
    - This program has been tested on the Linux server
    - To compile the server run:
        - make main
    - To compile the client run:
        - make client
    - To compile both at once
        - make main client

Running:
    - To run the client type: ./client
        - The client will automatically connect to localhost.
        - For proper usage, ensure the server process is running first
    - To run the server, type ./main
        - On initialization, the server will bind to localhost and do the following:
        - Import all users stored in "users.txt"
        - Notify through console output, that it is ready to receive requests

How the output works:
    - The server and client utilizes a 2 step communication protocol
        - The client, tells the server what operation it would like to perform "login, logout, register, etc."
        - The server will then acknowledge this request, by sending an "Ok" message. If this message is not received,
        it is an invalid request. 
        - If valid, the server will output to it's console, the request that it received
        - It will go through the necessary data exchange with the client, and respond back with Success or Fail
    
    - See screenshots.docx for examples