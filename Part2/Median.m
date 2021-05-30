%clear existing variables, close any figure windows, clear the command line
close all
clear all
clc

% Read image
filename = 'Lena.pgm';
im_orig = imread(filename);
im_orig_info = imfinfo(filename);
numRows = im_orig_info.Height;
numCols = im_orig_info.Width;
imgSize = numRows * numCols;
z = floor(imgSize/20);

% Clear any existing serial ports
delete(instrfind);

% Create a serial port object
serialPort = serial('COM8', 'BaudRate', 115200, 'DataBits', 8, 'Parity', 'none', 'StopBit', 1, 'OutputBufferSize', imgSize, 'InputBufferSize', imgSize);

% open the serial port for reading/writing
fopen(serialPort);

%% WRITE IMAGE
fprintf('Sending Number of Rows to FPGA... ');
fwrite(serialPort, numRows, 'uint16');

fprintf('Sending Number of Columns to FPGA... ');
fwrite(serialPort, numCols, 'uint16');

fprintf('Writing Image Data to FPGA... '); tic;
fwrite(serialPort, im_orig(:)); toc;

% TODO Uncomment to receive timing values (LSB first)
write_time = double(uint32(fread(serialPort, 1, 'uint32'))) / 100.000e6;
fprintf('Reported time: %g\n', write_time);
%
fprintf('\n');


%% PROCESS IMAGE

% TODO Uncomment to receive timing values (LSB first)
fprintf('Waiting for image to process... '); tic;
proc_time = double(uint32(fread(serialPort, 1, 'uint32'))) / 100.000e6; toc;    % Data expected lowest significant byte first
%
fprintf('Reported time: %g\n', proc_time);
fprintf('\n');


%% READ IMAGE

fprintf('Reading image back from FPGA... '); tic;
B = fread(serialPort, imgSize, 'uint8'); toc;
im_final = reshape(B, [numRows numCols]);

% TODO Uncomment to receive timing values (LSB first)
read_time = double(uint32(fread(serialPort, 1, 'uint32'))) / 100.000e6;
fprintf('Reported time: %g\n', read_time);
%
fclose(serialPort);

%% OUTPUT

figure;
subplot(1,2,1);
imshow(mat2gray(im_orig,[0 255]));
title('Original');
subplot(1,2,2);
imshow(mat2gray(im_final,[0 255]));
title('Filtered');

