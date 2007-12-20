clear all;

TheFiles = dir('Forecast/NonEvents_Cache/*Valley2.field');

AvgData{1} = zeros(1, 0);
AvgData{2} = zeros(1, 0);
AvgData{3} = zeros(1, 0);

for FileIndex = 1:length(TheFiles)
    FieldData = ScanFieldFile(TheFiles(FileIndex).name);
    if (length(FieldData(3, (FieldData(3, :) > 20))) > 0)
        TheFiles(FileIndex).name
    end
%     AvgData{1} = cat(2, AvgData{1}, FieldData(1, :));
%     AvgData{2} = cat(2, AvgData{2}, FieldData(2, :));
    AvgData{3} = cat(2, AvgData{3}, FieldData(3, FieldData(3, :) > -20));
end

% figure;
% hist(AvgData{1}, 18);
% title('Longitude distribution');
% figure;
% hist(AvgData{2}, 7);
% title('Latitude distribution');
figure;
hist(AvgData{3}, OptimalBinCount(AvgData{3}));
title('StdAnom distribution');
disp('Mean');
disp(nanmean(AvgData{3}));
disp('Variance');
disp(nanvar(AvgData{3}));
disp('StdDev');
disp(sqrt(nanvar(AvgData{3})));
