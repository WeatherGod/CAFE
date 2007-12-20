function main(args)

InitSize = 0.15
MaxSize = 0

Name = subwrd(args, 1)




'open grid.ctl'
'set parea 0 11 0 8.5'
'set ylab off'
'set xlab off'
'set grads off'
'set mpdset mres'
'set map 1'
'set mproj scaled'
'set clevs -999'
'set lon -105 -55'
'set lat 28 52'

UVal = GetUVal()

GridInfo = GridBoundaries()
X1 = subwrd(GridInfo, 1)
X2 = subwrd(GridInfo, 2)
Y1 = subwrd(GridInfo, 3)
Y2 = subwrd(GridInfo, 4)

* This is to create the map
'd var'

*  This is to create a blacked out area of the plot to write out text
'set line 0 1 1'
'draw recf 0.5 8.1 10.5 8.48'

*  Declaration
'define BigCnt = var'


* initializing the BigCnt array

* BTW, Xix and Yix is short for Xindex and Yindex.  Unfortunately, when you have an array, there is a maximum limit to the size of the variable to call it
* which is 16 characters before or after substitution of values in place of the variable names.  If this doesn't make sense, blame the creators of GrAD
* for designing the scripting language with this limitation.

Xix = X1
while (Xix <= X2)
   Yix = Y1
   while (Yix <= Y2)
      'set defval BigCnt 'Xix' 'Yix' NULL'
      Yix = Yix + 1
   endwhile
   Xix = Xix + 1
endwhile



LineRead = read(Name)
IOCode = sublin(LineRead, 1)
MarkCnt = 0

while (IOCode != 2 & IOCode != 1 & IOCode != 9 & IOCode != 8)
   Values = sublin(LineRead, 2)
   EventLon = subwrd(Values, 1)
   EventLat = subwrd(Values, 2)

   if (EventLon != '\N' & EventLat != '\N')
      MarkCnt = MarkCnt + 1
      'q w2gr 'EventLon' 'EventLat
      Xix = subwrd(result,3)
      Yix = subwrd(result,6)
      Xix = math_int(Xix)
      Yix = math_int(Yix)

      PrevVal = GetValue('BigCnt',Xix,Yix)

      if (PrevVal = 'NULL')
         'set defval BigCnt 'Xix' 'Yix' 1'
      else
         'set defval BigCnt 'Xix' 'Yix' 'PrevVal + 1
      endif
      
      
   endif

   LineRead = read(Name)
   IOCode = sublin(LineRead, 1)
endwhile



say MarkCnt
'd max(max(BigCnt, X='X1', X='X2'), Y='Y1', Y='Y2')'
MaxLin = sublin(result, Y2 - Y1 + 3)
MaxCnt = subwrd(MaxLin, 4)


'set gxout shaded'
'set cmin -5'
'set cmax 10'
'set cint 1'
'd BigCnt'

say result
          

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

function GetValue(arg1, arg2, arg3)
   'q defval 'arg1' 'arg2' 'arg3
return(subwrd(result,3))

function GetUVal()
* Returns the pre-set undefined value for the ctl file
   'q ctlinfo'
   TheLine = sublin(result,3)
return(subwrd(TheLine,2))

function PrintOutArray(VarName)
   GridInfo = GridBoundaries()
   XBound1 = subwrd(GridInfo, 1)
   XBound2 = subwrd(GridInfo, 2)
   YBound1 = subwrd(GridInfo, 3)
   YBound2 = subwrd(GridInfo, 4)
   say "Grid Boundaries: "GridInfo
   Xindex = XBound1

   while (Xindex <= XBound2)
      Yindex = YBound1
      RowString = Xindex%'  '
      while (Yindex <= YBound2)
         RowString = RowString%' '%GetValue(VarName, Xindex, Yindex)
         Yindex = Yindex + 1
      endwhile
      say RowString
      Xindex = Xindex + 1
   endwhile
return("")


