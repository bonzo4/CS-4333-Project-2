# Instructions to run:
1. Compile receiver:
   ```bash
   g++ -o receiver_program receiver/main.cpp receiver/receiveFile.cpp receiver/Receiver.cpp
   ```
2. Compile sender:
   ```bash
   g++ -o sender_program sender/main.cpp sender/sendFile.cpp
   ```
3. Run receiver (in one terminal):
   ```bash
   ./receiver_program
   ```
4. Run sender (in another terminal):    
   ```bash
   ./sender_program
   ```