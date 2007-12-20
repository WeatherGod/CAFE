*  setenv GAUDFT /usr/local/grads/udf/regrid2/udft.regrid2
*  setenv GAUDFT /home/meteo/rgrumm/grads/udf/regrid2/udft.regrid2
*
*  This program works for hgtprs 1000,925,850,700 and 500. not for 300 or 250hPa
*
reinit
"set display color white"
*run=2004090812
say "pull YYYYMMDDHH"
pull run
say "User sent in " run
*pull ZZ
idir=substr(run,1,8)
ihour=substr(run,9,2)
*
*  Ben's levels and fields see config.dat
*
LEVELS="1000 925 850 700 500 300 250"
PARMS="tmpprs hgtprs ugrdprs vgrdprs"
NPARMS=4
NLEVS =2
*
MAP = "EAST+" 
CINT=30
*
* MaxTau should be received from the model grids
*
maxtau=29
*
tau=1
tauinc=1
count=0
LAT="20 70"
LON="250 300"
*
* Max-Min Lat Lon values 
*
LAT1 = 28
LAT2 = 52 
LON1 = -105 
LON2 = -55 
*
* Set Map data
*
mpvals="-90 -72 35 47"
mapres="hires"
*
* Open Climo and data is normal but now open MODEL than close/open the climo
*"open climo/hgt_climo.ctl"
*"open /GRIB/"idir"/eta"ihour".ctl"
"open ETA.CTL"
say "Raw MODEL: NAM file open for business" 
*
*
say "Loop over parms"
np=1
MODFILE=1
while(np <= NPARMS)
   PARM=subwrd(PARMS,np)
   say NPARMS" "np" "PARM
   say "open climo file"
   if(PARM = hgtprs)
      "run OpenClimo.gs hgt"
   else
      if(PARM = tmpprs)
         "run OpenClimo.gs air"
      else
          if(PARM = ugrdprs)
            "run OpenClimo.gs uwnd"
          else
             if(PARM = vgrdprs)
               "run OpenClimo.gs vwnd"
             endif
          endif
     endif
  endif
   np = np+1
*
*  Loop Over Levels
*
   nl=1
   while(nl <= NLEVS)
      LEVEL=subwrd(LEVELS,nl)
*      say PARM" nelve: " nl" "LEVEL
*      if(LEVEL = 850 )
          TimeLoop(tau,maxtau,LEVEL,PARM,MAP,MODFILE)
*      endif
      nl = nl+1
   endwhile
   say "close files "
   "close 2"
endwhile
*
*
"quit"

*
function TimeLoop(tau,maxtau,LEVEL,PARM,MAP,MODFILE)
*
* 01 Sep 2004 works only on first pass
* Model File passed in as 1 for the disnorm
*
say "tau "tau" maxtau "maxtau" level "LEVEL" parm "PARM" map "MAP
cint=5
while (tau <= maxtau)
   "clear"
   "set dfile "MODFILE
   etainit=getDTG(1)
   verifday=getDTG(tau)
   "set t "tau
   "q dims"
   rec=sublin(result,5)
   valideta=subwrd(rec,6)
   dtg=substr(valideta,4,5)
*
*
*
   meandtg = meanDTG(valideta)
   fcast=(tau-1)*3
   rc=disnorm(meandtg,PARM,LEVEL,tau,cint,MAP,1)
*
* 
   "set dfile 1"
   "run Top2Peaks.gs AnomName"
    PeakInfo=result
*
   "set gxout contour"
   "run Top2Valleys.gs AnomName"
    ValleyInfo=result
   say tau" PEAK "etainit" "verifday" "PARM" "LEVEL" "PeakInfo" "meandtg
   say tau" VALLEY "etainit" "verifday" "PARM" "LEVEL" "ValleyInfo" "meandtg
  "d AnomName"
   Extreme(AnomName,MAP)
*   "printim test-"tau".gif"
   tau = tau +1
endwhile
return

function meanDTG(dtg)
*
* Use input dtg to get the mean DTG from the climo files
* -Assumes there is 6-hourly climated data
*
DATE=substr(dtg,4,5)
HOUR=substr(dtg,1,2)
CYCLE=HOUR
if( HOUR = 03) 
    CYCLE = 00
else
   if(HOUR = 09)
     say "HOUR is 09 set to 06"
     CYCLE =06
   else
     if(HOUR=15)
        say "HOUR Is 15 set to 12"
        CYCLE=12
     else
        if(HOUR =21) 
           CYCLE=18
        endif
     endif
   endif
endif
*
if (dtg != "29FEB")
    meandtg=CYCLE"Z" DATE "1900"
 else
    meandtg=CYCLE"Z" 28FEB "1900"
endif
say dtg" " DATE" HOUR: "HOUR" MEANDTG: "meandtg
*pull ZZ
return meandtg

function getDTG(T)
*
* Use q time and third and fifth will give the time same if set t 1 but vary if set t 1 10
*set t "T
"q time"
DTG=subwrd(result,3)
return DTG

