function EventIndices = FindEventIndices(CAFEPath, ScoreRun_Name, EventNames, EventListName)

    FileID = fopen([CAFEPath, '/CorrelationCalcs/', ScoreRun_Name, '/', EventListName]);
    if (FileID <= 0)
        disp 'Could not open event name list file for reading'
        EventIndices = [];
        return;
    end

    EventNameList = textscan(FileID, '%s', 'delimiter', ',');
    EventNameList = EventNameList{1};
    fclose(FileID);

    for NameIndex = 1:length(EventNames)
        Tempy = strmatch(EventNames{NameIndex}, EventNameList);

        if (length(Tempy) == 0)
            disp 'The event could not be found in the event list file';
            EventIndices = [];
            return
        end
        
        EventIndices(NameIndex) = Tempy(1);
    end
end