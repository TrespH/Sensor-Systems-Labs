%% PORT CONFIGURATION
clear
clc
close all

% List available serial ports
availablePorts = serialportlist;

% If no ports are available, display an error
if isempty(availablePorts)
    error('No serial ports available.');
end

% Display a dialog to select the serial port
[selectedPort, ok] = listdlg('PromptString', 'Select a serial port:', ...
                             'SelectionMode', 'single', ...
                             'ListString', availablePorts);

% If the user cancels the dialog, exit the script
if ~ok
    disp('User canceled port selection.');
    return;
end

% Get the selected port
serialPort = availablePorts{selectedPort};

% Specify the baud rate
baudRate = 115200;

% Specify the acquisition time (in seconds)
acquisitionTime = 10;  % Set the amount of time you want to acquire data

% Create the serial object
s = serialport(serialPort, baudRate);

% Set the terminator (optional, depends on your device)
configureTerminator(s, "LF"); % or 'CR', 'CR/LF', etc.

% Specify the timeout of the communication with the port (in seconds)
timeout = 1;  % Set a small timeout for reading
s.Timeout = timeout;

%% DATA ACQUISITION
% Initialize variables
data = []; % Array to store the received data
startTime = tic;  % Start a timer

% Read data for the specified amount of time
disp('Reading serial data...');

while toc(startTime) < acquisitionTime
    % Check if data is available
    if s.NumBytesAvailable > 0
        % Read the available data
        line = readline(s);  % Read a line of data as a string

        % Split the line into separate numbers (doesn't do anything if
        % there are no commas)
        values = str2double(split(line, ','));  % Convert to array of numbers

        % Store the array of numbers in the Data matrix (array if there are
        % no commas)
        data(end+1,:) = values;
        
    end
end

% Close the serial port
clear s;

%% PLOT
% Plot the collected data

figure;
plot(data, '-o');
title('Acquired Data');
xlabel('Sample Number');
ylabel('Data Value');
grid on;
legend('First curve'); % add as many strings as the number of curves you are plotting

disp('Data acquisition complete. Plotting data...');

%% SAVE
% Save the workspace (all the variables created so far) in the current folder to use them in a 
% second time or in a different program

% Save('workspace.mat')
% Save('data.mat', 'data') % if you want to save in the file only the variable data