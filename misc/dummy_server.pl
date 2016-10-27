use 5.020;

use lib qw(./extlib/lib64/perl5/5.20.2/x86_64-linux);
use lib qw(./extlib/lib/perl5);
use feature 'signatures';
no warnings 'experimental::signatures';
no warnings 'experimental::smartmatch';

use DDP;
use Data::Dumper;

use Mojolicious::Lite;
use Bencode qw/bencode bdecode/;
use Crypt::CBC;
use Crypt::Cipher::AES;
use Digest::CRC qw/crc32/;
use Digest::MD5 qw/md5/;
use MIME::Base64;

get '*' => \&render;
get '/' => \&render;

sub render
{
	my $c = shift;
	my $data = [{remote_url => 'http://127.0.0.1:3000/', interval => 10}, ['ls', 'who', 'whoami']];
	my $bencoded_data = bencode($data);
	my $bencoded_crc32 = crc32($bencoded_data);
	my $encrypted_data = aes128encrypt($bencoded_crc32 . $bencoded_data);
	$c->render(text => $encrypted_data);
}

sub aes128encrypt
{
	my ($text, $key) = @_;
	my $cbc = Crypt::CBC->new(-cipher => 'Cipher::AES',
							  -key => md5($key),
							  -iv => pack('C*', map {0x00} 1..16),
							  -header => 'none',
							  -padding => 'standard',
							  -keysize => 16,
							  -literal_key => 1);
	encode_base64($cbc->encrypt($text), '');
}

# Start the Mojolicious command system
app->start;
