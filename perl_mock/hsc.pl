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

my $get_url = 'http://127.0.0.1:3000/fghij';
my $post_url = 'http://127.0.0.1:3000/abcde';

my $key = md5('random key');

if ($ARGV[0] eq 'cmd')
{
	my $res;
	while (<STDIN>)
	{
		$res .= $_;
	}

	my $ua = LWP::UserAgent->new;
	$ua->timeout(10);

	my $cbc = Crypt::CBC->new(-cipher => 'Cipher::AES',
		-key => $key,
		-iv => pack('C*', map {0x00} 1..16),
		-header => 'none',
		-padding => 'standard',
		-keysize => 16,
		-literal_key => 1);
	my $gzipped;
	gzip \$res => \$gzipped or die "gzip failed: $GzipError\n";
	$res = encode_base64($cbc->encrypt($gzipped), '');

	$ua->post($post_url, {data => $res});
}
else
{
	my $ua = LWP::UserAgent->new;
	$ua->timeout(10);

	my $response = $ua->get($get_url);
	if ($response->is_success)
	{
		my $content = $response->decoded_content;
		exit if ($content eq '');

		my $cbc = Crypt::CBC->new(-cipher => 'Cipher::AES',
			-key => $key,
			-iv => pack('C*', map {0x00} 1..16),
			-header => 'none',
			-padding => 'standard',
			-keysize => 16,
			-literal_key => 1);
		my $content = $cbc->decrypt(decode_base64($content));
		my $gunzipped;
		gunzip \$content => \$gunzipped or die "gunzip failed: $GunzipError\n";
		say $gunzipped;
	}
}
