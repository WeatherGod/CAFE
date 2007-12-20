function MakeFoldAnalysis_HistPlots(CAFEPath, ScoreRun_Name, DatabaseName, EventNames)

    EventTypeCount = length(EventNames);

    Arrange = [4 3];
    Indexer = 1;
    figure;
    set(gcf, 'Units', 'points', 'Position', [30 45 594 662], 'PaperUnits', 'points', 'PaperPosition', [30 45 594 662]);
    
    for EventIndex = 1:EventTypeCount
        AxesHandle = subplot(Arrange(1), Arrange(2), Indexer);
        hold on AxesHandle;

        xlabel(AxesHandle, [strrep(EventNames{EventIndex}, '_', ' '), ' Event Score'], 'FontUnits', 'points', 'FontSize', 9.5);
        
        if (mod(Indexer, Arrange(2)) == 1)
            ylabel(AxesHandle, 'Relative Abundance', 'FontUnits', 'points', 'FontSize', 9.5);
        end
        
        set(AxesHandle, 'YTickLabel', '', 'FontUnits', 'points', 'FontSize', 7.5);
        
        MakeEventScore_HistPlot(CAFEPath, ScoreRun_Name, DatabaseName, EventNames{EventIndex}, AxesHandle);

        Indexer = Indexer + 1;
    end
%    saveas(gcf, [CAFEPath, 'CorrelationCalcs/', ScoreRun_Name, '/', DatabaseName, '/EventScores_Hist.eps']);
end

