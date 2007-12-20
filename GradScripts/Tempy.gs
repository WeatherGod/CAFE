* in my cshrc.cat file, I have this line:
* setenv GAUDFT /home/meteo/rgrumm/grads/udf/regrid2/udft.regrid2

function main(args)
                                                                                                                                                             
Tau = subwrd(args, 1)
VarName = subwrd(args, 2)
LevCnt = subwrd(args, 3)

i = 1
while (i <= LevCnt)
   Levels.i = subwrd(args, i+3)
   i = i + 1
endwhile

AnomName = 'StdAnom'
Lat1 = 27.5
Lat2 = 30
ClimLon1 = 255
ClimLon2 = 275

Lon1 = ClimLon1
Lon2 = ClimLon2

* -------------------------------------------------------------------------------------------------------------------------------------------------
* If you set GetAbs to 1, the program will grab the maximum value for Peak1 if no peaks are found, and the smallest value for Valey1 if no valleys are found.
* If you set GetAbs to 0, the program will not obtain the maximum or minimum value if no peaks or valleys are found.
* It is highly suggested that GetAbs is set to 0 in order to preserve the integrity of the data.

*  03/18/2005 - Update Note:  with the development of the overall system, it is still suggested that GetAbs be set to zero when using this program to
*	build up the cluster database.  However, for the operational use of retriving the state of the atmosphere for the purpsoe of scoring its
*	"correlation" to past events, set GetAbs to one.  The program is set so that if nothing exists after the last expected arguement, then it will
*	default to a GetAbs of 0.  If something exist after the last normally expected arguement and it isn't a 1, it will default to 0.  If there is a 
*	one there, then GetAbs is set to 1.

GetAbs = subwrd(args, i+3)

if (GetAbs != '1') 
   GetAbs = 0
endif


ETAVars = 'NULL'
if (VarName = 'hgt')
   ETAVars = 'hgtprs'
endif
if (VarName = 'air')
   ETAVars = 'tmpprs'
endif
if (VarName = 'uwnd')
   ETAVars = 'ugrdprs'
endif
if (VarName = 'vwnd')
   ETAVars = 'vgrdprs'
endif
if (VarName = 'shum')
   ETAVars = 'spfhprs'
endif
if (VarName = 'shear')
   ETAVars = 'shear'
endif
if (VarName = 'thk')
   ETAVars = 'hgtprs'
endif
if (VarName = 'slp')
   ETAVars = 'pressfc'
endif
if (VarName = 'pr_wtr')
   ETAVars = 'pwatclm'
endif
if (VarName = 'uwnd.10m')
   ETAVars = 'ugrd10m'
endif
if (VarName = 'vwnd.10m')
   ETAVars = 'vgrd10m'
endif


* --------------------------------------------------------------------------------------------------------------------------------------------------

*-------------------------------- Multiple Level variables
if (VarName = 'hgt' | VarName = 'air' | VarName = 'uwnd' | VarName = 'vwnd' | VarName = 'shum')
   say 'In multi-level variable if-block'
   'open NAM/ETA.CTL'

   TheLine = sublin(result,2)
   if (subwrd(TheLine, 2) = 'Error:')
      say '---^^^---Start'
      say '---^^^---EndNAM/ETA.CTL'
      'quit'
   endif

   'open reanalysis/climo/'VarName'_climo.ctl'
   
   TheLine = sublin(result,2)
   if (subwrd(TheLine, 2) = 'Error:')
      say '---^^^---Start'
      say '---^^^---Endreanalysis/climo/'VarName'_climo.ctl'
      'quit'
   endif

   Date = getDTG()
   ClimDate = FixnDate(Date)   
   'set lat 'Lat1' 'Lat2
   'set lon 'Lon1' 'Lon2

   
   LevIndx = 1
   while (LevIndx <= LevCnt)
      SomeBad = 0
      Values = ETAVars'(lev='Levels.LevIndx',time='Date')'
      Climo = 'regrid2(mean.2(lon='Lon1-360',lon='Lon2-360',lev='Levels.LevIndx',time='ClimDate'),0.5,0.5,bl_p1,'Lon1','Lat1')'
      StdDev = 'regrid2(stddev.2(lev='Levels.LevIndx',time='ClimDate'),0.5,0.5,bl_p1,'Lon1','Lat1')'
      'define 'AnomName' = ('Values'-'Climo')/'StdDev
      
      'd 'ETAVars'(lev='Levels.LevIndx',time='Date')'
      if (subwrd(result, 1) = 'Constant')
         SomeBad = 1
      endif

      'd 'AnomName


      if (subwrd(result, 2) = 'Error:' | SomeBad = 1)
         say '---^^^---Start'
         say '---^^^---End'
      else
         PeakValStuff(AnomName' 'GetAbs)
      endif

      LevIndx = LevIndx + 1
   endwhile

