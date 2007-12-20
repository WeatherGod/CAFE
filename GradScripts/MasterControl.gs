function main(args)

Date = subwrd(args,1)
VarCnt = subwrd(args, 2)
say Date
* ArgPoint will be kind of like a pointer to be incremented though the "vector" of arguments
* It will start at index 3.
ArgPoint = 3

AnomName = 'StdAnom'
Lat1 = 28
Lat2 = 52
Lon1 = -105
Lon2 = -55

* --------------------------------------------------------------------------------------------------------------------------------------------------

ClimDate = FixnDate(Date)
GridInfo = ''

VarIndx = 1
while (VarIndx <= VarCnt)
   SpecLev = ''
   VarName = subwrd(args, ArgPoint)
   ArgPoint = ArgPoint + 1
   LevCnt = subwrd(args, ArgPoint)
   ArgPoint = ArgPoint + 1

   i = 1
   while (i <= LevCnt)
      Levels.i = subwrd(args, ArgPoint)
      i = i + 1
      ArgPoint = ArgPoint + 1
   endwhile

*-------------------------------- Multiple Level variables
   if (VarName = 'hgt' | VarName = 'air' | VarName = 'uwnd' | VarName = 'vwnd' | VarName = 'shum' | VarName = 'shear' | VarName = 'rhum')


      'xdfopen reanalysis/'VarName'/'VarName'.ctl'

      if (subwrd(result, 2) = 'Error:')
         say '---^^^---Start'
         say '---^^^---Endreanalysis/'VarName'/'VarName'.ctl'
         'quit'
      endif

      'open reanalysis/climo/'VarName'_climo.ctl'
   
      if (subwrd(result, 2) = 'Error:')
         say '---^^^---Start'
         say '---^^^---Endreanalysis/climo/'VarName'_climo.ctl'
         'quit'
      endif

      'set lat 'Lat1' 'Lat2
      'set lon 'Lon1' 'Lon2

      if (GridInfo = '')
         GridInfo = GridBoundaries()
      endif

      LevIndx = 1
      if (Levels.LevIndx = '10m')
         LevIndx = LevIndx + 1
         SpecLev = '10m'
      endif

      while (LevIndx <= LevCnt)
         SomeBad = 0
         Values = VarName'(lev='Levels.LevIndx',time='Date')'
         Climo = 'mean.2(lev='Levels.LevIndx',time='ClimDate')'
         StdDev = 'stddev.2(lev='Levels.LevIndx',time='ClimDate')'
         'define 'AnomName' = ('Values'-'Climo')/'StdDev

         'd 'AnomName

         if (subwrd(result, 2) = 'Error:')
            say '---^^^---Start'
            say '---^^^---End'
*      To get it to stop looping since I have an error.
            LevIndx = LevCnt
         else
            PeakValStuff(AnomName, GridInfo)
         endif

         LevIndx = LevIndx + 1

         if (Levels.LevIndx = '10m')
            LevIndx = LevIndx + 1
            SpecLev = '10m'
         endif
      endwhile
   endif

*------------------- Single level svariables -----------------------------------------------------
   if (VarName = 'slp' | VarName = 'pr_wtr' | ((VarName = 'vwnd' | VarName = 'uwnd') & SpecLev = '10m'))

      AddOn = ''
      if (VarName = 'pr_wtr')
         AddOn = '.eatm'
      endif

      if (VarName = 'vwnd' | VarName = 'uwnd')
         AddOn = '.10m'
      endif
         
   
      'xdfopen reanalysis/'VarName''AddOn'/'VarName''AddOn'.ctl'

      if (subwrd(result, 2) = 'Error:')
         say '---^^^---Start'
         say '---^^^---Endreanalysis/'VarName''AddOn'/'VarName''AddOn'.ctl'
         'quit'
      endif

      'open reanalysis/climo/'VarName''AddOn'_climo.ctl'
                                                                                                                                               
      if (subwrd(result, 2) = 'Error:')
         say '---^^^---Start'
         say '---^^^---Endreanalysis/climo/'VarName''AddOn'_climo.ctl'
         'quit'
      endif

      'set lat 'Lat1' 'Lat2
      'set lon 'Lon1' 'Lon2

      if (GridInfo = '')
         GridInfo = GridBoundaries()
      endif

      SomeBad = 0
      Values = VarName'(time='Date')'
      Climo = 'mean.2(time='ClimDate')'
      StdDev = 'stddev.2(time='ClimDate')'
      'define 'AnomName' = ('Values'-'Climo')/'StdDev

      'd 'AnomName

      if (subwrd(result, 2) = 'Error:')
         say '---^^^---Start'
         say '---^^^---End'
      else
         PeakValStuff(AnomName, GridInfo)
      endif
   endif

