use 5.018;

use lib qw(extlib/lib/perl5);
use lib qw(/home/void/hs/misc/extlib/lib/perl5);

use List::Util;

use Mojolicious::Lite;

my $file_prefix = '.';
my @permit_id = qw/abcde fghij/;

get '/:foo' => sub
{
	my $c = shift;
	my $foo = $c->param('foo');
	if (List::Util::any { $_ eq $foo } @permit_id)
	{
		my $filename = "$file_prefix/$foo.txt";
		$c->render(text => read_file($filename));
		unlink $filename;
	}
	else
	{
		$c->render(text => '');
	}
};

post '/:foo' => sub
{
	my $c = shift;
	my $foo = $c->param('foo');
	my $data = $c->param('data');
	if ((List::Util::any { $_ eq $foo } @permit_id) and defined($data))
	{
		my $filename = "$file_prefix/$foo.txt";
		write_file($filename, $data);
	}
	$c->render(text => '');
};

sub read_file
{
	my $file = shift;
	local $/ = undef;
	open (my $fh, "<", $file) or return '';
	<$fh>;
}

sub write_file
{
	my $file = shift;
	my $data = shift;
	open (my $fh, ">", $file) or return;
	print $fh $data;
}

app->start;
