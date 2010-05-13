#!/usr/bin/env perl

use strict;
use warnings;

#use Smart::Comments::JSON '###';
use Data::Compare;
use POSIX qw(floor);
use List::Util qw( min max );
use Clone qw( clone );

my (%edges, @edges, %colors, %urls, %methods);
my $active_req;

my $frame = 0;

my $lightColor = 1;
my $r = 256;
my $g = 256;
my $b = 256;

my $border2 = 360;

sub parse_enter ($);
sub gen_color ();
sub gen_dot ();
sub gen_last_html ();
sub gen_html ();
sub gen_dot_for_node ($$);

while (<>) {
    if (/^>enter (.*)/) {
        my $saved_ctx = clone([$active_req, \@edges]);

        parse_enter($1);

        my $ctx = [$active_req, \@edges];
        if (! Compare($saved_ctx, $ctx)) {
            #if ($frame == 8) {
                #warn "frame $frame: $active_req\n";
            #}

            gen_dot();
            gen_html();
            $frame++;
        }
    }
}

#warn "weird active url: $methods{'0x6c17b8'} $urls{'0x6c17b8'}\n";

gen_last_html();

sub gen_last_html () {
    system("tpage --define last_page=1 --define n=$frame dodo.tt > fr-$frame.html") == 0
        or die "Failed to invoke tpage to generate fr-$frame.html\n";
}

sub gen_html () {
    system("tpage --define n=$frame dodo.tt > fr-$frame.html") == 0
        or die "Failed to invoke tpage to generate fr-$frame.html\n";
}

sub parse_enter ($) {
    my $spec = shift;

    my ($method, $url, $c, $m, $r, $ar, $pr, $posted);

    if ($spec =~ /^(\w+) (\S+) c:(\d+) m:(\S+) r:(\S+) ar:(\S+) pr:(\S+)(?: posted:(\S+))?/g) {
        ($method, $url, $c, $m, $r, $ar, $pr, $posted) = ($1, $2, $3, $4, $5, $6, $7);
    } else {
        die "line $.: invalid enter spec: $spec\n";
    }

    my @posted;
    if ($posted) {
        @posted = split /,/, $posted;
    }

    if (! $edges{"$pr-$r"}) {
        $edges{"$pr-$r"} = 1;
        push @edges, [$pr, $r];
    }

    if (! $colors{$r}) {
        $colors{$r} = gen_color();
    }

    ### $c $m $r $ar $pr @posted
    $urls{$r} = $url;
    $methods{$r} = $method;

    $active_req = $ar;
}

sub gen_color () {
    my $border;

    my $r1 = int rand $r;
    my $r2 = int rand $g;
    my $r3 = int rand $b;

    my $sum = $r1 + $r2 + $r3;

    if (1) {
        my $border = 120 * 3;
        while ($sum < $border) {
            my $choose = int rand 3;
            if ($choose == 0) {
                $r1 += min(2, $border - $sum, 250 * 3 - $sum);
            } elsif ($choose == 1) {
                $r3 += min(2, $border - $sum, 250 * 3 - $sum);
            } else {
                # $choose == 2
                $r2 += min(2, $border - $sum, 250 * 3 - $sum);
            }
            $sum = $r1 + $r2 + $r3;
        }

        $lightColor = 0;

    } else {
        $r1 = min($r1, 210);
        $r2 = min($r2, 210);
        $r3 = min($r3, 210);

        while ($sum >= $border2) {
            my $choose = int rand 3;
            if ($choose == 0) {
                $r1 = max($r1 - 10, 1);
            } elsif ($choose == 1) {
                $r2 = max($r2 - 10, 1);
            } else {
                # $choose == 2
                $r3 = max($r3 - 10, 1);
            }

            $sum = $r1 + $r2 + $r3;
        }

        $lightColor = 1;
    }

    return sprintf("#%02x%02x%02x", $r1, $r2, $r3);
}

sub gen_dot_for_node ($$) {
    my ($out, $node) = @_;

    my $color = $colors{$node};
    my $label = gen_req_label($node);
    if ($node eq $active_req) {
        print $out qq{    r$node [shape="box" label="$label" color="black" fillcolor="$color"];\n};
    } else {
        print $out qq{    r$node [label="$label" fillcolor="$color"];\n};
    }
}

sub gen_dot () {
    my $outfile = "fr-$frame.dot";
    my $imgfile = "fr-$frame.png";
    open my $out, ">$outfile" or
        die "Cannot open $outfile for writing: $!\n";
    print $out <<_END_;
digraph frame_$frame {
    graph [center, label="Frame $frame", dpi=110,
        fontname="consolas"];
    edge [color=red, fontsize="13", fontcolor="#666666"];
    node [fillcolor="#f5f694", color="white",style=filled, fontname="consolas"];

_END_

    my %defined = ();
    my $i = 0;
    for my $edge (@edges) {
        my ($from, $to) = @$edge;
        #warn "from $from, to $to";
        if ($from ne '(nil)') {
            if (! $defined{$from}) {
                gen_dot_for_node($out, $from);
                $defined{$from} = 1;
            }
        }

        if (! $defined{$to}) {
            gen_dot_for_node($out, $to);
            $defined{$to} = 1;
        }

        if ($from ne '(nil)') {
            print $out qq{    r$from -> r$to [label="$i"];\n\n};
            $i++;
        }
    }

    print $out "}\n";

    close $out;
    print "$outfile generated.\n";
    system("dot -Tpng -o$imgfile $outfile") == 0
        or die "Failed to run dot\n";
}

sub gen_req_label ($) {
    my $id = shift;
    my $url = $urls{$id};
    my $method = $methods{$id};
    return "$method $url";
}

