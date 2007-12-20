#!/usr/local/bin/perl

# THIS PERL SCRIPT GRABS DATA FROM THE EVENTS DATABASE AND OUTPUTS TO HTML
# WRITTEN BY SJG, SUMMER 2005 FOR THE PENNSYLVANIA STATE CLIMATE OFFICE

use DBI;
use CGI qw/:standard :html3/;
use CGI qw/:standard *table start_ul/;

# ------------------------------------------------------------------------
# SETTING DEFAULT VALUES (FOR DEBUGGING PURPOSES)

$event_type = 'cold';

$start_year = "1948";
$start_month = "01";
$start_day = "01";
$end_year = "2005";
$end_month = "12";
$end_day = "31";

$months = '0';
$validtime = 1;
$byevent = 1;

$datetime = 'datetime';
$event_index = 'event_index';
$lat = 'lat';
$lon = 'lon';
$damage = '';
$comments = '';
$state = '';
$site_id = '';
$county = '';
$fips = '';
$c2 = '';
$c3 = '';
$c4 = '';
$c5 = '';
$township = '';
$landmark = '';
$datetime_end = '';
$dense = '';
$location = '';
$wfo = '';
$event_name = '';
$num_states = '';
$width = '';
$length = '';

$time_correct = '';
$order = "ASC"; 
$format = "Plain Text";
$limit = '3000';

# ------------------------------------------------------------------------
# RETRIEVING HTML FORM DATA

# ------------------------------------------------------------------------
# CHECKING WHICH FIELDS TO RETRIEVE

$table = $event_type;
$orderby = "t1.datetime";

my $field_count = 0;
@fields = {'0'};
if($datetime){$fields[$field_count]=$datetime;$field_count++;}
if($event_index){$fields[$field_count]=$event_index;$field_count++;}
if($lat){$fields[$field_count]=$lat;$field_count++;}
if($lon){$fields[$field_count]=$lon;$field_count++;}

if($damage){$fields[$field_count]=$damage;$field_count++;}
if($comments){$fields[$field_count]=$comments;$field_count++;}
if($state){$fields[$field_count]=$state;$field_count++;}
if($site_id){$fields[$field_count]=$site_id;$field_count++;}
if($site_name){$fields[$field_count]=$site_name;$field_count++;}
if($county){$fields[$field_count]=$county;$field_count++;}
if($fips){$fields[$field_count]=$fips;$field_count++;}
if($c2){$fields[$field_count]=$c2;$field_count++;}
if($c3){$fields[$field_count]=$c3;$field_count++;}
if($c4){$fields[$field_count]=$c4;$field_count++;}
if($c5){$fields[$field_count]=$c5;$field_count++;}
if($township){$fields[$field_count]=$township;$field_count++;}
if($landmark){$fields[$field_count]=$landmark;$field_count++;}
if($datetime_end){$fields[$field_count]=$datetime_end;$field_count++;}
if($dense){$fields[$field_count]=$dense;$field_count++;}
if($location){$fields[$field_count]=$location;$field_count++;}
if($wfo){$fields[$field_count]=$wfo;$field_count++;}
if($event_name){$fields[$field_count]=$event_name;$field_count++;}
if($num_states){$fields[$field_count]=$num_states;$field_count++;}
if($width){$fields[$field_count]=$width;$field_count++;}
if($length){$fields[$field_count]=$length;$field_count++;}
if($time_correct){$fields[$field_count]=$time_correct;$field_count++;}

if($byevent == 1) # group by event, so override field selection
{
	$fields[0] = "avgdatetime";
	$fields[1] = "event_type";
	$fields[2] = "avglat";
	$fields[3] = "avglon";
	$field_count = 4;

	#$orderby = "t1.avgdatetime";
} 

# add the selected fields to the SQL statement
$SQL_what = "0";
for($i = 0; $i < $field_count; $i++)
{
	$SQL_what = $SQL_what.",".$fields[$i];
}

if($event_type eq 'fire' && $byevent == 0){$SQL_what = $SQL_what.",time_correct AS time_correct";}


$requires_support = 0;

if($byevent == 1) # group by event, so make custom SQL_what (** NEW CODE)
{
	$datepart = "
	from_days(
           avg(
              to_days( datetime )*86400 + time_to_sec(time( datetime ))
           ) / 86400
        )";
	$timepart = "sec_to_time(
           mod(
              avg(
                 to_days( datetime )*86400 + time_to_sec(time( datetime))
              ), 86400
           )
        )";
	
	# these special event types override the time field for grouping by event 	
	if($event_type eq 'cold'){$timepart = "\'12:00:00\'";}
	if($event_type eq 'cold_old'){$timepart = "\'12:00:00\'";}
	if($event_type eq 'heat'){$timepart = "\'18:00:00\'";}
	if($event_type eq 'heat_old'){$timepart = "\'18:00:00\'";}
	if($event_type eq 'ozone'){$dateformat = $timepart = "\'18:00:00\'";}

	$SQL_what = "concat($datepart,\" \",$timepart) AS avgdatetime 
		, \"$event_type\" AS event_type 
		, FORMAT(AVG(lat),3) AS avglat 
		, FORMAT(AVG(lon),3) AS avglon";

	if($event_type eq 'cold'){$SQL_what = $SQL_what.", count(*) as occurrences";}
	if($event_type eq 'heat'){$SQL_what = $SQL_what.", count(*) as occurrences";}
	if($event_type eq 'tornado'){$SQL_what = $SQL_what.", sum(comments) as f_rank";}

} # end group by event

