#!/usr/bin/perl -w
use strict;

my $cfgfile = "/var/tmp/wifi4.conf";
my $log = "/var/tmp/wifi4.log";

my $device = "wlan0";

my $mode;

if ( -f $cfgfile) {
    $mode = "station";
} else {
    $mode = "ap";
}


# configuration server

{
    package MyWebServer;

    use HTTP::Server::Simple::CGI;
    use base qw(HTTP::Server::Simple::CGI);

    sub handle_request {
	my $self = shift;
	my $q  = shift;

	# process or send out form

	resp_hello($q);
    }

    sub resp_hello {
	my $q  = shift;   # CGI.pm object
	return if !ref $q;

	my $ssid = $q->param('ssid');
	my $pass = $q->param('pass');
	my $err = "";
	my $restart = 0;

	if (! defined $ssid) { $ssid = "" };
	if (! defined $pass) { $pass = "" };

	if ( length($ssid)>1 && length($pass) >1 ) {
	    # add this to existing table
	    if (open(FD,">>$cfgfile")) {
	        print FD "$ssid:$pass\n";
		close(FD);
		chmod(0600,$cfgfile);

		# attempt station reconfigure
		$restart = 1;

	    } else {
		$err = "failed writing datastore";
	    }
	}
	
	print "HTTP/1.0 200 OK\r\n";
	print $q->header;

	if ($restart == 1) {
	    print <<EOF;
<html>
<head>
</head>
<body>
<h2>Reconfiguring to client mode</h2>
</body>
</html>
EOF

	    `/sbin/reboot`;

	} else {
	    print <<EOF;
<html>
<head>
</head>
<body>
<h2>Enter Wifi Credentials</h2>
<font color='red'>$err</font><br>
<form action="action.pl">
<table>
<tr><td>ssid</td><td>
<input type="text" name="ssid" value="$ssid">
</td></tr>
<tr><td>password</td><td>
<input type="text" name="pass" value="$pass">
</td></tr>
</table>
<input type="submit" value="Submit">
</form>
</body>
</html>
EOF
        }
    }
} 

sub deployap {

    print STDERR "deployap()\n";

    `killall dnsmasq`;
    `killall hostapd`;
    `ifconfig $device down`;
    `killall wpa_supplicant`;
    `killall dhclient`;
    `killall apache`;
    `killall apache2`;
    `killall httpd`;
    `killall nginx`;


    # write temporary host mode configs 

    my $id = "random";
    my @r = `ifconfig $device`;

    chomp $r[0];
    print $r[0]."\n";
    if ($r[0] =~ /HWaddr.*:(..):(..):(..)/ ) {
       $id="$1$2$3";
    }

    $id = "RPI-$id";

    if (open(FD,">/tmp/hostapd.conf")) {
	print FD <<EOF;
interface=$device
driver=nl80211
ssid=$id
channel=6
EOF
	close(FD);
    }

    if (open(FD,">/tmp/dnsmasq.conf")) {
	print FD <<EOF;
log-facility=/var/log/dnsmasq.log
address=/#/192.168.4.1
interface=$device
dhcp-range=192.168.4.10,192.168.4.250,12h
no-resolv
log-queries
EOF
	close(FD);
    }

    # set restrictive firewall

    `/sbin/iptables -F`;
#    `/sbin/iptables -i $device -A INPUT -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT`;
#    `/sbin/iptables -i $device -A INPUT -p tcp --dport 80 -j ACCEPT`;
#    `/sbin/iptables -i $device -A INPUT -p udp --dport 53 -j ACCEPT`;
#    `/sbin/iptables -i $device -A INPUT -p udp --dport 67:68 -j ACCEPT`;
    `/sbin/iptables -i $device -A INPUT -j ACCEPT`;

    # start hostapd services

    sleep 2;
    `ifconfig $device 192.168.4.1 netmask 255.255.255.0 up`;
    `dnsmasq -C /tmp/dnsmasq.conf`;
    `hostapd -B /tmp/hostapd.conf`;

    # spawn our configuration server

    print STDERR "spawn webservice()\n";
    my $pid = MyWebServer->new(80)->background();
    exit(0);
    return "exit";
}

sub trystation {

    print STDERR "trystation()\n";

    my $h = [];
    my $j = 0;
    if (open(FD,"<$cfgfile")) {
        while (my $s = <FD>) {
	    chop($s); chomp($s);
	    print STDERR "from config: $s\n";
	    if ($s =~ /^(.*):(.*)$/) {
		print STDERR "adding $s\n";
		$h->[$j++] = $1;
		$h->[$j++] = $2;
	    }
	}
	close(FD);
    }

    my $network;
    my $found = "none";
    my $key = undef;
    my $priority = 100000;
    my $attempts = 0;

    system("ifconfig $device up");
    while ($found eq "none") {
	my @r = `iwlist $device scan`;
	foreach my $r (@r) {
	    if ($r =~ /ESSID:"(.*)"/) {
		my $tn = $1;
		print STDERR "checking $tn\n";
		for (my $i=0;$i<(scalar @$h);$i+=2) {
		    if ($tn eq $h->[$i] && $i<$priority) {
			print STDERR "found $tn\n";
			$found = $tn;
			$priority = $i;
			$key = $h->[$i+1];
		    }
		}
	    }
	}
	if ($found eq "none") {
	    $attempts++;
	    if ($attempts > 3) {
		return("ap");
	    }
	}
    }

    $network = $found;

    print STDERR "trying $network...";

    open (FD,">/etc/wpa_supplicant.conf") or die "failed to open /etc/wpa_supplicant.conf";
    print FD "network={\n";
    print FD '  ssid="'.$network.'"'."\n";
    print FD '  psk="'.$key.'"'."\n";
    print FD "}\n";
    close(FD);

    system("ifconfig $device down");
    system("killall wpa_supplicant");
    system("killall dhclient");
    sleep(2);
    system("ifconfig $device up");
    sleep(2);
    if ($key ne "nokey") {
	open(FD,">/etc/my_wpa_supplicant.conf") or die "cant write /etc/my_wpa_supplicant.conf";
	print FD <<EOF;
network={
    ssid="$network"
    psk="$key"
}
EOF
	close(FD);
	#system("iwconfig $device essid $network");
	#system("iwconfig $device key $key");
	system("wpa_supplicant -c/etc/my_wpa_supplicant.conf -i$device -Dnl80211,wext &");
    }
    system("dhclient $device");
    return("exit");
}



for (;;) {
    if ( $mode eq "station") {
        $mode = trystation();

    } elsif ($mode eq "ap") {
        $mode = deployap();

    } elsif ($mode eq "exit") {
	exit(0);

    } else {
	print STDERR "error: illegal mode $mode\n";
	exit(2);
    }
}
