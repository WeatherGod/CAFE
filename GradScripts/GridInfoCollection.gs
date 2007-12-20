function main(args)

Filename = subwrd(args, 1)
Lat1 = subwrd(args, 2)
Lat2 = subwrd(args, 3)
Lon1 = subwrd(args, 4)
Lon2 = subwrd(args, 5)


'open 'Filename

'q file 1'
if (subwrd(result, 3) != ":")
   say '---^^^---OpenFailed'
   say '---^^^---AllDone'
   'quit'
endif


'set lat 'Lat1' 'Lat2
'set lon 'Lon1' 'Lon2

GridInfo = GridBoundaries()
X1 = subwrd(GridInfo, 1)
X2 = subwrd(GridInfo, 2)
Y1 = subwrd(GridInfo, 3)
Y2 = subwrd(GridInfo, 4)

say '---^^^---GridData'
say GridInfo

'define Values = hgt(lev=500, time=12Z15MAR1998)'
'd Values'

say '---^^^---LowerDegreeRange'
'q gr2w 'X1' 'Y1
say result

say '---^^^---UpperDegreeRange'
'q gr2w 'X2' 'Y2
say result

say '---^^^---AllDone'

close

'quit'




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
   X1 = math_int(X1) + offset + 1
   X2 = math_int(X2) - 1
                                                                                                                                                             
   Y1 = subwrd(YLine, 11)
   Y2 = subwrd(YLine, 13)
   offset = 0
   if (math_fmod(Y1, 1) != 0)
      offset = 1
   endif
   Y1 = math_int(Y1) + offset + 1
   Y2 = math_int(Y2) - 1
                                                                                                                                                             
return(X1' 'X2' 'Y1' 'Y2)