*****************************************************************************
function disnorm(meandtg,etafield,plev,dt,cint,MAP,FILE)
*
* MEANDTG  : Mean date for Climo
* etafiedl : eta variable we are using
* plev     : Pressure level
* dt       : 
* CINT     :
* MAP      : MAP
* FILE     : MOdel data file
*
*
say MAP" disnorm FILE: "FILE" etafield: "etafield" tau "dt" meandtg "meandtg
"run MAP.gs "MAP
"set lev "plev
"set dfile "FILE
"define etavar="etafield"."FILE"(t="dt")"
TT=getDTG(dt)
*
"set dfile 2"
"set time "meandtg
"set lev "plev
"set rgb 50 0 150 0"
"define monmean=regrid2(mean,0.5,0.5,bl_p1,205.00,10.00)"
"define monstd=regrid2(stddev,0.5,0.5,bl_p1,205.00,10.00)"
*"define monstd=regrid2(stddev,0.5,0.5,bl_p1,-155.00,10.00)"
*"define monmean=regrid2(mean,0.5,0.5,bl_p1,-155.00,10.00)"
"set gxout contour"
"set ccolor 6"
"set cthick 5"
say "display monmean for testing only"
*"d "monmean
"set grads off"
"set clevs -5 -4 -3 -2 -1 1 2 3 4 5 "
"set ccols 15 9 14 11 5 0 7 12 2 6 15"
*"d (etavar-monmean)/monstd"
"run cbarn.gs 1 1 0.4 2.75 "
"set gxout shaded"
"set cthick 11"
"set clab on"
"set clevs -6 -5.5 -5 -4.5 -4 -3.5 -3 -2.5 -2 -1.5 -1 1 1.5 2 2.5 3 3.5 4 4.5 5 5.5 6"
"define AnomName = ("etavar" -"monmean")/"monstd")"
*
"set clab on"
"set clevs -5 -4 -3 -2 -1 1 2 3 4 5 "
"set ccols 15 9 14 11 5 0 7 12 2 6 15"
"d "AnomName 
"run cbarn.gs 1 1 0.4 2.75 "
"set gxout contour"
"set clab on"
"set ccolor 1"
"set cstyle 1"
"set cthick 11"
"d "etavar
*
say "plotted test plot"
return AnomName 

function getmonth(string)

monstr = "JAN FEB MAR APR MAY JUN JUL AUG SEP OCT NOV DEC"

ii=1
while (ii<=12)
   if (subwrd(monstr,ii) = string)
      number=ii
   endif
  ii=ii+1
endwhile

return(number)


function Extreme(AnomName,MAP)
*
*
"run MAP.gs "MAP
"set gxout stat"
"d AnomName"
rec=sublin(result,8)
say rec
nmin=subwrd(rec,4)
nmax=subwrd(rec,5)
say "max "nmax" min "nmin
return
*
*
*function Top2Peaks(args)
function Top2Peaks(VarName)
*
*  Top 2 peaks in a dataset
*
*   VarName=subwrd(args,1)
*   say "******Top2Peaks VarName "VarName
   GridInfo = GridBoundaries()
   XBound1 = subwrd(GridInfo, 1)
   XBound2 = subwrd(GridInfo, 2)
   YBound1 = subwrd(GridInfo, 3)
   YBound2 = subwrd(GridInfo, 4)
*
*  Notice how the edge is not being used in this work
*
   Xindex    = XBound1 + 1
   PeakVal.1 = -999
   PeakX.1   = -999
   PeakY.1   = -999
   PeakVal.2 = -999
   PeakX.2   = -999
   PeakY.2   = -999
*
   while (Xindex < XBound2)
*      say "x loop "Xindex
      Yindex = YBound1 + 1
*
      while (Yindex < YBound2)
         CentVal  = GetValue(VarName, Xindex, Yindex)
         UpVal    = GetValue(VarName, Xindex, Yindex + 1)
         DownVal  = GetValue(VarName, Xindex, Yindex - 1)
         LeftVal  = GetValue(VarName, Xindex - 1, Yindex)
         RightVal = GetValue(VarName, Xindex + 1, Yindex)
         UpRiVal  = GetValue(VarName, Xindex + 1, Yindex + 1)
         UpLeVal  = GetValue(VarName, Xindex - 1, Yindex + 1)
         DwnRiVal  = GetValue(VarName, Xindex + 1, Yindex - 1)
         DwnLeVal  = GetValue(VarName, Xindex - 1, Yindex - 1)
*TEST         say "y  "Yindex" "CentVal" "UpVal
         if ((CentVal >= UpVal) & (CentVal >= DownVal) & (CentVal >= LeftVal) & (CentVal >= RightVal) & (CentVal >= UpRiVal) & (CentVal >= UpLeVal) & (CentVal >= DwnRiVal) & (CentVal >= DwnLeVal))
*
*     In other words, if the CentVal is greater than or equal to all values that neighbors it, then it is true
*
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
*
         Yindex = Yindex + 1
      endwhile
      Xindex = Xindex + 1
   endwhile
*
*  Make Lat/Lon of X/Y
*
 'q gr2w 'PeakX.1' 'PeakY.1
  X1 = subwrd(result,3) 
  Y1 = subwrd(result,6) 
 'q gr2w 'PeakX.2' 'PeakY.2
  X2 = subwrd(result,3) 
  Y2 = subwrd(result,6) 
return (PeakVal.1' 'X1' 'Y1' 'PeakVal.2' 'X2' 'Y2)
***return (PeakVal.1' 'PeakX.1' 'PeakY.1' 'PeakVal.2' 'PeakX.2' 'PeakY.2)

*
function LatLon(LAT,LON)
*
*
"q gr2w "LAT" "LON
lat=subwrd(result,3)
lon=subwrd(result,6)
out=lat" "lon
say out
return (out)
