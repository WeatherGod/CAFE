#!/usr/local/bin/perl

# THIS PERL SCRIPT GRABS DATA FROM THE EVENTS DATABASE AND OUTPUTS TO HTML
# WRITTEN BY SJG, SUMMER 2005 FOR THE PENNSYLVANIA STATE CLIMATE OFFICE

use DBI;
use CGI qw/:standard :html3/;
use CGI qw/:standard *table start_ul/;

# ------------------------------------------------------------------------
# SETTING DEFAULT VALUES (FOR DEBUGGING PURPOSES)

$event_type = $ARGV[0];

#print $event_type."\n";

$start_year = "1948";
$start_month = "01";
$start_day = "01";

@TimeArr = gmtime;

$end_year = 1900 + $TimeArr[5];
$end_month = 1+$TimeArr[4];
$end_day = $TimeArr[3];
if($end_month<10){$end_month='0'.$end_month;}
if($end_day<10){$end_day='0'.$end_day;}

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
$num_states = '';
$width = '';
$length = '';

$time_correct = '';
$order = "ASC"; 
$format = "Plain Text";
$limit = '9999';

# ------------------------------------------------------------------------
# RETRIEVING HTML FORM DATA

my $query = new CGI;;
#if ($query->param())
#{
     # EVENT TYPE
#	$event_type = $query->param('event_type');

	# START AND END DATES	
#	$start_year = $query->param('start_year');
#	$start_month = $query->param('start_month');
#	$start_day = $query->param('start_day');
#	$end_year = $query->param('end_year');
#	$end_month = $query->param('end_month');
#	$end_day = $query->param('end_day');

	# SELECTION OPTIONS
#	$months = $query->param('month');
#	$validtime = $query->param('validtime');
#	$byevent = $query->param('byevent');

	# FIELD PARAMETERS
#	$datetime = $query->param('datetime');
#	$event_index = $query->param('event_index');
#	$lat = $query->param('lat');
#	$lon = $query->param('lon');
#	$damage = $query->param('damage');
#	$comments = $query->param('comments');
#	$state = $query->param('state');
#	$county = $query->param('county');
#	$fips = $query->param('fips');
#	$site_id = $query->param('site_id');
#	$site_name = $query->param('site_name');
#	$c2 = $query->param('c2');
#	$c3 = $query->param('c3');
#	$c4 = $query->param('c4');
#	$c5 = $query->param('c5');	
#	$township = $query->param('township');
#	$landmark = $query->param('landmark');
#	$datetime_end = $query->param('datetime_end');
#	$dense = $query->param('dense');
#	$time_correct = $query->param('time_correct');
#	$location = $query->param('location');

	# MORE SELECTION OPTIONS
#	$order = $query->param('order');
#	$format = $query->param('format');
#	$limit = $query->param('limit');
#}

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
if($byevent == 1) # group by event, so make custom SQL_what
{
	$dateformat = "%Y-%m-%d %H:%i:%S";
	if($event_type eq 'cold'){$dateformat = "%Y-%m-%d 12:00:00";}
	if($event_type eq 'heat'){$dateformat = "%Y-%m-%d 18:00:00";}
	if($event_type eq 'ozone'){$dateformat = "%Y-%m-%d 18:00:00";}

	# the following tables are so large that they require support tables to convert the select
	# query from order n^2 to order n
	if($event_type eq 't_wind' or $event_type eq 'hail' or $event_type eq 'ozone' or $event_type eq 'tornado')
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

if($validtime==1 && $event_type eq 'fire'){
	if($byevent == 0){ $SQL_where = $SQL_where." and time_correct = 1";}
}

if($byevent == 1 && $event_type eq 'tornado'){
	$SQL_where = $SQL_where." and t2.f_rank >= 5";
}

if($byevent == 1 && $requires_support == 0)
{
	$SQL_where = $SQL_where." and (\@FirstDate :=(SELECT MIN(t2.datetime) FROM ".$table." t2 WHERE t1.event_index = t2.event_index) ) ";
}

if($byevent == 1 && $requires_support == 1)
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

$SQL_QUERY = "\n\nSELECT ".$SQL_what."\n FROM ".$SQL_from." \n WHERE ".$SQL_where.
	"\n ORDER BY ".$orderby." ".$order." LIMIT ".$limit;

# DEBUGGER BREAK
#print "\n\n".$SQL_QUERY."\n\n"; exit();

$host = "gypsy";
$database = "events";

my $dbh = DBI->connect("DBI:mysql:database=$database;host=$host",
                          "webuser", "webuser",
                          {'RaiseError' => 1});

$chk0 = $dbh->prepare("set \@FirstDate = \"2005-01-01 00:00:00\"");
$chk0->execute();
$chk0->finish();

#	$SQL_QUERY = "\n\nSELECT 
#		DATE_FORMAT( 
#			DATE_ADD( 
#				MIN(t1.datetime), INTERVAL AVG( 
#					TIME_TO_SEC( 
#						TIMEDIFF(t1.datetime, \@FirstDate) 
#					) 
#				) 
#			SECOND_MICROSECOND), '$dateformat') 
#		AS avgdatetime 
#		, FORMAT(AVG(event_index),0) AS event_index 
#		, FORMAT(AVG(lat),3) AS avglat 
#		, FORMAT(AVG(lon),3) AS avglon 
#		FROM $table t1 
#		WHERE \@FirstDate :=(select MIN(t2.datetime) FROM $table t2 WHERE t1.event_index = t2.event_index)
#		GROUP BY t1.event_index ";


$chk = $dbh->prepare($SQL_QUERY);
$chk->execute();

# GENERATING OUTPUT

if($format eq "Web Page"){$formatnum=1;}else{$formatnum=0;}

#print header;

# TABLE HEADER

if($formatnum == 1) # Web Page
{
	print qq(<table border="1"><tr>);
	for($i = 0; $i < $field_count; $i++)
	{
		$table_header = $table_header."<th>".$fields[$i]."</th>";
	}
	print $table_header;
	print "</tr>";
} elsif($formatnum == 0) { # Plain Text
#	for($i = 0; $i < $field_count; $i++)
#	{
#		if($i+1 == $field_count)
#		{
#			$table_header = $table_header.$fields[$i];
#		} else {
#			$table_header = $table_header.$fields[$i].",";
#		}
#	}
#	print $table_header;
#	print "\n";
}

# TABLE RECORDS
my $recordcount = 0;
while (my $ref = $chk->fetchrow_hashref()) {

	$table_row = "";
	if($formatnum == 1) # Web Page
	{
		for($j = 0; $j < $field_count; $j++)
		{
			$table_row = $table_row."<td>".$ref->{$fields[$j]}."</td>";
		}
		print "<tr>".$table_row."</tr>";
	} elsif($formatnum == 0) { # Plain Text
		for($j = 0; $j < $field_count; $j++)
		{
			if($j+1 == $field_count)
			{
				$table_row = $table_row.$ref->{$fields[$j]};
			} else {
				$table_row = $table_row.$ref->{$fields[$j]}.",";
			}
		}
		print $table_row;
		print "\n";
	}	

	$recordcount++;
}

if($formatnum == 1){print qq(</table>);}

# SUMMARY OUTPUT
#print qq(<br><b>$recordcount</b> records downloaded.);

#print "<br><br>You executed the following mySQL query: <br><i>".$SQL_QUERY."</i>";

#print end_html;

# CLOSING DATABASE CONNECTION
$chk->finish();
$dbh->disconnect();
