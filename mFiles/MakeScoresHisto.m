function [FreqCount, XBinCents, BinWidth] = MakeScoresHisto(CAFEPath, ScoreRun_Name, DatabaseName, EventName, EventIndices)
    
    Event_Scores = load('-ascii', [CAFEPath, '/CorrelationCalcs/', ScoreRun_Name, '/', DatabaseName, '/', EventName, '_EventScore.csv']);
    Event_Scores = Event_Scores(:, EventIndices);     % removes date information, and other events' scores
    
    for EventIndex = 1:length(EventIndices)
        if (max(Event_Scores(:, EventIndex)) > 0)
            BinCount = OptimalBinCount(Event_Scores(:, EventIndex));
            BinWidth(EventIndex) = (max(Event_Scores(:, EventIndex)) - min(Event_Scores(:, EventIndex))) / BinCount;
            [FreqCount{EventIndex}, XBinCents{EventIndex}] = hist(Event_Scores(:, EventIndex), BinCount);
        else
            BinWidth(EventIndex) = NaN;
            FreqCount{EventIndex} = [];
            XBinCents{EventIndex} = [];
        end
    end
    
end