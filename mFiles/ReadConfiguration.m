function ConfigStruct = ReadConfiguration(TableConfigFile)


fid = fopen(TableConfigFile);

DatabaseStem = '';
ProcessedStem = '';
TimePeriods = {};
TheDomainLons = [0.0 0.0];
TheDomainLats = [0.0 0.0];
PeaksValleys = {'Peak1' 'Peak2' 'Valley1' 'Valley2'};

EventTypeCount = 0;
TimePeriodCount = 0;


EventTypeNames = {};
EventTypes_Vars = {};

if (fid > 0)
    ReadLine = ReadNoComment(fid);
    
    while (FoundStartTag(ReadLine, 'Config') == 0 && feof(fid) == 0)
        ReadLine = ReadNoComment(fid);
    end
    
    if (feof(fid) ~= 0)
        disp('Gone through the file without finding <Config> tag\n\n')
    else
        ReadLine = ReadNoComment(fid);
    
        while (feof(fid) == 0)
            
            if (FoundStartTag(ReadLine, 'DatabaseStem') == 1)
                
                DatabaseStem = StripTags(ReadLine, 'DatabaseStem');
%		disp(DatabaseStem);
                
            elseif (FoundStartTag(ReadLine, 'ProcessedStem') == 1)
                
                ProcessedStem = StripTags(ReadLine, 'ProcessedStem');
%		disp(ProcessedStem);
                
            elseif (FoundStartTag(ReadLine, 'TimePeriods') == 1)
                
                TempHold = StripTags(ReadLine, 'TimePeriods');
                TimePeriodCount = TimePeriodCount + 1;
                Remainder = '';
                [TimePeriods{TimePeriodCount}, Remainder] = strtok(TempHold, ',');
                
                while (~isempty(Remainder))
                    TimePeriodCount = TimePeriodCount + 1;
                    [TimePeriods{TimePeriodCount}, Remainder] = strtok(Remainder, ',');
                end
%		disp(TimePeriods);
            
            elseif (FoundStartTag(ReadLine, 'Domain') == 1)
                
                LonFound = 0;
                LatFound = 0;
                
                while ((LonFound == 0 || LatFound == 0) && (feof(fid) == 0))
                    ReadLine = ReadNoComment(fid);
                    if (FoundStartTag(ReadLine, 'Lon') == 1)
                        TempHold = StripTags(ReadLine, 'Lon');
                        [FirstOne, Remainder] = strtok(TempHold, ',');
                        TheDomainLons(1) = str2num(FirstOne);
                        TheDomainLons(2) = str2num(strtok(Remainder));
                        LonFound = 1;
                    elseif (FoundStartTag(ReadLine, 'Lat') == 1)
                        TempHold = StripTags(ReadLine, 'Lat');
                        [FirstOne, Remainder] = strtok(TempHold, ',');
                        TheDomainLats(1) = str2num(FirstOne);
                        TheDomainLats(2) = str2num(strtok(Remainder));
                        LatFound = 1;
                    end
                end
              
            elseif (FoundStartTag(ReadLine, 'EventType') == 1)

                EventTypeCount = EventTypeCount + 1;
                EventTypes_Vars{EventTypeCount, 1} = {};
                EventTypes_Vars{EventTypeCount, 2} = {};
                VariableCount = 0;
                ReadLine = ReadNoComment(fid);                            
                while ((FoundEndTag(ReadLine, 'EventType') ~= 1) && (feof(fid) == 0))
                    
                    if (FoundStartTag(ReadLine, 'TypeName') == 1)
                        EventTypeNames{EventTypeCount} = StripTags(ReadLine, 'TypeName');
                    elseif (FoundStartTag(ReadLine, 'Variable') == 1)
                        VariableCount = VariableCount + 1;
                        EventTypes_Vars{EventTypeCount, 1}{VariableCount} = '';
                        EventTypes_Vars{EventTypeCount, 2}{VariableCount} = {};
                        
                        while (FoundEndTag(ReadLine, 'Variable') ~= 1 && (feof(fid) == 0))
                            ReadLine = ReadNoComment(fid);
                            
                            if (FoundStartTag(ReadLine, 'Name') == 1)
                                EventTypes_Vars{EventTypeCount, 1}{VariableCount} = StripTags(ReadLine, 'Name');
                            elseif (FoundStartTag(ReadLine, 'Levels') == 1)
                                TempHold = StripTags(ReadLine, 'Levels');
                                k = 1;
                                                                
                                if (~isempty(TempHold))
                                    [EventTypes_Vars{EventTypeCount, 2}{VariableCount}{k}, Remainder] = strtok(TempHold, ',');