endif

*------------------- Single level svariables -----------------------------------------------------
if (VarName = 'slp' | VarName = 'pr_wtr' | VarName = 'vwnd.10m' | VarName = 'uwnd.10m')
   say 'In single-level variable if-block'
   AddOn = ''
   if (VarName = 'pr_wtr')
      AddOn = '.eatm'
   else
      AddOn = ''
   endif
   
   'open NAM/ETA.CTL'
   TheLine = sublin(result,2)
   if (subwrd(TheLine, 2) = 'Error:')
      say '---^^^---Start'
      say '---^^^---EndNAM/ETA.CTL'
      'quit'
   endif

   'open reanalysis/climo/'VarName''AddOn'_climo.ctl'
   
   TheLine = sublin(result,2)
   if (subwrd(TheLine, 2) = 'Error:')
      say '---^^^---Start'
      say '---^^^---Endreanalysis/climo/'VarName''AddOn'_climo.ctl'
      'quit'
   endif

   say 'Files opened'

   if (VarName = 'vwnd.10m')
      VarName = 'vwnd'
   endif

   if (VarName = 'uwnd.10m')
      VarName = 'uwnd'
   endif

   Date = getDTG()
   ClimDate = FixnDate(Date)
   'q dims'
   say result
   'set lat 'Lat1' 'Lat2
   'set lon 'Lon1' 'Lon2
   'q dims'
   say result

   SomeBad = 0
   
   Values = ETAVars'(time='Date')'
       
   Climo = 'regrid2(mean.2(time='ClimDate'),0.5,0.5,bl_p1,'Lon1','Lat1')'
*   'define Tempy = mean(time='ClimDate')'
*   'define Tempy = regrid2(mean.2(time='ClimDate'), 0.5,0.5,bl_p1,'Lon1','Lat1')'
*   PrintOutArray('Tempy')
   StdDev = 'regrid2(stddev.2(time='ClimDate'),0.5,0.5,bl_p1,'Lon1','Lat1')'

   

   'define 'AnomName' = ('Values'-'Climo')/'StdDev


   'd 'ETAVars'(time='Date')'
   if (subwrd(result, 1) = 'Constant')
      SomeBad = 1
   endif

   'd 'AnomName

   if (subwrd(result, 2) = 'Error:' | SomeBad = 1)
      say '---^^^---Start'
      say '---^^^---End'
      'quit'
   else
      PeakValStuff(AnomName' 'GetAbs)
   endif

   
endif

*---------------------------------------------- Special variables
if (VarName = 'shear')
   'open NAM/ETA.CTL'

   TheLine = sublin(result,2)

   if (subwrd(TheLine, 2) = 'Error:')
      say '---^^^---Start'
      say '---^^^---EndNAM/ETA.CTL'
      'quit'
   endif

   'open reanalysis/climo/'VarName'_climo.ctl'

   TheLine = subline(result,2)
   if (subwrd(TheLine, 2) = 'Error:')
      say '---^^^---Start'
      say '---^^^---Endreanalysis/climo/'VarName'_climo.ctl'
      'quit'
   endif

   Date = getDTG()
   ClimDate = FixnDate(Date)
   'set lat 'Lat1' 'Lat2
   'set lon 'Lon1' 'Lon2


   LevIndx = 1
   while (LevIndx <= LevCnt)
      SomeBad = 0
      du = 'u'ETAVars'(lev='Levels.LevIndx',time='Date') - u'ETAVars'(z=1,time='Date')'
      dv = 'v'ETAVars'(lev='Levels.LevIndx',time='Date') - v'ETAVars'(z=1,time='Date')'

*--------------- not sure if this dz line is entirely correct
      dz = 'hgtprs(lev='Levels.LevIndx',time='Date') - hgtsfc(z=1,time='Date') - 10'

      Values = 'mag('du','dv')/'dz
      Climo = 'regrid2(mean.2(lev='Levels.LevIndx',time='ClimDate'),0.5,0.5,bl_p1,'Lon1-360','Lat1')'
      StdDev = 'regrid2(stddev.2(lev='Levels.LevIndx',time='ClimDate'),0.5,0.5,bl_p1,'Lon1-360','Lat1')'
      'define 'AnomName' = ('Values'-'Climo')/'StdDev

      'd u'ETAVars'(lev='Levels.LevIndx',time='Date')'
      if (subwrd(result, 1) = 'Constant')
         SomeBad = 1
      endif

      'd v'ETAVars'(lev='Levels.LevIndx',time='Date')'
      if (subwrd(result, 1) = 'Constant')
         SomeBad = 1
      endif

      'd 'AnomName


      if (subwrd(result, 2) = 'Error:' | SomeBad = 1)
         say '---^^^---Start'
         say '---^^^---End'
      else
         PeakValStuff(AnomName' 'GetAbs)
      endif

      LevIndx = LevIndx + 1
   endwhile
