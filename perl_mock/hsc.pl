use 5.018;

use lib qw(extlib/lib/perl5);
use lib qw(/home/void/hs/misc/extlib/lib/perl5);

use LWP::UserAgent;
use Try::Tiny;

use Crypt::CBC;
use Crypt::Cipher::AES;
use MIME::Base64;

my $get_url = 'http://127.0.0.1:3000/fghij';
my $post_url = 'http://127.0.0.1:3000/abcde';

my $key = 'random key';

if ($ARGV[0] eq 'cmd')
{
	my $res;
	while (<STDIN>)
	{
		$res .= $_;
	}

	my $ua = LWP::UserAgent->new;
	$ua->timeout(10);

	my $cbc = Crypt::CBC->new(-cipher => 'Cipher::AES', -key => $key, -salt => 1);
	$res = encode_base64($cbc->encrypt($res), '');

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

		my $cbc = Crypt::CBC->new(-cipher => 'Cipher::AES', -key => $key, -salt => 1);
		say $cbc->decrypt(decode_base64($content));
	}
}