*---------------------------------------------- Special variables
   if (VarName = 'thk')
      'xdfopen reanalysis/hgt/hgt.ctl'

      if (subwrd(result, 2) = 'Error:')
         say '---^^^---Start'
         say '---^^^---Endreanalysis/hgt/hgt.ctl'
         'quit'
      endif

      'open reanalysis/climo/thk_climo.ctl'
                                                                                                                                               
      if (subwrd(result, 2) = 'Error:')
         say '---^^^---Start'
         say '---^^^---Endreanalysis/climo/thk_climo.ctl'
         'quit'
      endif

      'set lat 'Lat1' 'Lat2
      'set lon 'Lon1' 'Lon2

      if (GridInfo = '')
         GridInfo = GridBoundaries()
      endif

      LevIndx = 1
      while (LevIndx <= LevCnt)
         SomeBad = 0
         BotIndx = LevIndx
         TopIndx = LevIndx + 1
         ClimIndx = LevIndx + 2
         Thkness = 'hgt(lev='Levels.TopIndx',time='Date') - hgt(lev='Levels.BotIndx',time='Date')'
         Climo = 'mean.2(lev='Levels.ClimIndx',time='ClimDate')'
         StdDev = 'stddev.2(lev='Levels.ClimIndx',time='ClimDate')'
   
         'define 'AnomName' = ('Thkness' -'Climo')/'StdDev

         'd 'AnomName

         if (subwrd(result, 2) = 'Error:')
            say '---^^^---Start'
            say '---^^^---End'
         else
            PeakValStuff(AnomName, GridInfo)
         endif

         LevIndx = LevIndx + 3
      endwhile
   endif

   'close 2'
   'close 1'
   VarIndx = VarIndx + 1
endwhile

'quit'


*################ Functions ######################
function PeakValStuff(AnomName, GridInfo)
*   AnomName = subwrd(ArgList,1)
*   XBound1 = subwrd(ArgList, 2)
*   XBound2 = subwrd(ArgList, 3)
*   YBound1 = subwrd(ArgList, 4)
*   YBound2 = subwrd(ArgList, 5)

*   Peak1 = -999
*   Peak2 = -999
*   Valley1 = 999
*   Valley2 = 999
*   Peak1X = -999
*   Peak2X = -999
*   Peak1Y = -999
*   Peak2Y = -999
*   Valley1X = -999
*   Valley2X = -999
*   Valley1Y = -999
*   Valley2Y = -999
*   Peak1Lat = -999
*   Peak1Lon = -999
*   Peak2Lat = -999
*   Peak2Lon = -999
*   Vlly1Lat = -999
*   Vlly1Lon = -999
*   Vlly2Lat = -999
*   Vlly2Lon = -999

   ExtrInfo = ExtremaInfo(AnomName, GridInfo)
   Peak1 = subwrd(ExtrInfo, 1)
   Peak1X = subwrd(ExtrInfo, 2)
   Peak1Y = subwrd(ExtrInfo, 3)
   Peak2 = subwrd(ExtrInfo, 4)
   Peak2X = subwrd(ExtrInfo, 5)
   Peak2Y = subwrd(ExtrInfo, 6)

   Valley1 = subwrd(ExtrInfo, 7)
   Valley1X = subwrd(ExtrInfo, 8)
   Valley1Y = subwrd(ExtrInfo, 9)
   Valley2 = subwrd(ExtrInfo, 10)
   Valley2X = subwrd(ExtrInfo, 11)
   Valley2Y = subwrd(ExtrInfo, 12)

   if (Peak1 = -999)
      Peak1Lon = -999
      Peak1Lat = -999
      Peak2 = -999
      Peak2Lon = -999
      Peak2Lat = -999
   else
      'q gr2w 'Peak1X' 'Peak1Y
      Peak1Lon = subwrd(result, 3)
      Peak1Lat = subwrd(result, 6)

      if (Peak2 != -999)
         'q gr2w 'Peak2X' 'Peak2Y
         Peak2Lon = subwrd(result, 3)
         Peak2Lat = subwrd(result, 6)
      else
         Peak2Lon = -999
         Peak2Lat = -999
      endif
   endif

   if (Valley1 = 999)
      Vlly1Lon = -999
      Vlly1Lat = -999
      Valley2 = 999
      Vlly2Lon = -999
      Vlly2Lat = -999
   else
      'q gr2w 'Valley1X' 'Valley1Y
      Vlly1Lon = subwrd(result, 3)
      Vlly1Lat = subwrd(result, 6)
 
      if (Valley2 != 999)
         'q gr2w 'Valley2X' 'Valley2Y
         Vlly2Lon = subwrd(result, 3)
         Vlly2Lat = subwrd(result, 6)
      else
         Vlly2Lon = -999
         Vlly2Lat = -999
      endif
   endif

   say '---^^^---Start'
   say Peak1' 'Peak1Lon' 'Peak1Lat
   say Peak2' 'Peak2Lon' 'Peak2Lat
   say Valley1' 'Vlly1Lon' 'Vlly1Lat
   say Valley2' 'Vlly2Lon' 'Vlly2Lat
   say '---^^^---End'

return('')

*---------------------------------------------------------------------------------------------------------------------------

