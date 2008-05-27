function Result = MakeStdAnomHists(CAFEPath, ConfigFilename, EventNames, DoProcessed)
% Makes the histograms for the standard anomaly values of the members.

ConfigInfo = ReadConfiguration([CAFEPath, '/', ConfigFilename]);

RestrictEvents = 0;

if (~isempty(EventNames))
    RestrictEvents = 1;
end

if (DoProcessed == 0)
   TheDatabaseNames = ConfigInfo.DatabaseNames;
else
   TheDatabaseNames = ConfigInfo.ProcessedNames;
end

for DatabaseIndex = 1:length(TheDatabaseNames)
    Database = TheDatabaseNames{DatabaseIndex};
    for TableIndex = 1:length(ConfigInfo.EventTypes)
        TableName = ConfigInfo.EventTypes(TableIndex).EventName;

    	if (RestrictEvents == 1 && isempty(strmatch(TableName, EventNames, 'exact')))
            % The TableName does not match those in EventNames, so move on to the next iteration.
            continue;
        end

        for VarIndex = 1:length(ConfigInfo.EventTypes(TableIndex).Fields)
            FieldName = [TableName, '_', ConfigInfo.EventTypes(TableIndex).Fields{VarIndex}];

%	    disp(FieldName);    
            LonLatAnom = load('-ascii', [CAFEPath, '/AnalysisInfo/', Database, '/', FieldName, '.lonlatanom']);

            if (isempty(LonLatAnom))
%	        disp(length(LonLatAnom));
                continue;
            end
            
            AnomVals = LonLatAnom(:, 3);
	    
%                figure;
%	        disp(AnomVals);
%	        disp(OptimalBinCount(AnomVals));
            set(gcf, 'Units', 'points', 'Position', [0 0 336 277], ...
                     'PaperUnits', 'points', 'PaperPosition', [0 0 336 277]);
            hist(AnomVals, OptimalBinCount(AnomVals));

            saveas(gcf, [CAFEPath, '/AnalysisInfo/', Database, '/', FieldName, '_Hist.jpg']);
        end
    end
end

end
