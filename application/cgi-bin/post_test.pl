# Ref: https://www.perl.com/article/perl-and-cgi/

use strict;
use warnings;
use CGI;

my $cgi = CGI->new;
my %param = map { $_ => scalar $cgi->param($_) } $cgi->param() ;

print <<'END';

<!DOCTYPE html>
<html>
  <head>
    <title>POST Test</title>
  </head>
  <body>
    <!-- {form} -->
    <h1>POST Test Perl</h1>
END
for my $k ( sort keys %param ) {
    print join "<p> Key: ", $k, " Value: ", $param{$k}, "</p>";
}
print <<'END';
    <p>Last value was: {last_value}</p>
    <form method='post' action='post_test.py'>
      <label>Data to send: </label>
      <input type='text' placeholder='Enter data' name='data_to_send' required>
      <input type='submit' value="Submit data"/>
    </form>    
  </body>
</html>
END