function TheQuartileValue = IQV(Quartile, TheValues)
    TempHold = TheValues(~isnan(TheValues));
    ValueLength = length(TempHold);
    
    if (ValueLength < 4)
        TheQuartileValue = NaN;
    else
        if (Quartile > 0 && Quartile < 4)
            TempHold = sort(TempHold);
            
            ElementNum = floor((Quartile/4.0)*(ValueLength + 1.0));
            DecimalPart = rem(Quartile*(ValueLength + 1.0), 4.0)/4.0;
            TheQuartileValue = TempHold(ElementNum) + (DecimalPart*(TempHold(ElementNum + 1) - TempHold(ElementNum)));
        else
            TheQuartileValue = NaN;
        end
    end
end
        
        
    
    