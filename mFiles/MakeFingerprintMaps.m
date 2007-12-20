function MakeFingerprintMaps(CAFEPath, ConfigFilename, EventNames, DoProcessed)

ConfigInfo = ReadConfiguration([CAFEPath, '/', ConfigFilename]);

%landareas = shaperead('world','UseGeoCoords',true);
landareas = shaperead('usa_st', 'UseGeoCoords', true);

% Probably should update this so that the config file can tell the script
% which projection to use.
MapAxisHandle = axesm('lambert','Grid','on','MapParallels',[50 50],'Origin',[-107]);


[x y] = mfwdtran(ConfigInfo.DomainLats, ConfigInfo.DomainLons);


%MaxRadius = 0.012;
%MinRadius = 0.005;

MaxRadius = 5.0;
MinRadius = 3.0;
BaseThick = 0.25;


RestrictEvents = 0;

if (length(EventNames) > 0)
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

    	if (RestrictEvents == 1 & isempty(strmatch(TableName, EventNames, 'exact')))
            % The TableName does not match those in EventNames, so move on to the next iteration.
            continue;
        end
        
        for VarIndex = 1:length(ConfigInfo.EventTypes(TableIndex).Fields)
            FieldName = [TableName, '_', ConfigInfo.EventTypes(TableIndex).Fields{VarIndex}];
            LonLatAnom = load('-ascii', [CAFEPath, '/AnalysisInfo/', Database, '/', FieldName, '.lonlatanom']);

            if (isempty(LonLatAnom))
                continue;
            end
            
            AxisCopy = axesm('lambert','Grid','on','MapParallels',[50 50],'Origin',[-107]);
            
            
            set(AxisCopy,'XLim',[x(1) x(2)],'YLim',[y(1) y(2)]);
            set(gcf, 'CurrentAxes', AxisCopy, 'Units', 'points', 'Position', [0 0 236 177], 'PaperUnits', 'points', 'PaperPosition', [0 0 236 177]);
            
            title(AxisCopy, strrep([Database, '/', FieldName], '_', ' '), 'FontUnits', 'points', 'FontSize', 5.5);
            
            [XPos YPos] = mfwdtran(LonLatAnom(:, 2), LonLatAnom(:, 1));
            [UniqueXY, m, n] = unique([XPos YPos], 'rows');
            XYCounts = zeros(length(UniqueXY), 1);
                
            for XYIndex = 1:length(UniqueXY)
                XYCounts(XYIndex) = sum(XYIndex == n);
            end

            Increm = (MaxRadius - MinRadius) / max(XYCounts);
            
%            plot(UniqueXY(:, 1), UniqueXY(:, 2), 'ok', 'MarkerSize', XYCounts .* Increm + MinRadius);

            for XYIndex = 1:length(UniqueXY)
                CircRadius = MinRadius;
                TheX = UniqueXY(XYIndex, 1);
                TheY = UniqueXY(XYIndex, 2);
                
                plot(TheX, TheY, 'ok', 'MarkerSize', XYCounts(XYIndex) * Increm + MinRadius, 'LineWidth', (XYCounts(XYIndex) - 1) * Increm + BaseThick);
                
                %for CircIndex = 1:XYCounts(XYIndex)
                %    plot(TheX, TheY, 'ok', 'MarkerSize', CircRadius);
                    
                    %rectangle('Position', [UniqueXY(XYIndex, 1) - CircRadius, UniqueXY(XYIndex, 2) - CircRadius,...
                    %          CircRadius * 2.0, CircRadius * 2.0],...
                    %          'Curvature', [1, 1],...
                    %          'LineWidth', Increm * 0.75);
                %    CircRadius = CircRadius + Increm;
                %end
            end
            
            geoshow(landareas, 'FaceColor','white', 'EdgeColor', 'black');
    	    saveas(gcf, [CAFEPath, '/AnalysisInfo/', Database, '/', FieldName, '_MapPlot.jpg']);
            clf;
        end
    end
end

end