endif


if (VarName = 'thk')
   
   LevsClimo = Levels.3
   'open NAM/ETA.CTL'

   TheLine = sublin(result,2)
   if (subwrd(TheLine, 2) = 'Error:')
      say '---^^^---Start'
      say '---^^^---EndNAM/ETA.CTL'
      'quit'
   endif

   'open reanalysis/climo/thk_climo.ctl'
   
   TheLine = sublin(result,2)
   if (subwrd(TheLine, 2) = 'Error:')
      say '---^^^---Start'
      say '---^^^---Endreanalysis/climo/thk_climo.ctl'
      'quit'
   endif

   Date = getDTG()
   ClimDate = FixnDate(Date)
   'set lat 'Lat1' 'Lat2
   'set lon 'Lon1' 'Lon2

   SomeBad = 0
   Thkness = ETAVars'(lev='Levels.2',time='Date') - 'ETAVars'(lev='Levels.1',time='Date')'
   Climo = 'regrid2(mean.2(lev='LevsClimo',time='ClimDate'),0.5,0.5,bl_p1,'Lon1-360','Lat1')'
   StdDev = 'regrid2(stddev.2(lev='LevsClimo',time='ClimDate'),0.5,0.5,bl_p1,'Lon1-360','Lat1')'
   
   'define 'AnomName' = ('Thkness' -'Climo')/'StdDev

   'd 'ETAVars'(lev='Levels.2',time='Date')'
   if (subwrd(result,1) = 'Constant')
      SomeBad = 1
   endif

   'd 'ETAVars'(lev='Levels.1',time='Date')'
   if (subwrd(result,1) = 'Constant')
      SomeBad = 1
   endif

   'd 'AnomName

   if (subwrd(result, 2) = 'Error:' | SomeBad = 1)
      say '---^^^---Start'
      say '---^^^---End'
   else
      PeakValStuff(AnomName' 'GetAbs)
   endif
   
endif

'close 2'
'close 1'

'quit'


*################ Functions ######################
*-------------------------------------------------------------------------------------------
function GetMaxInfo(VrName)
   GridInfo = GridBoundaries()
   XBound1 = subwrd(GridInfo, 1)
   XBound2 = subwrd(GridInfo, 2)
   YBound1 = subwrd(GridInfo, 3)
   YBound2 = subwrd(GridInfo, 4)

   GridLen = 1
   Xindex = XBound1
   MaxVal = -999
   MaxX = -999
   MaxY = -999
   
   while (Xindex <= XBound2)
      Yindex = YBound1
                                                                                                                                                             
      while (Yindex <= YBound2)
         CurrVal = GetValue(VrName, Xindex, Yindex)
         if (CurrVal > MaxVal)
            MaxVal = CurrVal
            MaxX = Xindex
            MaxY = Yindex
         endif
                                                                                                                                                             
         Yindex = Yindex + GridLen
      endwhile
      Xindex = Xindex + GridLen
   endwhile

return(MaxVal' 'MaxX' 'MaxY)

*-------------------------------------------------------------------------------------------
function GetMinInfo(VrName)
   GridInfo = GridBoundaries()
   XBound1 = subwrd(GridInfo, 1)
   XBound2 = subwrd(GridInfo, 2)
   YBound1 = subwrd(GridInfo, 3)
   YBound2 = subwrd(GridInfo, 4)

   say GridInfo
   GridLen = 1
   Xindex = XBound1
   MinVal = 999
   MinX = -999
   MinY = -999
                                                                                                                                                             
   while (Xindex <= XBound2)
      Yindex = YBound1
                                                                                                                                                             
      while (Yindex <= YBound2)
         CurrVal = GetValue(VrName, Xindex, Yindex)
         if (CurrVal < MinVal)
            MinVal = CurrVal
            MinX = Xindex
            MinY = Yindex
         endif
                                                                                                                                                             
         Yindex = Yindex + GridLen
      endwhile
      Xindex = Xindex + GridLen
   endwhile

return(MinVal' 'MinX' 'MinY)

*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

