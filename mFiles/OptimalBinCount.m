function BinCount = OptimalBinCount(Values)
    if (length(Values) > 4)
        BinSize = 2.0*(IQV(3, Values) - IQV(1, Values))*(length(Values))^(-1.0/3.0);
        BinCount = ceil((max(Values) - min(Values))/BinSize);
        
        if (BinCount < 1.0)
            BinCount = 1.0;
        end
    elseif (length(Values) > 1)
        StandDev = std(Values);
        if (StandDev == 0.0 || isnan(StandDev))
            BinCount = 1;
        else
            BinSize = 3.49*StandDev*(length(Values))^(-1.0/3.0);
            BinCount = ceil((max(Values) - min(Values))/BinSize);
            
            if (BinCount < 1.0)
                BinCount = 1.0;
            end
        end
    else
        BinCount = 0;
    end
end
        
