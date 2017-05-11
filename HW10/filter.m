port='COM4'; % Edit this with the correct name of your PORT.

% Makes sure port is closed
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end
fprintf('Opening port %s....\n',port);

% Defining serial variable
mySerial = serial(port, 'BaudRate', 230400);

% Opening serial connection
fopen(mySerial);

% Writing some data to the serial port
fprintf(mySerial,'r \n')

% Reading the echo from the PIC32 to verify correct communication
for i = 1:100
    data_read(i,1:4) = fscanf(mySerial,'%d %d %d %f\n');
end

% Closing serial connection
fclose(mySerial)

plot(data_read(5:100,2))
hold on
plot(data_read(5:100,3))
plot(data_read(:,4))
legend('Original Data','MAF','IIR')
