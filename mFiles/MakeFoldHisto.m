function [FreqCount, XBinCents, BinWidth] = MakeFoldHisto(CAFEPath, ScoreRun_Name, DatabaseName, EventName, EventIndices, FoldCount)

    EventScores = zeros(0);
    BaseFileName = [CAFEPath, '/CorrelationCalcs/', ScoreRun_Name, '/', DatabaseName, '/'];
    for FoldIndex = 1:FoldCount
        Tempy = load('-ascii', [BaseFileName, EventName, '_EventScore_Fold_', num2str(FoldIndex), '.csv']);
        
        EventScores = cat(1, EventScores, Tempy(:, EventIndices));
    end
    
    clear Tempy;    % don't need it any more, so let it "go out of scope"
        
    for EventIndex = 1:length(EventIndices)
        if (max(EventScores(:, EventIndex)) > 0)
            BinCount = OptimalBinCount(EventScores(:, EventIndex));
            BinWidth(EventIndex) = (max(EventScores(:, EventIndex)) - min(EventScores(:, EventIndex))) / BinCount;
            [FreqCount{EventIndex}, XBinCents{EventIndex}] = hist(EventScores(:, EventIndex), BinCount);
        else
            BinWidth(EventIndex) = NaN;
            FreqCount{EventIndex} = [];
            XBinCents{EventIndex} = [];
        end
    end
    
end