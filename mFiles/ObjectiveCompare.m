function Result = ObjectiveCompare(FileDirectory, EventNames)

EventTypeCount = length(EventNames);

for EventIndex = 1:EventTypeCount
    BaseFileNames{EventIndex} = [FileDirectory, EventNames{EventIndex}];
    EventScores{EventIndex} = load('-ascii', [BaseFileNames{EventIndex}, '_EventScore.csv']);

    EventScores{EventIndex} = EventScores{EventIndex}(:, 1:EventTypeCount);
end

EventNames{7} = 'Cold';
EventNames{8} = 'Heat';

NonEventScores = load('-ascii', [FileDirectory, 'Non_Event_EventScore.csv']);
NonEventScores = NonEventScores(:, 1:EventTypeCount);

for EventIndex = 1:EventTypeCount
    BoxPlotData{EventIndex} = EventScores{EventIndex}(:, EventIndex);
    MaxVals(1, EventIndex) = max(EventScores{EventIndex}(:,EventIndex));
    NonEventBoxPlot{EventIndex} = NonEventScores(:, EventIndex);
    MaxVals(2, EventIndex) = max(NonEventScores(:, EventIndex));
    Sizes(EventIndex) = length(BoxPlotData{EventIndex});
end

OverallMax = max(max(MaxVals))
MaxSizes = max(Sizes)


%  This loop evens out the arrays so that boxplot can read them in properly.  The arrays are padded with NaNs
for EventIndex = 1:EventTypeCount
    if (Sizes(EventIndex) ~= MaxSizes)
        BoxPlotData{EventIndex}((Sizes(EventIndex) + 1):MaxSizes) = NaN((MaxSizes - Sizes(EventIndex)), 1);
    end
end

figure;
hold on;

size(BoxPlotData)
size(NonEventBoxPlot)

boxplot([BoxPlotData{:}], 'positions', (1/2):2:(EventTypeCount*2), 'whisker', 1.5);
boxplot([NonEventBoxPlot{:}], 'positions', 1:2:(EventTypeCount*2), 'whisker', 1.5);

hold off;

xlabel('Event Types');
ylabel('EventScore');
set(gca, 'XTickLabel', EventNames);
set(gca, 'LineWidth', 1);
set(gca, 'YLim', [0 (OverallMax + 1)]);

%saveas(gcf, [FileDirectory, 'Objective_Events_BoxPlot.jpg']);