%                                    EventTypes_Vars(EventTypeCount).Variables(VariableCount).VarLevels{k} = TheLevels{k};
                                    while (~isempty(Remainder))
                                        k = k + 1;
                                        [EventTypes_Vars{EventTypeCount, 2}{VariableCount}{k}, Remainder] = strtok(Remainder, ',');
%                                        EventTypes_Vars(EventTypeCount).Variables(VariableCount).VarLevels{k} = TheLevels{k};
                                    end
                                    
                                    EventTypes_Vars{EventTypeCount, 2}{VariableCount} = sort(EventTypes_Vars{EventTypeCount, 2}{VariableCount});
%                                    EventTypes_Vars(EventTypeCount).Variables(VariableCount).VarLevels = sort(EventTypes_Vars(EventTypeCount.Variables(VariableCount).VarLevels);
%                                else
%                                    EventTypes_Vars(EventTypeCount).Variables(VariableCount).VarLevels = [];
                                end
                            end
                        end
                    end
                    ReadLine = ReadNoComment(fid);
                end
                
            end
            
            ReadLine = ReadNoComment(fid);
        end   %ends while(~FoundEndTag('Config') or not eof) statement
    end % ends if( found 'Config' tag)
    
    fclose(fid);
end % ends if (file is open)

ConfigStruct = struct('DatabaseNames', [''], 'ProcessedNames', [''], 'DomainLons', [0.0 0.0], 'DomainLats', [0.0 0.0], 'EventTypes', ...
                        struct('EventName', [''], 'Fields', ['']));

TimePeriods = sort(TimePeriods);
for TimeIndex = 1:TimePeriodCount
    ConfigStruct.DatabaseNames{TimeIndex} = [DatabaseStem, '_', TimePeriods{TimeIndex}];
    ConfigStruct.ProcessedNames{TimeIndex} = [ProcessedStem, '_', TimePeriods{TimeIndex}];
end

ConfigStruct.DomainLons = TheDomainLons;
ConfigStruct.DomainLats = TheDomainLats;

[EventTypeNames, TypeIndexOrder] = sort(EventTypeNames);

for EventIndex = 1:EventTypeCount
    ConfigStruct.EventTypes(EventIndex).EventName = EventTypeNames{EventIndex};
    Indexer = 0;
    SortedTypeIndex = TypeIndexOrder(1, EventIndex);
    
    [SortedVarNames, VarNameOrder] = sort(EventTypes_Vars{SortedTypeIndex, 1});
    
    for VarIndex = 1:length(VarNameOrder)
        SortedVarIndex = VarNameOrder(VarIndex);
        TheVarName = SortedVarNames{VarIndex};

        if (length(EventTypes_Vars{SortedTypeIndex, 2}{SortedVarIndex}) > 0)
            for LevIndex = 1:length(EventTypes_Vars{SortedTypeIndex, 2}{SortedVarIndex})
                PartFieldName = [TheVarName, '_', EventTypes_Vars{SortedTypeIndex, 2}{SortedVarIndex}{LevIndex}];
                            
                for PeakValIndex = 1:length(PeaksValleys)
                    Indexer = Indexer + 1;
                    ConfigStruct.EventTypes(EventIndex).Fields{Indexer} = [PartFieldName, '_', PeaksValleys{PeakValIndex}];
                end
            end
        else
            for PeakValIndex = 1:length(PeaksValleys)
                Indexer = Indexer + 1;
                ConfigStruct.EventTypes(EventIndex).Fields{Indexer} = [TheVarName, '_', PeaksValleys{PeakValIndex}];
            end
        end
    end
end


end


function Result = FoundEndTag(TheLine, TagWord)
    if (length(strfind(TheLine, ['</', TagWord, '>'])) > 0)
        Result = 1;
    else
        Result = 0;
    end
end


function Result = FoundStartTag(TheLine, TagWord)
    if (length(strfind(TheLine, ['<', TagWord, '>'])) > 0)
        Result = 1;
    else
        Result = 0;
    end
end


function LineRead = ReadNoComment(fid)
    LineRead = '';

    while (strcmp(LineRead, '') && feof(fid) == 0)
        LineRead = fgetl(fid);
        CommentPos = strfind(LineRead, '#');
        if (length(CommentPos) > 0)
            LineRead = LineRead(1:(CommentPos(1) - 1));
        end
    end
end


function LineRead = StripTags(TheLine, TagWord)

    LineRead = '';
    
    if (FoundStartTag(TheLine, TagWord) == 1 && FoundEndTag(TheLine, TagWord) == 1)
        StartTagPos = strfind(TheLine, ['<', TagWord, '>']);
        EndTagPos = strfind(TheLine, ['</', TagWord, '>']);
        
        LineRead = TheLine((StartTagPos + length(TagWord) + 2):(EndTagPos - 1));
        
    end
end
