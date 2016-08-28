#!/usr/bin/perl -w
use strict;
use DBI;
use DBD::SQLite;
use Data::Dumper;
use Socket;
use Sys::Hostname;

my $dbh;
my $qqq;
my $sth;
my $res;
my $row;
my $n;
my $rsock;
my $ssock;
my $s;
my @a;
my $chost;
my $caddr;
my $cport;
my $csockaddr;
my $cipaddr;
my $initflag;
my $rin;
my $win;
my $ein;
my $rout;
my $wout;
my $eout;
my $timeout;
my $now;

my $responsetime;;
my $retrytime;
my $success;
my $moretodo;
my $backoff;
my $rhost;
my $rport;
my $nfound;
my $timeleft;
my $ts;
my $servicecount;
my $hisiaddr;
my $hispaddr;
my $magic;

$initflag = 0;
$timeout = 5;

my $port = shift;;
my $db   = shift;

if (! defined $db || ! defined $port) {
    print "usage: tinymq.pl port dbfilename\n";
    exit(-1);
}

if (! -f $db) {
    $initflag = 1;
}

$dbh = DBI->connect("dbi:SQLite:dbname=$db","","");

if ($initflag) {
    $qqq = "CREATE TABLE mqout (x INTEGER PRIMARY KEY, ts INTEGER NOT NULL DEFAULT CURRENT_TIMESTAMP, shost TEXT, sport INTEGER, dhost TEXT, dport INTEGER, token TEXT, data TEXT, seq INTEGER, backoff INTEGER DEFAULT 1, magic TEXT, UNIQUE(magic))";
    $sth = $dbh->prepare($qqq);
    $sth->execute();
    $qqq = "CREATE TABLE mqin  (x INTEGER PRIMARY KEY AUTOINCREMENT, ts INTEGER NOT NULL DEFAULT CURRENT_TIMESTAMP, shost TEXT, sport INTEGER, dhost TEXT, dport INTEGER, token TEXT, data TEXT, seq INTEGER, magic TEXT, UNIQUE(magic) )";
    $sth = $dbh->prepare($qqq);
    $sth->execute();
}

my $proto = getprotobyname('udp');
my $iaddr = gethostbyname(hostname());
my $paddr = sockaddr_in($port, INADDR_ANY); # 0 means let kernel pick

socket($rsock, PF_INET, SOCK_DGRAM, $proto)   or die "socket: $!";
bind($rsock, $paddr)                          or die "bind: $!";

socket($ssock, PF_INET, SOCK_DGRAM, $proto)   or die "socket: $!";


print STDERR "listening on $port\n";

for (;;) {
    $rin = $win = $ein = '';
    vec($rin,fileno($rsock), 1) = 1;
    $ein = $rin | $win;
    ($nfound,$timeleft) = select($rout=$rin, $wout=$win, $eout=$ein, $timeout);

    print "nfound $nfound\n";
    if ($nfound > 0) {
        $hispaddr = recv($rsock, $a,65536,0);
	($cport, $hisiaddr) = sockaddr_in($hispaddr);
	$caddr = inet_ntoa($hisiaddr);
	#$host = gethostbyaddr($hisiaddr, AF_INET);
	print STDERR Dumper($a);
	@a = split(/:/,$a);
	print STDERR Dumper(@a);
	print STDERR "caddr $caddr\n";
	print STDERR "cport $cport\n";

	# key is shost:sport:token:seq
	# should I add this as a key? and require a UNIQUE

	$magic = "$caddr:$cport:$a[4]:$a[0]";

	print STDERR "magic >$magic<\n";

	$qqq = "INSERT OR IGNORE INTO mqin (shost,sport,token,data,seq, magic) VALUES (?,?,?,?,?,?)";
	print STDERR "$qqq\n";
	$sth = $dbh->prepare($qqq);
	$sth->execute($caddr,$cport,$a[4],$a[5],$a[0],$magic);
	$s = join(",","ACK",$magic);
	print STDERR "send >$s<\n";
        send($rsock, $s, 0, $hispaddr);

    } else { # check for outbound work
        $now = time();
	$moretodo = 1;
	while ($moretodo) {
	    # find the oldest record of each host that needs to be sent
	    $qqq = "SELECT x,strftime('%s',ts),dhost,dport,token,data,backoff FROM mqout GROUP BY dhost ORDER BY x ASC";
	    print STDERR "$qqq\n";

	    $sth = $dbh->prepare($qqq);
	    $res = $sth->execute();

	    $servicecount = 0;
	    while ($row = $sth->fetchrow_arrayref) {
		print STDERR Dumper($row);
		$servicecount++;

		$rhost = $row->[2];
		$rport = $row->[3];
		$backoff = $row->[7];
		$ts      = $row->[1];

		#if ($ts + $backoff < $now) {
		#    print STDERR "ignoring record due to $ts + $backoff < $now\n";
		#    next;
		#}

		$retrytime = 2;
		$responsetime = 4;
		$success = 0;
		while ($success == 0) {
		    $s = join(":",@$row);
		    print STDERR "sending $s\n";
		    $hisiaddr = inet_aton($rhost) or die "unknown host";
		    $hispaddr = sockaddr_in($rport, $hisiaddr);
		    send($ssock,$s,0,$hispaddr);

		    print STDERR "waiting $responsetime for ack\n";
		    $rin = $win = $ein = '';
		    vec($rin, fileno($ssock), 1) = 1;
		    ($n,$timeleft) = select($rout=$rin, $wout=$win, $eout=$ein, $responsetime);
		    if ($n>0) {
			$n = recv($ssock, $a,65536,0);
			print STDERR "recv got >$a<\n";
			if ($a =~ /^ACK/ ) {  # FIXME accept anything as an ACK right now
			    print STDERR "got ack\n";
			    $success = 1;
			    $qqq = "DELETE FROM mqout WHERE x = ?";
			    print STDERR "$qqq\n";
			    $sth = $dbh->prepare($qqq);
			    $res = $sth->execute($row->[0]);
			} else {
			    if ($backoff < 32) {
				$qqq = "UPDATE mqout SET backoff = ? WHERE x = ?";
				$sth = $dbh->prepare($qqq);
				$res = $sth->execute($backoff*2,$row->[0]);
				print STDERR "bad ack, backoff " . $backoff*2 . "\n";
			    }
			}
		    } else {
			if ($backoff < 32) {
			    $qqq = "UPDATE mqout SET backoff = ? WHERE x = ?";
			    $sth = $dbh->prepare($qqq);
			    $res = $sth->execute($backoff*2,$row->[0]);
			    print STDERR "no ack, backoff " . $backoff*2 . "\n";
			}
		    }
		}
	    }
	    $moretodo = $servicecount;
	}
    }
}
