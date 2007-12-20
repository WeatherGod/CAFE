Lat1 = 32
Lat2 = 43
Lon1 = -93
Lon2 = -84

'open reanalysis/climo/uwnd_climo.ctl'

'set lat 'Lat1' 'Lat2
'set lon 'Lon1' 'Lon2


'define Climo = mean(lev=100,time=00Z01JAN1900)'
'define StdAnom = stddev(lev=100,time=00Z01JAN1900)'

say 'TheMean...'
PrintOutArray('Climo')

say 'TheStdDev...'
PrintOutArray('StdAnom')

quit;






function GetValue(arg1, arg2, arg3)
   'q defval 'arg1' 'arg2' 'arg3
return(subwrd(result,3))


function GridBoundaries()
   'q dims'
   XLine = sublin(result, 2)
   YLine = sublin(result, 3)


   X1 = subwrd(XLine, 11)
   X2 = subwrd(XLine, 13)

   offset = 0
   if (math_fmod(X1, 1) != 0)
      offset = 1
   endif
   X1 = math_int(X1) + offset
   X2 = math_int(X2)

   Y1 = subwrd(YLine, 11)
   Y2 = subwrd(YLine, 13)
   offset = 0
   if (math_fmod(Y1, 1) != 0)
      offset = 1
   endif
   Y1 = math_int(Y1) + offset
   Y2 = math_int(Y2)

return(X1' 'X2' 'Y1' 'Y2)


function PrintOutArray(VarName)
   GridInfo = GridBoundaries()
   XBound1 = subwrd(GridInfo, 1)
   XBound2 = subwrd(GridInfo, 2)
   YBound1 = subwrd(GridInfo, 3)
   YBound2 = subwrd(GridInfo, 4)
   say 'GridBoundaries 'GridInfo
   Yindex = YBound1

   while (Yindex <= YBound2)
      Xindex = XBound1
      RowString = Yindex%'  '
      while (Xindex <= XBound2)
         RowString = RowString' 'GetValue(VarName, Xindex, Yindex)
         Xindex = Xindex + 1
      endwhile
      say RowString
      Yindex = Yindex + 1
   endwhile
return('')

