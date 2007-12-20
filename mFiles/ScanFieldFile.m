function TheField = ScanFieldFile(TheFileName)
    fid = fopen(['Forecast/NonEvents_Cache/', TheFileName], 'r');
    TheField = [];
    
    if (fid == -1)
        disp(['Could not open file, ', TheFileName, ' for reading...']);
        return;
    end
    
    TempArray = textscan(fid, '%n%n%n%*s', 'delimiter', ' ', 'treatAsEmpty', '\N');
    fclose(fid);
    
    TheField(1, 1:length(TempArray{1})) = TempArray{1};
    TheField(2, 1:length(TempArray{2})) = TempArray{2};
    TheField(3, 1:length(TempArray{3})) = TempArray{3};
end