function PeakValStuff(ArgList)

   AnomName = subwrd(ArgList,1)
   GetAbs = subwrd(ArgList,2)
   Peak1 = -999
   Peak2 = -999
   Valley1 = 999
   Valley2 = 999
   Peak1X = -999
   Peak2X = -999
   Peak1Y = -999
   Peak2Y = -999
   Valley1X = -999
   Valley2X = -999
   Valley1Y = -999
   Valley2Y = -999
   Peak1Lat = -999
   Peak1Lon = -999
   Peak2Lat = -999
   Peak2Lon = -999
   Vlly1Lat = -999
   Vlly1Lon = -999
   Vlly2Lat = -999
   Vlly2Lon = -999

   UVal = GetUVal()

   PeakInfo = Top2Peaks(AnomName)
   Peak1 = subwrd(PeakInfo, 1)
   Peak1X = subwrd(PeakInfo, 2)
   Peak1Y = subwrd(PeakInfo, 3)
   Peak2 = subwrd(PeakInfo, 4)
   Peak2X = subwrd(PeakInfo, 5)
   Peak2Y = subwrd(PeakInfo, 6)

   if (Peak1 = UVal)
      Peak1 = -999
   endif

   if (Peak2 = UVal)
      Peak2 = -999
   endif

   if (Peak1 = -999)
      if (GetAbs = 1)
         say 'Getting absolute max value'
         MaxInfo = GetMaxInfo(AnomName)
         Peak1 = subwrd(MaxInfo, 1)
         Peak1X = subwrd(MaxInfo, 2)
         Peak1Y = subwrd(MaxInfo, 3)

         if (Peak1 = UVal)
            Peak1 = -999
            Peak1Lon = -999
            Peak1Lat = -999
         else
            'q gr2w 'Peak1X' 'Peak1Y
            Peak1Lon = subwrd(result, 3)
            Peak1Lat = subwrd(result, 6)
         endif
      else
         Peak1Lon = -999
         Peak1Lat = -999
      endif
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

   ValyInfo = Top2Valleys(AnomName)
   Valley1 = subwrd(ValyInfo, 1)
   Valley1X = subwrd(ValyInfo, 2)
   Valley1Y = subwrd(ValyInfo, 3)
   Valley2 = subwrd(ValyInfo, 4)
   Valley2X = subwrd(ValyInfo, 5)
   Valley2Y = subwrd(ValyInfo, 6)

   if (Valley1 = UVal)
      Valley1 = 999
   endif

   if (Valley2 = UVal)
      Valley2 = 999
   endif

   if (Valley1 = 999)
      if (GetAbs = 1)
         say 'Getting absolute minimum value'
         MinInfo = GetMinInfo(AnomName)
         Valley1 = subwrd(MinInfo, 1)
         Valley1X = subwrd(MinInfo, 2)
         Valley1Y = subwrd(MinInfo, 3)

         if (Valley1 = UVal)
            Valley1 = 999
            Vlly1Lon = -999
            Vlly1Lat = -999
         else
            'q gr2w 'Valley1X' 'Valley1Y
            Vlly1Lon = subwrd(result, 3)
            Vlly1Lat = subwrd(result, 6)
         endif
      else
         Vlly1Lon = -999
         Vlly1Lat = -999
      endif
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
*                           Date is in the ZZzDDmmmYYYY format (capital letters mean numeric, lower case means alphabetic)#####

   climhr=substr(datestr,1,3)
   climday=substr(datestr,4,2)
   climmon=substr(datestr,6,3)
   climyr = "1900"                                                                                                                                    
   if (climday = 29 & climmon = 'FEB')
      climday = 28
   endif

   if (climday = 29 & climon = 'Feb')
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
return('')
*--------------------------------------------------------------------------------------

function GetUVal()
* Returns the pre-set undefined value for the ctl file
   'q ctlinfo'
   TheLine = sublin(result,3)
return(subwrd(TheLine,2))

*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