# ---OLD! Please remove this code.
if($byevent == 2) # group by event, so make custom SQL_what OLD CODE
{
   $dateformat = "%Y-%m-%d %H:%i:%S";
	if($event_type eq 'cold'){$dateformat = "%Y-%m-%d 12:00:00";}
	if($event_type eq 'cold_old'){$dateformat = "%Y-%m-%d 12:00:00";}
	if($event_type eq 'heat'){$dateformat = "%Y-%m-%d 18:00:00";}
	if($event_type eq 'heat_old'){$dateformat = "%Y-%m-%d 12:00:00";}
	if($event_type eq 'ozone'){$dateformat = "%Y-%m-%d 18:00:00";}

	# the following tables are so large that they require support tables to convert the select
	# query from order n^2 to order n
	if($event_type eq 't_wind' or $event_type eq 'hail' or $event_type eq 'ozone' or $event_type eq 'tornado'
		or $event_type eq 'flood' or $event_type eq 'flash_flood' or $event_type eq 'cold' or $event_type eq 'heat')
	{
		$requires_support = 1;

	$SQL_what = "
		DATE_FORMAT(
			DATE_ADD(
				t2.datetime_min, INTERVAL AVG(
					TIME_TO_SEC(
						TIMEDIFF(t1.datetime, t2.datetime_min)
					)
				) 
			SECOND_MICROSECOND), '$dateformat')
		AS avgdatetime 
		, \"$event_type\" AS event_type 
		, FORMAT(AVG(lat),3) AS avglat 
		, FORMAT(AVG(lon),3) AS avglon";
#	if($event_type eq 'tornado' && $byevent == 1){$SQL_what = $SQL_what.",sum(comments+1) AS f_sum";}

	} else { #use the less efficient query that does not require a support table
	$SQL_what = "
		DATE_FORMAT( 
			DATE_ADD( 
				MIN(t1.datetime), INTERVAL AVG( 
					TIME_TO_SEC( 
						TIMEDIFF(t1.datetime, \@FirstDate) 
					) 
				) 
			SECOND_MICROSECOND), '$dateformat') 
		AS avgdatetime 
		, \"$event_type\" AS event_type 
		, FORMAT(AVG(lat),3) AS avglat 
		, FORMAT(AVG(lon),3) AS avglon";
	}
}

# ------------------------------------------------------------------------
# CHECKING WHAT TIMES TO RETRIEVE

$start_date = $start_year."-".$start_month."-".$start_day;
$end_date = $end_year."-".$end_month."-".$end_day;

$SQL_from = $table." t1";

$SQL_where = "";

$SQL_where = $SQL_where." (DATEDIFF(t1.datetime, '$start_date') >= 0 and DATEDIFF(t1.datetime, '$end_date') <= 0)";

if($months ne "0")
{
	$SQL_where = $SQL_where." and MONTH(datetime) = ".$months;
}

if($validtime==1 && $event_type eq 'fire' && $byevent == 0){
	$SQL_where = $SQL_where." and time_correct = 1";
}

if($byevent == 2 && $requires_support == 0)
{
	$SQL_where = $SQL_where." and (\@FirstDate :=(SELECT MIN(t2.datetime) FROM ".$table." t2 WHERE t1.event_index = t2.event_index) ) ";
}

if($byevent == 2 && $requires_support == 1)
{
	$SQL_where = $SQL_where." and t1.event_index = t2.event_index ";
	$SQL_from = $SQL_from.", ".$table."_support t2";
}

if($byevent == 1)
{
	$SQL_where = $SQL_where." GROUP BY t1.event_index ";
}

# ------------------------------------------------------------------------
# SQL DATABASE CONNECTION AND QUERY

#$SQL_where = "1";

$SQL_QUERY = "\n\nSELECT ".$SQL_what."\n FROM ".$SQL_from." \n WHERE ".$SQL_where.
	"\n ORDER BY ".$orderby." ".$order." LIMIT ".$limit;

# DEBUGGER BREAK
# print "\n\n".$SQL_QUERY."\n\n"; exit();

$host = "gypsy";
$database = "events";

my $dbh = DBI->connect("DBI:mysql:database=$database;host=$host",
                          "webuser", "webuser",
                          {'RaiseError' => 1});

$chk = $dbh->prepare($SQL_QUERY);
$chk->execute();

# TABLE RECORDS

while (my $ref = $chk->fetchrow_hashref()) 
{
	$omit = 0;
	if($byevent == 1 && $event_type eq 'tornado' && $ref->{f_rank} < 5){
		$omit = 1; #only want to include tornado events where the sum of the f-scale values is >= 5
	}

	if($byevent == 1 && $event_type eq 'heat' && $ref->{occurrences} < 5){
		$omit = 1; #only want to include heat events with more than 5 occurrences per event
	}
	if($byevent == 1 && $event_type eq 'cold' && $ref->{occurrences} < 5){
		$omit = 1; #only want to include cold events with more than 5 occurrences per event
	}

	my $table_row = "";
	if($omit == 0)
	{
	  for(my $j = 0; $j < $field_count; $j++)
	  {
		 if($j+1 == $field_count)
		 {
			$table_row = $table_row.$ref->{$fields[$j]};
		 } else {
			$table_row = $table_row.$ref->{$fields[$j]}.",";
		 }
	  }
	  print "$table_row\n";
	}	
}

# CLOSING DATABASE CONNECTION
$chk->finish();
$dbh->disconnect();
