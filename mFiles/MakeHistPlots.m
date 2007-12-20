function MakeHistPlots(CAFEPath, ScoreRun_Name, DatabaseName, EventNames)

EventTypeCount = length(EventNames);
FinalEventScores = zeros(0);
UntrainedEventScores = zeros(0);
MaxScores = zeros(EventTypeCount,1);
ScaleFactor = 1.0;

% the event scores in files from the same ScoreRun directory should be in the same order.
NonEvent_Scores = load('-ascii', [CAFEPath, 'CorrelationCalcs/', ScoreRun_Name, '/', DatabaseName, '/Non_Event_EventScore.csv']);
NonEvent_Scores = NonEvent_Scores(:, 1:EventTypeCount);

for EventIndex = 1:EventTypeCount
    FinalEventScores{EventIndex} = zeros(0);    % just initializing...
%    UntrainedEventScores{EventIndex} = zeros(0);
%    for FoldIndex = 1:10
        BaseFileName = [CAFEPath, 'CorrelationCalcs/', ScoreRun_Name, '/', DatabaseName, '/'];
        Tempy = load('-ascii', [BaseFileName, EventNames{EventIndex}, '_EventScore.csv']);
        
        %eliminates the date information in the eventscore file...
        FinalEventScores{EventIndex} = Tempy(:, 1); %cat(1, FinalEventScores{EventIndex}, Tempy(:, 1));
        
        %Tempy = load('-ascii', [BaseFileName, 'UNTRAINED.', EventNames{EventIndex}, '_EventScore_Case_', num2str(FoldIndex), '.csv']);
        
        %eliminates the date information in the eventscore file...
        %UntrainedEventScores{EventIndex} = cat(1, UntrainedEventScores{EventIndex}, Tempy(:, 1));
%    end
    MaxScores(EventIndex) = max([max(FinalEventScores{EventIndex});... %max(UntrainedEventScores{EventIndex});
				 max(NonEvent_Scores(:,EventIndex))]);
end

OverallMax = max(MaxScores);

clear Tempy;    % don't need it any more, so let it "go out of scope"
% at this point, all the cases' event scores have been loaded into
% FinalEventScores.

for EventIndex = 1:EventTypeCount
    if (MaxScores(EventIndex) > 0)
        figure;
        set(gcf, 'Name', ['EventScore distribution for ', EventNames{EventIndex}, ' events']);
        hold on;
        
        BinCount = OptimalBinCount(FinalEventScores{EventIndex});
        BinWidth(1) = (max(FinalEventScores{EventIndex}) - min(FinalEventScores{EventIndex})) / BinCount;
        [FreqCounts, XBinCents] = hist(FinalEventScores{EventIndex}, BinCount);
        
        BinWidth(2) = NaN;
%         BinCount = OptimalBinCount(UntrainedEventScores{EventIndex});
%         BinWidth(2) = (max(UntrainedEventScores{EventIndex}) - min(UntrainedEventScores{EventIndex})) / BinCount;
%         [UntrainedFreqCounts, UntrainedXBinCents] = hist(UntrainedEventScores{EventIndex}, BinCount);

    	BinCount = OptimalBinCount(NonEvent_Scores(:, EventIndex));
        BinWidth(3) = (max(NonEvent_Scores(:, EventIndex)) - min(NonEvent_Scores(:, EventIndex))) / BinCount;
        [NonEventFreqCounts, NonEventXBinCents] = hist(NonEvent_Scores(:, EventIndex), BinCount);
        
        NewBinWidth = min(BinWidth);
%        NewXDomain = 0:NewBinWidth:MaxScores(EventIndex) + 5;
	NewXDomain = XBinCents;
        
%        NewFreqCounts = interp1([min(XBinCents) - BinWidth(1), XBinCents, max(XBinCents) + BinWidth(1)],...
%                                [0, FreqCounts, 0], NewXDomain, 'linear');
	NewFreqCounts = FreqCounts;

%        NewNonEventFreqCounts = interp1(NonEventXBinCents, NonEventFreqCounts, NewXDomain);
%        plot([min(NonEventXBinCents) - BinWidth(3), NonEventXBinCents, max(NonEventXBinCents) + BinWidth(3)],...
%            [0, ScaleFactor .* NonEventFreqCounts, 0], 'g', 'LineWidth', 2);
        ScaleFactor = nansum(NewFreqCounts) / nansum(NonEventFreqCounts);
        fill([min(NonEventXBinCents) - BinWidth(3), NonEventXBinCents, max(NonEventXBinCents) + BinWidth(3)],...
            [0, (ScaleFactor .* NonEventFreqCounts), 0], 'g', 'EdgeColor','g');
        
        Bar1 = bar(NewXDomain, NewFreqCounts, 'FaceColor', 'b', 'EdgeColor', 'b');
        set(Bar1,'BarWidth', 0.6);
%        NewUntrainedFreqCounts = interp1([min(UntrainedXBinCents) - BinWidth(2), UntrainedXBinCents, max(UntrainedXBinCents) + BinWidth(2)],...
%                                         [0, UntrainedFreqCounts, 0], NewXDomain, 'linear');
%         ScaleFactor2 = nansum(NewFreqCounts) / nansum(NewUntrainedFreqCounts);
%         Bar2 = bar(NewXDomain, (ScaleFactor2 .* NewUntrainedFreqCounts), 'FaceColor', 'r', 'EdgeColor', 'r');
%         set(Bar2, 'BarWidth', 0.3);
        
        title(['EventScore distribution for ', EventNames{EventIndex}, ' events']);
        xlabel('EventScore Bins');
        ylabel('Occurances');
        legend('RandomDate', 'Trained');
        set(gca, 'XLim', [0 (max(NewXDomain) + max(BinWidth))], 'YLim', [0 (max([max(NewFreqCounts); %max(ScaleFactor2 .* NewUntrainedFreqCounts);
							               max(ScaleFactor .* NonEventFreqCounts)]))]);
        hold off;

%        saveas(gcf, [CAFEPath, 'CorrelationCalcs/', ScoreRun_Name, '/', DatabaseName, '/', EventNames{EventIndex}, '_EventScore_Hist.jpg']);
        
%         figure;
%         Bar3 = bar(XBinCents, FreqCounts, 'FaceColor', 'b', 'EdgeColor', 'b');
%         set(Bar3,'BarWidth', 0.5);
%         hold on;
%         Bar4 = bar(UntrainedXBinCents, 10.0 .* UntrainedFreqCounts, 'FaceColor', 'r', 'EdgeColor', 'r');
%         set(Bar4, 'BarWidth', 0.25);
%         
%         title(['EventScore distribution for ', EventNames{EventIndex}, ' events']);
%         xlabel('EventScore Bins');
%         ylabel('Occurances');
%         legend('Trained', 'Untrained');
%         set(gca, 'XLim', [0 (MaxScores(EventIndex))], 'YLim', [0 (max([max(FreqCounts); max(10.0 .* UntrainedFreqCounts)]))]);
%         hold off;
    end
end