function Top2Peaks(VarName)
   GridInfo = GridBoundaries()
   XBound1 = subwrd(GridInfo, 1)
   XBound2 = subwrd(GridInfo, 2)
   YBound1 = subwrd(GridInfo, 3)
   YBound2 = subwrd(GridInfo, 4)

   Xindex = XBound1 + 1
   PeakVal.1 = -999
   PeakX.1 = -999
   PeakY.1 = -999
   PeakVal.2 = -999
   PeakX.2 = -999
   PeakY.2 = -999

   while (Xindex < XBound2)
      Yindex = YBound1 + 1

      while (Yindex < YBound2)
         CentVal = GetValue(VarName, Xindex, Yindex)
         UpVal = GetValue(VarName, Xindex, Yindex + 1)
         DownVal = GetValue(VarName, Xindex, Yindex - 1)
         LeftVal = GetValue(VarName, Xindex - 1, Yindex)
         RightVal = GetValue(VarName, Xindex + 1, Yindex)
         UpRiVal = GetValue(VarName, Xindex + 1, Yindex + 1)
         UpLeVal = GetValue(VarName, Xindex - 1, Yindex + 1)
         DwnRiVal = GetValue(VarName, Xindex + 1, Yindex - 1)
         DwnLeVal = GetValue(VarName, Xindex - 1, Yindex - 1)

         if ((CentVal >= UpVal) & (CentVal >= DownVal) & (CentVal >= LeftVal) & (CentVal >= RightVal) & (CentVal >= UpRiVal) & (CentVal >= UpLeVal) & (CentVal >= DwnRiVal) & (CentVal >= DwnLeVal))
*                In other words, if the CentVal is greater than or equal to all values that neighbors it, then it is true
            if (CentVal > PeakVal.2)
               if (CentVal > PeakVal.1)
                  PeakVal.2 = PeakVal.1
                  PeakX.2 = PeakX.1
                  PeakY.2 = PeakY.1
                  PeakVal.1 = CentVal
                  PeakX.1 = Xindex
                  PeakY.1 = Yindex
               else
                  PeakVal.2 = CentVal
                  PeakX.2 = Xindex
                  PeakY.2 = Yindex
               endif
            endif
         endif

         Yindex = Yindex + 1
      endwhile
      Xindex = Xindex + 1
   endwhile


return(PeakVal.1' 'PeakX.1' 'PeakY.1' 'PeakVal.2' 'PeakX.2' 'PeakY.2)

*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

function Top2Valleys(VarName)
   GridInfo = GridBoundaries()
   XBound1 = subwrd(GridInfo, 1)
   XBound2 = subwrd(GridInfo, 2)
   YBound1 = subwrd(GridInfo, 3)
   YBound2 = subwrd(GridInfo, 4)

   Xindex = XBound1 + 1
   PeakVal.1 = 999
   PeakX.1 = -999
   PeakY.1 = -999
   PeakVal.2 = 999
   PeakX.2 = -999
   PeakY.2 = -999
                                                                                                                                                                                    
   while (Xindex < XBound2)
      Yindex = YBound1 + 1
                                                                                                                                                                                    
      while (Yindex < YBound2)
         CentVal = GetValue(VarName, Xindex, Yindex)
         UpVal = GetValue(VarName, Xindex, Yindex + 1)
         DownVal = GetValue(VarName, Xindex, Yindex - 1)
         LeftVal = GetValue(VarName, Xindex - 1, Yindex)
         RightVal = GetValue(VarName, Xindex + 1, Yindex)
         UpRiVal = GetValue(VarName, Xindex + 1, Yindex + 1)
         UpLeVal = GetValue(VarName, Xindex - 1, Yindex + 1)
         DwnRiVal = GetValue(VarName, Xindex + 1, Yindex - 1)
         DwnLeVal = GetValue(VarName, Xindex - 1, Yindex - 1)
                                                                                                                                                                                    
         if ((CentVal <= UpVal) & (CentVal <= DownVal) & (CentVal <= LeftVal) & (CentVal <= RightVal) & (CentVal <= UpRiVal) & (CentVal <= UpLeVal) & (CentVal <= DwnRiVal) & (CentVal <= DwnLeVal))
*                In other words, if the CentVal is less than or equal to all values that neighbors it, then it is true
            if (CentVal < PeakVal.2)
               if (CentVal < PeakVal.1)
                  PeakVal.2 = PeakVal.1
                  PeakX.2 = PeakX.1
                  PeakY.2 = PeakY.1
                  PeakVal.1 = CentVal
                  PeakX.1 = Xindex
                  PeakY.1 = Yindex
               else
                  PeakVal.2 = CentVal
                  PeakX.2 = Xindex
                  PeakY.2 = Yindex
               endif
            endif
         endif
                                                                                                                                                                                    
         Yindex = Yindex + 1
      endwhile
      Xindex = Xindex + 1
   endwhile

return(PeakVal.1' 'PeakX.1' 'PeakY.1' 'PeakVal.2' 'PeakX.2' 'PeakY.2)

*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
function getDTG()
   'q time'
   DTG = subwrd(result, 3)
return(DTG)
*--------------------------------------------------------------------------------
function AngleMod(Angle)
   tempmod = math_fmod(Angle, 360.0)

   if (tempmod < 0)
      tempmod = tempmod + 360.0
   endif
return(tempmod)
