# TinyReliablePersistentMessageQueue
Tiny implementation of a Reliable, Persistent, Message Queue

The goal of this project is to create an ever tinier Reliable, Persistent, Message Queuing System for textual messages.

If you want to send binary you will have to base64 or otherwise ASCII armor the message.

tinymq uses an SQLite database for persistent message storage on both the sending and
receiving end of the queue.

You run an instance of tinymq on each host at a port of your choice.  I usually use port 9000.

An application wishing to send a message does a simple SQLite INSERT:

INSERT into mqout (host,port,token,data) VALUES ('some address','9000','token','my message');

'my message' can be any ASCII armored string.  I often put JSON in there.

The transport is UDP and the protocol implements its own reliability layer.  

tinymq will never give up trying to deliver a message.  The only way to stop its attempts is to delete the entry from the mqout table of the SQLite database.

tinymq is not particularly fast, but it is reliable and resilient.  You should probably limit individual messages to less than 64000 characters.  I generally use it for much shorter messages.

The message, when delivered to the recipient tinymq process, will end up in the mqin table of the recipient's SQLite database.

Everything is auto-created as needed.

Installing Dependencies:

sudo apt-get install cpanminus sqlite3

sudo cpanm Sys::Hostname

sudo cpanm Socket

sudo cpanm Data::Dumper

sudo cpanm DBD::SQLite

sudo cpanm DBI


Run the script:

Then just run the script on each machine

MachineA% perl tinymq.pl 9000 /var/tmp/tinymqdb

MachineB% perl tinymq.pl 9000 /var/tmp/tinymqdb

MachineC% perl tinymq.pl 9000 /var/tmp/tinymqdb

and so on.

(you may need to open up port 9000 if you have a firewall)

Send Messages:

From MachineA you can send a message to MachineB

MachineA% echo "INSERT into mqout (host,port,token,data) VALUES ('MachineB','9000','token','HelloMachineBFromMachineA');" | sqlite3 /var/tmp/tinymqdb

After the message is delivered you can see it on MachineB:

MachineB% echo "SELECT * FROM mqin;" | sqlite3 /var/tmp/tinymqdb

Of course, MachineA and MachineB must resolve to real machine names or IP addresses.

Caveats:

There is no encryption of the communications.  Myself I encrypt at the network layer with IPSec or OpenVPN.

Token is a future enhancement via which I plan to implement a low CPU demand encryption layer.  It will probably be shared key with AES128.

SQLite is not exactly small.  I may implement a lighter weight message store.

Did I mention tinymq is slow?  I would not try to pass messages through it at a high rate.
