use 5.018;

use lib qw(extlib/lib/perl5);
use lib qw(/home/void/hs/misc/extlib/lib/perl5);
use lib qw(/home/void/stock/extlib/lib/perl5);

use Try::Tiny;

use Crypt::CBC;
use Crypt::Cipher::AES;
use MIME::Base64;
use Digest::MD5 qw(md5);

use IO::Compress::Gzip qw(gzip $GzipError);
use IO::Uncompress::Gunzip qw(gunzip $GunzipError);

#my $status = gzip $input => $output or die "gzip failed: $GzipError\n";
#my $status = gunzip $input => $output or die "gunzip failed: $GunzipError\n";

my $data = "abcdde";
my $gziped;
gzip \$data => \$gziped or die "gzip failed: $GzipError\n";
my $password = 'random key';
my $key = md5($password);

my $cbc = Crypt::CBC->new(-cipher => 'Cipher::AES',
	-key => $key,
	-iv => pack('C*', map {0x00} 1..16),
	-header => 'none',
	-padding => 'standard',
	-keysize => 16,
	-literal_key => 1);
say encode_base64($cbc->encrypt($gziped));

my $data2 = '1Golj+yoVySpQSHXjBw7NjdBfw5u6pJAP8PYp0YcJjo=';
my $gunziped;
my $data3 = $cbc->decrypt(decode_base64($data2));
gunzip \$data3 => \$gunziped or die "gunzip failed: $GunzipError\n";
say $gunziped;