function FixnDate(datestr)
*                Date is in the format %HZ%d%b%Y

   climhr=substr(datestr,1,3)
   climday=substr(datestr,4,2)
   climmon=substr(datestr,6,3)
   climyr = '1900'                                                                                                                                    
   if (climday = 29 & climmon = 'FEB')
      climday = 28
   endif

   if (climday = 29 & climmon = 'Feb')
      climday = 28
      climmon = 'FEB'
   endif
                                                                                                                                              
return(climhr%climday%climmon%climyr)
*                                           concatenation being returned ##

*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

function GetValue(arg1, arg2, arg3)
   'q defval 'arg1' 'arg2' 'arg3
return(subwrd(result,3))

*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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


*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

function PrintOutArray(VarName)
   GridInfo = GridBoundaries()
   XBound1 = subwrd(GridInfo, 1)
   XBound2 = subwrd(GridInfo, 2)
   YBound1 = subwrd(GridInfo, 3)
   YBound2 = subwrd(GridInfo, 4)
   say 'Grid Boundaries: 'GridInfo
   Xindex = XBound1
   
   while (Xindex <= XBound2)
      Yindex = YBound1
      RowString = Xindex%'  '
      while (Yindex <= YBound2)
         RowString = RowString' 'GetValue(VarName, Xindex, Yindex)
         Yindex = Yindex + 1
      endwhile
      say RowString
      Xindex = Xindex + 1
   endwhile
return('')
*--------------------------------------------------------------------------------------
* Returns the pre-set undefined value for the ctl file
function GetUVal()
   'q ctlinfo'
   TheLine = sublin(result,3)
return(subwrd(TheLine,2))

*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
function ExtremaInfo(VarName, GridInfo)
*   VarName = subwrd(ArgInfo, 1)
   XBound1 = subwrd(GridInfo, 1)
   XBound2 = subwrd(GridInfo, 2)
   YBound1 = subwrd(GridInfo, 3)
   YBound2 = subwrd(GridInfo, 4)

   UVal = GetUVal()

   Xindex = XBound1 + 1

   PeakVal1 = -999
*   PeakX1 = -999
*   PeakY1 = -999
   PeakVal2 = -999
*   PeakX2 = -999
*   PeakY2 = -999

   VallVal1 = 999
*   VallX1 = -999
*   VallY1 = -999
   VallVal2 = 999
*   VallX2 = -999
*   VallY2 = -999

   while (Xindex < XBound2)
      Yindex = YBound1 + 1

      while (Yindex < YBound2)
         CentVal = GetValue(VarName, Xindex, Yindex)

         if (CentVal != UVal)
            UpVal = GetValue(VarName, Xindex, Yindex + 1)
            DownVal = GetValue(VarName, Xindex, Yindex - 1)
            LeftVal = GetValue(VarName, Xindex - 1, Yindex)
            RightVal = GetValue(VarName, Xindex + 1, Yindex)
            UpRiVal = GetValue(VarName, Xindex + 1, Yindex + 1)
            UpLeVal = GetValue(VarName, Xindex - 1, Yindex + 1)
            DwnRiVal = GetValue(VarName, Xindex + 1, Yindex - 1)
            DwnLeVal = GetValue(VarName, Xindex - 1, Yindex - 1)

            if ((CentVal >= UpVal) & (CentVal >= DownVal) & (CentVal >= LeftVal) & (CentVal >= RightVal) & (CentVal >= UpRiVal) & (CentVal >= DwnLeVal) & (CentVal >= DwnRiVal) & (CentVal >= UpLeVal))
*                In other words, if the CentVal is greater than or equal to all values that neighbor it, then it is a peak
               if (CentVal > PeakVal2)
                  if (CentVal > PeakVal1)
                     PeakVal2 = PeakVal1
                     PeakX2 = PeakX1
                     PeakY2 = PeakY1
                     PeakVal1 = CentVal
                     PeakX1 = Xindex
                     PeakY1 = Yindex
                  else
                     PeakVal2 = CentVal
                     PeakX2 = Xindex
                     PeakY2 = Yindex
                  endif
               endif
            endif

            if ((CentVal <= UpVal) & (CentVal <= DownVal) & (CentVal <= LeftVal) & (CentVal <= RightVal) & (CentVal <= UpRiVal) & (CentVal <= DwnLeVal) & (CentVal <= DwnRiVal) & (CentVal <= UpLeVal))
*                   In other words, if the CentVal is less than or equal to all values that neighbor it, then it is a valley
               if (CentVal < VallVal2)
                  if (CentVal < VallVal1)
                     VallVal2 = VallVal1
                     VallX2 = VallX1
                     VallY2 = VallY1
                     VallVal1 = CentVal
                     VallX1 = Xindex
                     VallY1 = Yindex
                  else
                     VallVal2 = CentVal
                     VallX2 = Xindex
                     VallY2 = Yindex
                  endif
               endif
            endif
         endif

         Yindex = Yindex + 1
      endwhile
      Xindex = Xindex + 1
   endwhile

return(PeakVal1' 'PeakX1' 'PeakY1' 'PeakVal2' 'PeakX2' 'PeakY2' 'VallVal1' 'VallX1' 'VallY1' 'VallVal2' 'VallX2' 'VallY2)
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
