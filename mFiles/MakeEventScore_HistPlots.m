function MakeEventScore_HistPlot(CAFEPath, ScoreRun_Name, DatabaseName, EventName, AxesHandle)

    EventIndex = FindEventIndices(CAFEPath, ScoreRun_Name, DatabaseName, {EventName}, 'Non_Event_EventNameList.txt');
    
    if (length(EventIndex) == 0)
        disp 'Problem making event score plot';
        return;
    end
    
    [Tempy1, Tempy2, BinWidths(1)] = MakeScoresHisto(CAFEPath, ScoreRun_Name, DatabaseName, 'Non_Event', EventIndex);
    FreqCounts{1} = Tempy1{1, 1};
    XBinCents{1} = Tempy2{1, 1};
    
    
    EventIndex = FindEventIndices(CAFEPath, ScoreRun_Name, DatabaseName, {EventName}, [EventName, '_EventNameList.txt']);
    
    if (length(EventIndex) == 0)
        disp 'Problem making event score plot';
        return;
    end
    
    [Tempy1, Tempy2, BinWidths(2)] = MakeScoresHisto(CAFEPath, ScoreRun_Name, DatabaseName, EventName, EventIndex);
    FreqCounts{2} = Tempy1{1, 1};
    XBinCents{2} = Tempy2{1, 1};
    
    
    EventIndex = FindEventIndices(CAFEPath, ScoreRun_Name, DatabaseName, {EventName}, ['UNTRAINED.', EventName, '_EventNameList.txt']);
    
    if (length(EventIndex) == 0)
        disp 'Problem making event score plot';
        return;
    end
    
    [Tempy1, Tempy2, BinWidths(3)] = MakeFoldHisto(CAFEPath, ScoreRun_Name, DatabaseName, EventName, EventIndex, 10);
    FreqCounts{3} = Tempy1{1, 1};
    XBinCents{3} = Tempy2{1, 1};
    
    
    MaxX = max([max(XBinCents{1}) + BinWidths(1);...
                max(XBinCents{2}) + BinWidths(2);...
                max(XBinCents{3}) + BinWidths(3);...
               ]);
           
    set(AxesHandle, 'XLim', [0 MaxX]);
    
    for Index = 1:3
        TotalArea(Index) = nansum(FreqCounts{Index}) .* BinWidths(Index);
    end
        
    MinArea = min(TotalArea);
    
    LineSpecs = {'-k', '--k', ':k'};
    LineWidths = [0.5 0.5 1.0];
    for Index = 1:3
        ScaleFactor = MinArea ./ TotalArea(Index);
        plot(AxesHandle, [min(XBinCents{Index}) - BinWidths(Index), XBinCents{Index}, max(XBinCents{Index}) + BinWidths(Index)],...
                 [0, ScaleFactor .* (FreqCounts{Index}), 0], LineSpecs{Index}, 'LineWidth', LineWidths(Index));
    end
end
