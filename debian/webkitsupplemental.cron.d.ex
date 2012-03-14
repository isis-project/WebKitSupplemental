#
# Regular cron jobs for the webkitsupplemental package
#
0 4	* * *	root	[ -x /usr/bin/webkitsupplemental_maintenance ] && /usr/bin/webkitsupplemental_maintenance
