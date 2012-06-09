
ABOUT
-----

This module is only intended to be used with the iOS application called Drupad
which is available on the Apple AppStore.

For more information about the application: http://drupad.com
For more information about the module itself: http://drupal.org/project/drupad
For bug reports, support or anything:
- Issue queue: http://drupal.org/project/issues/drupad
- Mail: drupad@breek.fr
- Twitter: @drupadapp

INSTALL
-------

1° Activate as any other module

2° Visit Configuration > System > Drupad to configure the module

3° Set the 'use Drupad application' permission

4° Launch the application from your iPhone or iPodTouch :-)


PERMISSIONS
-----------

Please not that Drupad is permissions aware. As such, any user
without proper permission like say 'perform backup' and 'access comments'
will NOT be able to 'view comments' NOR 'performing a backup' with backup &
migrate.

TROUBLESHOOTING
---------------

Some people have reported problems to log in to their site with the app.

It turns out that most of them are running Drupal on PHP - CGI environnement.

There are some workarounds to make it work, even though this is an effort that needs to continue.

Try the following, but read entirely before actually doing it:

1° Add one of these instructions in your Drupal root .htaccess file (at the bottom) just:
   - BEFORE the closing tag `</IfModule>`
   - and AFTER this instruction `RewriteRule ^(.*)$ index.php?q=$1 [L,QSA]`

   - please try CODE SNIPPET #1 first, if it works: that's it
     if CODE SNIPPET #1 doesn't work then try CODE SNIPPET #2

    # -- CODE SNIPPET #1 --
    RewriteRule .* - [E=HTTP_AUTHORIZATION:%{HTTP:Authorization},L]
    # -- CODE SNIPPET #1 --

    # -- CODE SNIPPET #2 --
    RewriteCond %{HTTP:Authorization}  !^$
    RewriteRule ^(.*)$ index.php?q=$1&login=%{HTTP:Authorization} [L,QSA]
    # -- CODE SNIPPET #2 --

2° Quit Drupad application, when I say quit, I mean KILL IT if you're in iOS 4
   You do that by:
   - leaving the Drupad app
   - double tapping the Home screen of your device
   - tap and hold the Drupad icon
   - tap the red minus sort of badge on the Drupad icon

3° Optionnally (safer), you could truncate your `sessions` table or whatever sessions mecanism you use.
   If you don't know how to do that, don't worry this shouldn't affect you if you
   never succeed to log in with the app.

4° Try again to login from the app. That's it.

If this doesn't work, try again but this time using CODE SNIPPET #2.
But please note, that this code snippet is less secure. Because your credentials
which are sent in the request headers from the application will be redirected and rewritten
as a HTTP GET parameter. Even though they aren't sent in clear (base 64 encoded, which is weak), they can
be logged by your webserver or whatever you server does, but you know more on that than me.

If you don't want to use this last method. Then we (you and me and the Drupal community) need to find
a better solution:
- use Apache mod_php rahter than CGI
- find and develop a whole new authentication mecanism

Please keep in mind, that this is a work in progress. And as many Drupad users can tell you I always
reply to users troubleshootings, as fast as I can handle it :-), see support and contact info.

Best regards,
Jérémy (jchatard)
http://drupal.org/user/130002