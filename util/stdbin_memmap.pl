#!/usr/bin/perl
use strict;
use warnings;

my $filename = 'stdbin_memmap.csv';

open(my $fh, '<', $filename) or die "Could not open file '$filename' $!";

my $cnt = 0;
my $hdrlen = 25;
my $msum = 0;

# select number from page 155 in IXblue interface manual 
my @select_blks = [0,1,4,7,8,9,15,16,17,24,26];

while (my $row = <$fh>) {

  if ( $cnt ~~ @select_blks ) { 
    chomp $row;
    
    my @flst = split(',',$row);

    my $mlen = $flst[0];
    my $mabs = $msum + $hdrlen; 

    $msum = $mlen + $msum;

    print "$cnt,  $mabs, $flst[1]\n";
  }

  $cnt++;
}

close $fh;
